
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

#include <cstdio>
#include <cctype>
#include <cassert>
#include <expat.h>
#include "utils/ExpatHandlers.hpp"
#include "utils/Parser.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @details Removes current handlers from handlers' stack. Handlers previous
 *          to current handlers takes the XML parser control.
 *          Method name prefix 'ep' comes from Expat Parser.
 * @see ExpatUserData
 * @param[in] eud Parser data shared by handlers.
 */
void ccruncher::ExpatHandlers::epback(ExpatUserData &eud)
{
  assert(eud.getCurrentHandlers() == this);

  // removing this handlers from stack
  eud.removeCurrentHandlers();
}

/**************************************************************************//**
 * @details Push new handlers to the handlers' stack. Current handlers are
 *          postponed until the task completion of the new handlers. This
 *          method is called when a new element appears and we have a class
 *          derived from ExpatHandlers than can parse this element. New
 *          handlers are automatically called to process the new element.
 *          Method name prefix 'ep' comes from Expat Parser.
 * @see ExpatUserData
 * @param[in] eud Parser data shared by handlers.
 * @param[in] eh Object inherited from ExpatHandlers.
 * @param[in] tag XML element name related to the new handlers.
 * @param[in] atts Attributes to pass to process the new element.
 */
void ccruncher::ExpatHandlers::eppush(ExpatUserData &eud,
    ExpatHandlers *eh, const char *tag, const char **atts)
{
  assert(eud.getCurrentHandlers() == this);

  // adding new handlers to stack
  eud.setCurrentHandlers(tag, eh);

  // calling new handler
  eh->epstart(eud, tag, atts);
}

/**************************************************************************//**
 * @details Process data into an element. Override this method to catch
 *          element's data info. Default action only allows spaces.
 * @see     IData#epdata as example.
 * @param[in] eud Parser data shared by handlers.
 * @param[in] tag Element's name.
 * @param[in] cdata Fragment of data.
 * @param[in] len Length of the current data fragment.
 */
void ccruncher::ExpatHandlers::epdata(ExpatUserData &/*eud*/, const char */*tag*/,
    const char *cdata, int len)
{
  // default simple rule: character data is not allowed
  for(int i=0; i<len; i++)
  {
    if (!isspace(cdata[i]))
    {
      throw Exception("unexpected text parsing xml: '" + string(cdata, len) + "'");
    }
  }
}

/**************************************************************************//**
 * @details This method has an unused param. It is required to maintains a
 *          common interface with the other ep* methods.
 *          Method name prefix 'ep' comes from Expat Parser.
 */
void ccruncher::ExpatHandlers::epstop(ExpatUserData &)
{
  // throwing an exception that will be catched by ExpatParser::parse()
  throw int(999);
}


/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as a char array.
 *          If attribute is not found then returns an exception.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @return Attribute value as char array.
 * @throw Exception If attribute is not found in attributes list.
 */
