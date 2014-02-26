
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#include <cstring>
#include <cstdio>
#include <cctype>
#include <sstream>
#include <cassert>
#include "utils/ExpatParser.hpp"
#include "utils/ExpatUserData.hpp"

#define BUFFER_SIZE 32*1024

// expat versions previous to 1.95.x don't have defined these macros
#ifndef XML_STATUS_OK
#define XML_STATUS_OK    1
#define XML_STATUS_ERROR 0
#endif

using namespace std;

/**************************************************************************/
ccruncher::ExpatParser::ExpatParser() : userdata(4000), checksum(0UL)
{
  xmlparser = NULL;
}

/**************************************************************************/
ccruncher::ExpatParser::~ExpatParser()
{
  if (xmlparser != NULL) {
    XML_ParserFree(xmlparser);
  }
}

/**************************************************************************//**
 * @details Instantiate the expat parser and assigns the default handlers.
 *          These new handlers will call the the appropiate methods based
 *          on the stack content.
 */
void ccruncher::ExpatParser::reset()
{
  if (xmlparser != NULL) {
    XML_ParserFree(xmlparser);
    xmlparser = NULL;
  }

  // creating parser object
  xmlparser = XML_ParserCreate(NULL);
  assert(xmlparser != NULL);

  // setting userData
  XML_SetUserData(xmlparser, &userdata);

  // setting element handlers
  XML_SetElementHandler(xmlparser, startElement, endElement);

  // setting characterdata handler
  XML_SetCharacterDataHandler(xmlparser, characterData);

  // checksum value initialization
  checksum = adler32(0L, Z_NULL, 0);
}

/**************************************************************************//**
 * @details Apply defines to attributes values and calls the method
 *          ExpatHandlers#epstart of the object that is in the top of the
 *          stack.
 * @param[in] ud_ ExpatUserData object.
 * @param[in] name Element's name.
 * @param[in] atts Element's attributes.
 */
void ccruncher::ExpatParser::startElement(void *ud_, const char *name, const char **atts)
{
  // setting current tag
  ExpatUserData *ud = static_cast<ExpatUserData *>(ud_);
  ud->setCurrentTag(name);

  // replacing defines
  if (ud->defines.size() > 0)
  {
    for (int i=0; atts[i]; i+=2)
    {
      atts[i+1] = ud->applyDefines(atts[i+1]);
    }
  }

  // calling current handler
  ExpatHandlers *eh = ud->getCurrentHandlers();
  eh->epstart(*ud, name, atts);
}

/**************************************************************************//**
 * @details Calls the method ExpatHandlers#epend of the object that is in
 *          the top of the stack. If the ending element match the current
 *          ExpatHandler element (top in stack), then removes this object
 *          from stack.
 * @param[in] ud_ ExpatUserData object.
 * @param[in] name Element's name.
 */
void ccruncher::ExpatParser::endElement(void *ud_, const char *name)
{
  // retrieving current handler
  ExpatUserData *ud = static_cast<ExpatUserData *>(ud_);
  ExpatHandlers *eh = ud->getCurrentHandlers();

  // setting current tag
  ud->setCurrentTag(NULL);

  // calling current handler
  eh->epend(*ud, name);

  // if current handler = name then set new handler
  if (strcmp(name,ud->getCurrentName()) == 0)
  {
    // restore parent handlers
    ud->removeCurrentHandlers();
    eh = ud->getCurrentHandlers();
    // notify that children finished
    eh->epend(*ud, name);
  }
}

/**************************************************************************//**
 * @details Calls the method ExpatHandlers#epdata of the object that is in
 *          the top of the stack.
 * @param[in] ud_ ExpatUserData object.
 * @param[in] cdata Fragment of data.
 * @param[in] len Length of the current data fragment.
 */
void ccruncher::ExpatParser::characterData(void *ud_, const char *cdata, int len)
{
  ExpatUserData *ud = static_cast<ExpatUserData *>(ud_);
  ExpatHandlers *eh = ud->getCurrentHandlers();
  eh->epdata(*ud, ud->getCurrentTag(), cdata, len);
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
void ccruncher::ExpatParser::parse(const std::string &xmlcontent, ExpatHandlers *eh, bool *stop) throw(Exception)
{
  reset();
  userdata.setCurrentHandlers("", eh);
  parse(NULL, const_cast<char*>(xmlcontent.c_str()), xmlcontent.length(), stop);
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
void ccruncher::ExpatParser::parse(gzFile file, ExpatHandlers *eh, bool *stop) throw(Exception)
{
  reset();
  userdata.setCurrentHandlers("", eh);
  char buf[BUFFER_SIZE];
  buf[0] = 0;
  parse(file, buf, BUFFER_SIZE, stop);
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
void ccruncher::ExpatParser::parse(gzFile file, char *buf, size_t buffer_size, bool *stop) throw(Exception)
{
  assert(buf != NULL);
  assert(buffer_size > 0);

  try
  {
    int len=0;
    int done;

    do
    {
      if (stop != NULL && *stop == true) {
        throw Exception("parser stopped");
      }

      if (file != NULL) {
        len = gzread(file, buf, buffer_size);
        done = (len < (int)buffer_size);
      }
      else {
        len = buffer_size;
        done = true;
      }

      if (len > 0)
      {
        checksum = adler32(checksum, (Bytef*)(buf), len);

        if (XML_Parse(xmlparser, buf, len, done) == XML_STATUS_ERROR)
        {
          char aux[512];
          snprintf(aux, 512, "%s at line %d column %d",
                     XML_ErrorString(XML_GetErrorCode(xmlparser)),
                     (int) XML_GetCurrentLineNumber(xmlparser),
                     (int) XML_GetCurrentColumnNumber(xmlparser));
          throw Exception(string(aux));
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
      char aux[512];
      snprintf(aux, 512, "error at line %d column %d",
                 (int) XML_GetCurrentLineNumber(xmlparser),
                 (int) XML_GetCurrentColumnNumber(xmlparser));
      throw Exception(string(aux));
    }
  }
  catch(std::exception &e)
  {
    char aux[512];
    snprintf(aux, 512, "error at line %d column %d",
                 (int) XML_GetCurrentLineNumber(xmlparser),
                 (int) XML_GetCurrentColumnNumber(xmlparser));
    throw Exception(e, string(aux));
  }
  catch(...)
  {
    char aux[512];
    snprintf(aux, 512, "error at line %d column %d",
                 (int) XML_GetCurrentLineNumber(xmlparser),
                 (int) XML_GetCurrentColumnNumber(xmlparser));
    throw Exception(string(aux));
  }
}

/**************************************************************************//**
 * @return The map of defines (name-value).
 */
const map<string,string>& ccruncher::ExpatParser::getDefines() const
{
  return userdata.defines;
}

/**************************************************************************//**
 * @param[in] m Map of defines (name-value).
 */
void ccruncher::ExpatParser::setDefines(const std::map<std::string,std::string> &m)
{
  userdata.defines = m;
}

/**************************************************************************//**
 * @return The checksum (adler32) of the XML content (previous to defines
 *         replacement).
 */
unsigned long ccruncher::ExpatParser::getChecksum() const
{
  return checksum;
}

