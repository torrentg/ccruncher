
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
#include <sstream>
#include "utils/ExpatParser.hpp"
#include "utils/ExpatUserData.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define BUFSIZE 32768

// expat versions previous to 1.95.x don't have defined these macros
#ifndef XML_STATUS_OK
#define XML_STATUS_OK    1
#define XML_STATUS_ERROR 0
#endif

//===========================================================================
// static variable definition
//===========================================================================
//TODO: declare this variable as non-static
const char *ccruncher::ExpatParser::current_tag = NULL;

//===========================================================================
// constructor
//===========================================================================
ccruncher::ExpatParser::ExpatParser()
{
  xmlparser = NULL;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::ExpatParser::~ExpatParser()
{
  if (xmlparser != NULL) {
    XML_ParserFree(xmlparser);
  }
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::ExpatParser::reset()
{
  if (xmlparser != NULL) {
    XML_ParserFree(xmlparser);
  }

  // creating parser object
  xmlparser = XML_ParserCreate(NULL);
  assert(xmlparser != NULL);

  // creating userData
  userdata = ExpatUserData(xmlparser);
  XML_SetUserData(xmlparser, &userdata);

  // setting element handlers
  XML_SetElementHandler(xmlparser, startElement, endElement);

  // setting characterdata handler
  XML_SetCharacterDataHandler(xmlparser, characterData);

  // other initializations
  current_tag = NULL;
}

//===========================================================================
// startElement
//TODO: avoid usage of static variables
//===========================================================================
void ccruncher::ExpatParser::startElement(void *ud_, const char *name, const char **atts)
{
  // setting current tag
  current_tag = name;

  // retrieving current handler
  ExpatUserData *ud = (ExpatUserData *) ud_;
  ExpatHandlers *eh = ud->getCurrentHandlers();

  // calling current handler
  eh->epstart(*ud, name, atts);
}

//===========================================================================
// endElement
//TODO: avoid usage of static variables
//===========================================================================
void ccruncher::ExpatParser::endElement(void *ud_, const char *name)
{
  // setting current tag
  current_tag = NULL;

  // retrieving current handler
  ExpatUserData *ud = (ExpatUserData *) ud_;
  ExpatHandlers *eh = ud->getCurrentHandlers();

  // if current handler = name use epend as finish event
  // calling current handler
  eh->epend(*ud, name);

  // checking handlers domain
  if (strcmp(name,ud->getCurrentName()) == 0)
  {
    // restore parent handlers
    ud->removeCurrentHandlers();
    eh = ud->getCurrentHandlers();
    // notify that children finished
    eh->epend(*ud, name);
  }
}

//===========================================================================
// characterData
//TODO: avoid usage of static variables
//===========================================================================
void ccruncher::ExpatParser::characterData(void *ud_, const char *s, int len) throw(Exception)
{
  ExpatUserData *ud = (ExpatUserData *) ud_;
  ExpatHandlers *eh = ud->getCurrentHandlers();
  eh->epdata(*ud, current_tag, s, len);
}

//===========================================================================
// parse
//===========================================================================
void ccruncher::ExpatParser::parse(const string &xmlcontent, ExpatHandlers *eh) throw(Exception)
{
  istringstream iss (xmlcontent, istringstream::in);
  parse(iss, eh);
}

//===========================================================================
// parse
//===========================================================================
void ccruncher::ExpatParser::parse(istream &xmlcontent, ExpatHandlers *eh) throw(Exception)
{
  char buf[BUFSIZE+1];
  streamsize len=0;
  int done;

  // pushing handlers to stack
  reset();
  userdata.setCurrentHandlers("", eh);

  // parsing doc
  try
  {
    do
    {
      xmlcontent.read(buf, BUFSIZE);
      len = xmlcontent.gcount();
      buf[len] = 0;
      done = (len < BUFSIZE);

      if (XML_Parse(xmlparser, buf, len, done) == XML_STATUS_ERROR)
      {
        char aux[512];
        sprintf(aux, "%s at line %d column %d",
                     XML_ErrorString(XML_GetErrorCode(xmlparser)),
                     (int) XML_GetCurrentLineNumber(xmlparser),
                     (int) XML_GetCurrentColumnNumber(xmlparser));
        throw Exception(string(aux));
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
      sprintf(aux, "error at line %d column %d",
                 (int) XML_GetCurrentLineNumber(xmlparser),
                 (int) XML_GetCurrentColumnNumber(xmlparser));
      throw Exception(string(aux));
    }
  }
  catch(Exception &e)
  {
    char aux[512];
    sprintf(aux, "%s at line %d column %d",
                 e.what(),
                 (int) XML_GetCurrentLineNumber(xmlparser),
                 (int) XML_GetCurrentColumnNumber(xmlparser));
    throw Exception(string(aux));
  }
  catch(...)
  {
    char aux[512];
    sprintf(aux, "error at line %d column %d",
                 (int) XML_GetCurrentLineNumber(xmlparser),
                 (int) XML_GetCurrentColumnNumber(xmlparser));
    throw Exception(string(aux));
  }
}

//===========================================================================
// returns main object from stack
//===========================================================================
void * ccruncher::ExpatParser::getObject()
{
  return userdata.getCurrentHandlers();
}
