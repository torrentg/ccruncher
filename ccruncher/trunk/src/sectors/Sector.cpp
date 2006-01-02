
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
// Sector.cpp - Sector code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Strings class
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added const qualifiers
//   . order = tag value - 1
//
//===========================================================================

#include "sectors/Sector.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"

//===========================================================================
// reset
//===========================================================================
void ccruncher::Sector::reset()
{
  order = -1;
  name = "";
  desc = "";
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Sector::Sector()
{
  reset();
}

//===========================================================================
// operator less-than (needed by sort functions)
//===========================================================================
bool ccruncher::operator <  (const Sector &x, const Sector &y)
{
  return (x.order < y.order);
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Sector::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"sector")) {
    if (getNumAttributes(attributes) != 3) {
      throw eperror(eu, "invalid number of attributes at sector");
    }
    else {
      order = getIntAttribute(attributes, "order", 0)-1;
      name = getStringAttribute(attributes, "name", "");
      desc = getStringAttribute(attributes, "desc", "_UNDEF_");

      if (order < 0 || name == "" || desc == "_UNDEF_")
      {
        throw eperror(eu, "invalid values at <sector>");
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
void ccruncher::Sector::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"sector")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Sector::getXML(int ilevel) const throw(Exception)
{
  string ret = Strings::blanks(ilevel);

  ret += "<sector ";
  ret += "name ='" + name + "' ";
  ret += "order ='" + Format::int2string(order+1) + "' ";
  ret += "desc ='" + desc + "'";
  ret += "/>\n";

  return ret;
}
