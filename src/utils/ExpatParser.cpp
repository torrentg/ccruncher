
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//===========================================================================

#include <cassert>
#include "utils/ExpatParser.hpp"
#include "utils/Exception.hpp"

#define EXPAT_BUFFER_SIZE 16*1024

// expat versions previous to 1.95.x don't have defined these macros
#ifndef XML_STATUS_OK
#define XML_STATUS_OK    1
#define XML_STATUS_ERROR 0
#endif

using namespace std;

/**************************************************************************/
ccruncher::ExpatParser::ExpatParser() : mXmlParser(nullptr), mChecksum(0UL), handlers(nullptr)
{
  // nothing to do
}

/**************************************************************************/
ccruncher::ExpatParser::~ExpatParser()
{
  if (mXmlParser != nullptr) {
    XML_ParserFree(mXmlParser);
  }
}

/**************************************************************************//**
 * @details Instantiate the expat parser and assigns the default handlers.
 *          These new handlers will call the the appropiate methods based
 *          on the stack content.
 */
void ccruncher::ExpatParser::reset()
{
  if (mXmlParser != nullptr) {
    XML_ParserFree(mXmlParser);
    mXmlParser = nullptr;
  }

  // creating parser object
  mXmlParser = XML_ParserCreate(nullptr);
  assert(mXmlParser != nullptr);

  // setting userData
  XML_SetUserData(mXmlParser, this);

  // setting element handlers
  XML_SetElementHandler(mXmlParser, startElement, endElement);

  // setting characterdata handler
  XML_SetCharacterDataHandler(mXmlParser, characterData);

  // checksum value initialization
  mChecksum = adler32(0L, Z_NULL, 0);
}

/**************************************************************************//**
 * @details Apply macros to attributes values and calls the method
 *          ExpatHandlers#epstart of the object that is in the top of the
 *          stack.
 * @param[in] eud ExpatParser pointer.
 * @param[in] tag Element's name.
 * @param[in] atts Element's attributes.
 */
void ccruncher::ExpatParser::startElement(void *eud, const char *tag, const char **atts)
{
  // retrieving current parser
  assert(eud != nullptr);
  ExpatParser *expatParser = static_cast<ExpatParser *>(eud);

  // calling current handler
  assert(expatParser->handlers != nullptr);
  expatParser->handlers->epstart(tag, atts);
}

/**************************************************************************//**
 * @details Calls the method ExpatHandlers#epend of the object that is in
 *          the top of the stack. If the ending element match the current
 *          ExpatHandler element (top in stack), then removes this object
 *          from stack.
 * @param[in] eud ExpatParser pointer.
 * @param[in] tag Element's name.
 */
void ccruncher::ExpatParser::endElement(void *eud, const char *tag)
{
  // retrieving current parser
  assert(eud != nullptr);
  ExpatParser *expatParser = static_cast<ExpatParser *>(eud);

  // calling current handler
  assert(expatParser->handlers != nullptr);
  expatParser->handlers->epend(tag);
}

/**************************************************************************//**
 * @details Calls the method ExpatHandlers#epdata of the object that is in
 *          the top of the stack.
 * @param[in] eud ExpatUserData object.
 * @param[in] cdata Fragment of data.
 * @param[in] len Length of the current data fragment.
 */
void ccruncher::ExpatParser::characterData(void *eud, const char *cdata, int len)
{
  // retrieving current parser
  assert(eud != nullptr);
  ExpatParser *expatParser = static_cast<ExpatParser *>(eud);

  // calling current handler
  assert(expatParser->handlers != nullptr);
  expatParser->handlers->epdata(cdata, len);
}

/**************************************************************************//**
 * @details This method is useful to do tests an parse small XMLs.
 *          Support to stop parsing changing the value of the variable
 *          <code>stop</code>.
 * @param[in] xmlcontent XML content.
 * @param[in] eh ExpatHandlers to use.
 * @param[in] stop Variable to stop parser from outside.
 * @throw Exception Error parsing string content.
 */
void ccruncher::ExpatParser::parse(const std::string &xmlcontent, ExpatHandlers *eh, bool *stop)
{
  if (eh == nullptr) {
    throw Exception("invalid xml handlers");
  }
  handlers = eh;
  parse(nullptr, const_cast<char*>(xmlcontent.c_str()), xmlcontent.length(), stop);
}

/**************************************************************************//**
 * @details Parse an XML file that can be gziped. Support to stop parsing
 *          changing the value of the variable <code>stop</code>.
 * @see http://www.zlib.net/
 * @param[in] file XML file.
 * @param[in] eh ExpatHandlers to use.
 * @param[in] stop Variable to stop parser from outside.
 * @throw Exception Error parsing file content.
 */
void ccruncher::ExpatParser::parse(gzFile file, ExpatHandlers *eh, bool *stop)
{
  if (eh == nullptr) {
    throw Exception("invalid xml handlers");
  }
  handlers = eh;
  char buf[EXPAT_BUFFER_SIZE] = {0};
  parse(file, buf, EXPAT_BUFFER_SIZE, stop);
}

/**************************************************************************//**
 * @details Internal method to parse an XML content.
 * @param[in] file XML file.
 * @param[in] buf Buffer to use in the XML parsing. If file is NULL then
 *            buffer must contain the XML content to be parsed.
 * @param[in] buffer_size Buffer size.
 * @param[in] stop Variable to stop parser from outside.
 * @throw Exception Error parsing XML content.
 */
void ccruncher::ExpatParser::parse(gzFile file, char *buf, size_t buffer_size, bool *stop)
{
  assert(buf != nullptr);
  assert(buffer_size > 0);

  try
  {
    int len=0;
    int done=0;

    reset();

    do
    {
      if (stop != nullptr && *stop == true) {
        throw int(999);
      }

      if (file != nullptr) {
        len = gzread(file, buf, buffer_size);
        done = (len < (int)buffer_size);
      }
      else {
        len = buffer_size;
        done = true;
      }

      if (len > 0)
      {
        mChecksum = adler32(mChecksum, (Bytef*)(buf), len);

        if (XML_Parse(mXmlParser, buf, len, done) == XML_STATUS_ERROR) {
          throw Exception(string(XML_ErrorString(XML_GetErrorCode(mXmlParser))));
        }
      }
    } while(!done);
  }
  catch(int spe)
  {
    // stop parser request
    if (spe == 999) {
      // nothing to do
    }
    // unknow exception
    else {
      throw Exception(getErrorLocation());
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, getErrorLocation());
  }
  catch(...)
  {
    throw Exception(getErrorLocation());
  }
}

/**************************************************************************//**
 * @return String message containing the error location.
 */
string ccruncher::ExpatParser::getErrorLocation() const
{
  return "error at line " + to_string(XML_GetCurrentLineNumber(mXmlParser)) +
         " column " + to_string(XML_GetCurrentColumnNumber(mXmlParser));
}

/**************************************************************************//**
 * @return The checksum (adler32) of the XML content.
 */
unsigned long ccruncher::ExpatParser::getChecksum() const
{
  return mChecksum;
}

