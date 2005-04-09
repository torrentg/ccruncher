
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
// Rating.cpp - Rating code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include "ratings/Rating.hpp"
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Rating::Rating()
{
  reset();
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Rating::reset()
{
  order = -1;
  name = "";
  desc = "";
}

//===========================================================================
// operador de comparacio (per permetre ordenacio)
//===========================================================================
bool ccruncher::operator <  (const Rating &x, const Rating &y)
{
  return (x.order < y.order);
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Rating::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"rating")) {
    if (getNumAttributes(attributes) != 3) {
      throw eperror(eu, "invalid number of attributes at rating");
    }
    else {
      order = getIntAttribute(attributes, "order", -1);
      name = getStringAttribute(attributes, "name", "");
      desc = getStringAttribute(attributes, "desc", "_UNDEF_");

      if (order <= 0 || name == "" || desc == "_UNDEF_")
      {
        throw eperror(eu, "invalid values at <rating>");
      }
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Rating::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"rating")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Rating::getXML(int ilevel) throw(Exception)
{
  string ret = Utils::blanks(ilevel);

  ret += "<rating ";
  ret += "name='" + name + "' ";
  ret += "order='" + Parser::int2string(order) + "' ";
  ret += "desc='" + desc + "'";
  ret += "/>\n";

  return ret;
}
