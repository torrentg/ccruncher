
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

#include <cstdio>
#include <expat.h>
#include "utils/ExpatHandlers.hpp"
#include "utils/Parser.hpp"
#include <cassert>

//===========================================================================
// destructor
//===========================================================================
ccruncher::ExpatHandlers::~ExpatHandlers()
{
  // nothing to do
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
  eud.setCurrentHandlers(name, eh);

  // calling new handler
  eh->epstart(eud, name, atts);
}

//===========================================================================
// epstop
//===========================================================================
void ccruncher::ExpatHandlers::epstop(ExpatUserData &)
{
  // throwing an exception to stop parser
  // retrieved by ExpatParser::parse() method
  throw int(999);
}

//===========================================================================
// epdata
//===========================================================================
void ccruncher::ExpatHandlers::epdata(ExpatUserData &, const char *, const char *s, int len)
{
  // default simple rule: character data is not allowed
  for(int i=0; i<len; i++)
  {
    if (s[i] != ' ' && s[i] != '\n' && s[i] != '\t')
    {
      throw Exception("unexpected text parsing xml");
    }
  }
}

//===========================================================================
// getAttributeValue
//===========================================================================
const char * ccruncher::ExpatHandlers::getAttributeValue(const char **atts, const string &attname) const
{
  for (int i=0; atts[i]; i+=2)
  {
    if (strcmp(attname.c_str(),atts[i]) == 0)
    {
      return atts[i+1];
    }
  }

  return NULL;
}

//===========================================================================
// getStringAttribute
//===========================================================================
string ccruncher::ExpatHandlers::getStringAttribute(const char **atts, const string &attname) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return string(val);
  }
  else
  {
    throw Exception("attribute '" + attname + "' not found");
  }
}

//===========================================================================
// getStringAttribute
//===========================================================================
string ccruncher::ExpatHandlers::getStringAttribute(const char **atts, const string &attname, const string &defval) const
{
  const char *val = getAttributeValue(atts, attname);

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
int ccruncher::ExpatHandlers::getIntAttribute(const char **atts, const string &attname) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::intValue(val);
  }
  else
  {
    throw Exception("attribute '" + attname + "' not found");
  }
}

//===========================================================================
// getIntAttribute
//===========================================================================
int ccruncher::ExpatHandlers::getIntAttribute(const char **atts, const string &attname, int defval) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::intValue(val);
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getLongAttribute
//===========================================================================
long ccruncher::ExpatHandlers::getLongAttribute(const char **atts, const string &attname) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::longValue(val);
  }
  else
  {
    throw Exception("attribute '" + attname + "' not found");
  }
}

//===========================================================================
// getLongAttribute
//===========================================================================
long ccruncher::ExpatHandlers::getLongAttribute(const char **atts, const string &attname, long defval) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::longValue(val);
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getDoubleAttribute
//===========================================================================
double ccruncher::ExpatHandlers::getDoubleAttribute(const char **atts, const string &attname) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::doubleValue(val);
  }
  else
  {
    throw Exception("attribute '" + attname + "' not found");
  }
}

//===========================================================================
// getDoubleAttribute
//===========================================================================
double ccruncher::ExpatHandlers::getDoubleAttribute(const char **atts, const string &attname, double defval) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::doubleValue(val);
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getDateAttribute
//===========================================================================
Date ccruncher::ExpatHandlers::getDateAttribute(const char **atts, const string &attname) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::dateValue(val);
  }
  else
  {
    throw Exception("attribute '" + attname + "' not found");
  }
}

//===========================================================================
// getDateAttribute
//===========================================================================
Date ccruncher::ExpatHandlers::getDateAttribute(const char **atts, const string &attname, const Date &defval) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::dateValue(val);
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getBooleanAttribute
//===========================================================================
bool ccruncher::ExpatHandlers::getBooleanAttribute(const char **atts, const string &attname) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::boolValue(val);
  }
  else
  {
    throw Exception("attribute '" + attname + "' not found");
  }
}
//===========================================================================
// getBooleanAttribute
//===========================================================================
bool ccruncher::ExpatHandlers::getBooleanAttribute(const char **atts, const string &attname, bool defval) const
{
  const char *val = getAttributeValue(atts, attname);

  if (val != NULL)
  {
    return Parser::boolValue(val);
  }
  else
  {
    return defval;
  }
}

//===========================================================================
// getNumAttributes
//===========================================================================
int ccruncher::ExpatHandlers::getNumAttributes(const char **atts) const
{
  int ret = 0;

  for (int i=0; atts[i]; i+=2)
  {
    ret++;
  }

  return ret;
}
