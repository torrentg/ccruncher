
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
//
// ExpatParser.cpp - ExpatParser code
// --------------------------------------------------------------------------
//
// 2005/03/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cassert>
#include <cstring>
#include <map>
#include <cstdio>
#include "ExpatParser.hpp"
#include "ExpatUserData.hpp"

// --------------------------------------------------------------------------

#define BUFSIZE 32768

//===========================================================================
// constructor
//===========================================================================
ccruncher::ExpatParser::ExpatParser()
{
  // creating parser object
  xmlparser = XML_ParserCreate(NULL);
  assert(xmlparser != NULL);

  // creating userData
  userdata = ExpatUserData::ExpatUserData(xmlparser);
  XML_SetUserData(xmlparser, &userdata);

  // setting element handlers
  XML_SetElementHandler(xmlparser, startElement, endElement);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::ExpatParser::~ExpatParser()
{
  XML_ParserFree(xmlparser);
}

//===========================================================================
// startElement
//===========================================================================
void XMLCALL ccruncher::ExpatParser::startElement(void *ud_, const char *name, const char **atts)
{
  // retrieving current handler
  ExpatUserData *ud = (ExpatUserData *) ud_;
  ExpatHandlers *eh = ud->getCurrentHandlers();

  // calling current handler
  eh->epstart(*ud, name, atts);
}

//===========================================================================
// endElement
//===========================================================================
void XMLCALL ccruncher::ExpatParser::endElement(void *ud_, const char *name)
{
  // retrieving current handler
  ExpatUserData *ud = (ExpatUserData *) ud_;
  ExpatHandlers *eh = ud->getCurrentHandlers();

  // if current handler = name use epend as finish event
  // calling current handler
  eh->epend(*ud, name);

  // checking handlers domain
  if (strcmp(name,ud->getCurrentName().c_str()) == 0)
  {
    // restore parent handlers
    ud->removeCurrentHandlers();
    eh = ud->getCurrentHandlers();
    // notify that children finished
    eh->epend(*ud, name);
  }
}

//===========================================================================
// parse
//===========================================================================
void ccruncher::ExpatParser::parse(string xmlcontent, ExpatHandlers *eh) throw(Exception)
{
  // pushing handlers to stack  
  userdata.setCurrentHandlers("", eh);
  
  // parsing doc
  try
  {
    if (XML_Parse(xmlparser, xmlcontent.c_str(), xmlcontent.length(), 1) == XML_STATUS_ERROR)
    {
      char buf[512];
      sprintf(buf, "ExpatParser::parse(): %s at line %d at column %d", 
                   XML_ErrorString(XML_GetErrorCode(xmlparser)),
                   XML_GetCurrentLineNumber(xmlparser),
                   XML_GetCurrentColumnNumber(xmlparser));
      throw Exception(string(buf));
    }
  }
  catch(Exception &e)
  {
    throw e;
  }
  catch(...)
  {
    throw Exception("ExpatParsers::parse()");
  }
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
        sprintf(aux, "ExpatParser::parse(): %s at line %d at column %d", 
                     XML_ErrorString(XML_GetErrorCode(xmlparser)),
                     XML_GetCurrentLineNumber(xmlparser),
                     XML_GetCurrentColumnNumber(xmlparser));
        throw Exception(string(aux));
      }
    } while(!done);
  }
  catch(Exception &e)
  {
    throw e;
  }
  catch(...)
  {
    throw Exception("ExpatParsers::parse()");
  }
}

//===========================================================================
// returns main object from stack
//===========================================================================
void * ccruncher::ExpatParser::getObject()
{
  return userdata.getCurrentHandlers();
}