const char * ccruncher::ExpatHandlers::getAttributeValue(const char **atts, const char *attname) const
{
  assert(atts != nullptr);
  assert(attname != nullptr);

  for(int i=0; atts[i]; i+=2)
  {
    if (strcmp(attname,atts[i]) == 0)
    {
      return atts[i+1];
    }
  }

  throw Exception("attribute '" + string(attname) + "' not found");
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as a char array.
 *          If attribute is not found then returns the default value.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @param[in] defval Attribute default value.
 * @return If attribute exist returns attribute value as char array, otherwise
 *         returns default value.
 */
const char * ccruncher::ExpatHandlers::getAttributeValue(const char **atts, const char *attname, const char *defval) const
{
  assert(atts != nullptr);
  assert(attname != nullptr);

  for(int i=0; atts[i]; i+=2)
  {
    if (strcmp(attname,atts[i]) == 0)
    {
      return atts[i+1];
    }
  }

  return defval;
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as a string.
 *          If attribute is not found then returns an exception.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @return Attribute value as string.
 * @throw Exception If attribute is not found in attributes list.
 */
string ccruncher::ExpatHandlers::getStringAttribute(const char **atts, const char *attname) const
{
  const char *val = getAttributeValue(atts, attname);
  return string(val);
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as string.
 *          If attribute is not found then returns the default value.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @param[in] defval Attribute default value.
 * @return If attribute exist returns attribute value as string, otherwise
 *         returns default value.
 */
string ccruncher::ExpatHandlers::getStringAttribute(const char **atts, const char *attname, const std::string &defval) const
{
  const char *val = getAttributeValue(atts, attname, nullptr);

  if (val != nullptr)
  {
    return string(val);
  }
  else
  {
    return defval;
  }
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as a int.
 *          If attribute is not found then returns an exception.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @return Attribute value as int.
 * @throw Exception If attribute is not found in attributes list.
 * @throw Exception If attribute value is not an int (see Parser#intValue).
 */
int ccruncher::ExpatHandlers::getIntAttribute(const char **atts, const char *attname) const
{
  const char *val = getAttributeValue(atts, attname);
  return Parser::intValue(val);
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as int.
 *          If attribute is not found then returns the default value.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @param[in] defval Attribute default value.
 * @return If attribute exist returns attribute value as int, otherwise
 *         returns default value.
 * @throw Exception If attribute value is not an int (see Parser#intValue).
 */
int ccruncher::ExpatHandlers::getIntAttribute(const char **atts, const char *attname, int defval) const
{
  const char *val = getAttributeValue(atts, attname, nullptr);

  if (val != nullptr)
  {
    return Parser::intValue(val);
  }
  else
  {
    return defval;
  }
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as a long.
 *          If attribute is not found then returns an exception.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @return Attribute value as long.
 * @throw Exception If attribute is not found in attributes list.
 * @throw Exception If attribute value is not a long (see Parser#longValue).
 */
long ccruncher::ExpatHandlers::getLongAttribute(const char **atts, const char *attname) const
{
  const char *val = getAttributeValue(atts, attname);
  return Parser::longValue(val);
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as long.
 *          If attribute is not found then returns the default value.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @param[in] defval Attribute default value.
 * @return If attribute exist returns attribute value as long, otherwise
 *         returns default value.
 * @throw Exception If attribute value is not a long (see Parser#longValue).
 */
long ccruncher::ExpatHandlers::getLongAttribute(const char **atts, const char *attname, long defval) const
{
  const char *val = getAttributeValue(atts, attname, nullptr);

  if (val != nullptr)
  {
    return Parser::longValue(val);
  }
  else
  {
    return defval;
  }
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as a double.
 *          If attribute is not found then returns an exception.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @return Attribute value as double.
 * @throw Exception If attribute is not found in attributes list.
 * @throw Exception If attribute value is not a double (see Parser#doubleValue).
 */
double ccruncher::ExpatHandlers::getDoubleAttribute(const char **atts, const char *attname) const
{
  const char *val = getAttributeValue(atts, attname);
  return Parser::doubleValue(val);
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as double.
 *          If attribute is not found then returns the default value.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @param[in] defval Attribute default value.
 * @return If attribute exist returns attribute value as double, otherwise
 *         returns default value.
 * @throw Exception If attribute value is not a double (see Parser#doubleValue).
 */
double ccruncher::ExpatHandlers::getDoubleAttribute(const char **atts, const char *attname, double defval) const
{
  const char *val = getAttributeValue(atts, attname, nullptr);

  if (val != nullptr)
  {
    return Parser::doubleValue(val);
  }
  else
  {
    return defval;
  }
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as a date.
 *          If attribute is not found then returns an exception.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @return Attribute value as date.
 * @throw Exception If attribute is not found in attributes list.
 * @throw Exception If attribute value is not a date (see Parser#dateValue).
 */
Date ccruncher::ExpatHandlers::getDateAttribute(const char **atts, const char *attname) const
{
  const char *val = getAttributeValue(atts, attname);
  return Parser::dateValue(val);
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as date.
 *          If attribute is not found then returns the default value.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @param[in] defval Attribute default value.
 * @return If attribute exist returns attribute value as date, otherwise
 *         returns default value.
 * @throw Exception If attribute value is not a date (see Parser#dateValue).
 */
Date ccruncher::ExpatHandlers::getDateAttribute(const char **atts, const char *attname, const Date &defval) const
{
  const char *val = getAttributeValue(atts, attname, nullptr);

  if (val != nullptr)
  {
    return Parser::dateValue(val);
  }
  else
  {
    return defval;
  }
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as a boolean.
 *          If attribute is not found then returns an exception.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @return Attribute value as boolean.
 * @throw Exception If attribute is not found in attributes list.
 * @throw Exception If attribute value is not a boolean (see Parser#boolValue).
 */
bool ccruncher::ExpatHandlers::getBooleanAttribute(const char **atts, const char *attname) const
{
  const char *val = getAttributeValue(atts, attname);
  return Parser::boolValue(val);
}

/**************************************************************************//**
 * @details Searches the attribute name in the attributes list and returns
 *          the attribute value as boolean.
 *          If attribute is not found then returns the default value.
 * @param[in] atts List of attributes.
 * @param[in] attname Attribute name.
 * @param[in] defval Attribute default value.
 * @return If attribute exist returns attribute value as boolean, otherwise
 *         returns default value.
 * @throw Exception If attribute value is not a boolean (see Parser#boolValue).
 */
bool ccruncher::ExpatHandlers::getBooleanAttribute(const char **atts, const char *attname, bool defval) const
{
  const char *val = getAttributeValue(atts, attname, nullptr);

  if (val != nullptr)
  {
    return Parser::boolValue(val);
  }
  else
  {
    return defval;
  }
}

/**************************************************************************//**
 * @param[in] atts List of attributes.
 * @return Number of attributes in the list.
 */
int ccruncher::ExpatHandlers::getNumAttributes(const char **atts) const
{
  int ret = 0;

  for(int i=0; atts[i]; i+=2)
  {
    ret++;
  }

  return ret;
}

