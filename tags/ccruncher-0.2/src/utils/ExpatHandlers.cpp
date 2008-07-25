
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
// ExpatHandlers.cpp - ExpatHandlers code
// --------------------------------------------------------------------------
//
// 2005/03/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cstdio>
#include <cassert>
#include <cstring>
#include <expat.h>
#include "utils/ExpatHandlers.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// destructor
//===========================================================================
ccruncher::ExpatHandlers::~ExpatHandlers()
{
  // nothing to do
}
    
//===========================================================================
// throw an error
//===========================================================================
Exception ccruncher::ExpatHandlers::eperror(ExpatUserData &eud, const string &msg)
{
  XML_Parser xmlparser = eud.getParser();
  char buf[256];

  sprintf(buf, "%s at line %d and column %d", 
               msg.c_str(),
               XML_GetCurrentLineNumber(xmlparser),
               XML_GetCurrentColumnNumber(xmlparser));

  return Exception(string(buf));
}

//===========================================================================
// epback
//===========================================================================
void ccruncher::ExpatHandlers::epback(ExpatUserData &eud)
{
  // assertions
  assert(eud.getCurrentHandlers() == this);

  // removing this handlers from stack
  eud.removeCurrentHandlers();
}

//===========================================================================
// eppush
//===========================================================================
void ccruncher::ExpatHandlers::eppush(ExpatUserData &eud, 
                  ExpatHandlers *eh, const char *name, const char **atts)
{
  // assertion
  assert(eud.getCurrentHandlers() == this);

  // adding new handlers to stack
  eud.setCurrentHandlers(string(name), eh);

  // calling new handler
  eh->epstart(eud, name, atts);
}

//===========================================================================
// getAttributeValue
//===========================================================================
char * ccruncher::ExpatHandlers::getAttributeValue(const char **atts, const string &attname) const 
{
  for (int i=0; atts[i]; i+=2) 
  {
    if (strcmp(attname.c_str(),atts[i]) == 0)
    {
      return (char *) atts[i+1];
    }
  }

  return NULL;
}

//===========================================================================
// getStringAttribute
//===========================================================================
string ccruncher::ExpatHandlers::getStringAttribute(const char **atts, const string &attname, const string &defval)
{
  char *val = getAttributeValue(atts, attname);
  
  if (val != NULL)
  {
    return string(val);
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getIntAttribute
//===========================================================================
int ccruncher::ExpatHandlers::getIntAttribute(const char **atts, const string &attname, const int &defval)
{
  char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    try
    {
      return Parser::intValue(val);
    }
    catch(Exception &e)
    {
      return defval;
    }
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getLongAttribute
//===========================================================================
long ccruncher::ExpatHandlers::getLongAttribute(const char **atts, const string &attname, const long &defval)
{
  char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    try
    {
      return Parser::longValue(val);
    }
    catch(Exception &e)
    {
      return defval;
    }
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getDoubleAttribute
//===========================================================================
double ccruncher::ExpatHandlers::getDoubleAttribute(const char **atts, const string &attname, const double &defval)
{
  char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    try
    {
      return Parser::doubleValue(val);
    }
    catch(Exception &e)
    {
      return defval;
    }
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getDateAttribute
//===========================================================================
Date ccruncher::ExpatHandlers::getDateAttribute(const char **atts, const string &attname, const Date &defval)
{
  char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    try
    {
      return Parser::dateValue(val);
    }
    catch(Exception &e)
    {
      return defval;
    }
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getBooleanAttribute
//===========================================================================
bool ccruncher::ExpatHandlers::getBooleanAttribute(const char **atts, const string &attname, const bool &defval)
{
  char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    try
    {
      return Parser::boolValue(val);
    }
    catch(Exception &e)
    {
      return defval;
    }
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// isEqual
//===========================================================================
bool ccruncher::ExpatHandlers::isEqual(const char *pchr, const string &str)
{
  if (strcmp(str.c_str(), pchr) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//===========================================================================
// getNumAttributes
//===========================================================================
int ccruncher::ExpatHandlers::getNumAttributes(const char **atts)
{
  int ret = 0;

  for (int i=0; atts[i]; i+=2) 
  {
    ret++;
  }

  return ret;
}
