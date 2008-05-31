
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
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/20 - Gerard Torrent [gerard@mail.generacio.com]
//   . implemented Strings class
//
// 2005/07/21 - Gerard Torrent [gerard@mail.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@mail.generacio.com]
//   . added const qualifiers
//   . order = tag value - 1
//
// 2006/02/11 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed method ExpatHandlers::eperror()
//
// 2007/07/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed sector.order tag
//
//===========================================================================

#include "sectors/Sector.hpp"
#include "utils/Strings.hpp"

//===========================================================================
// reset
//===========================================================================
void ccruncher::Sector::reset()
{
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
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Sector::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"sector")) {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("invalid number of attributes at sector");
    }
    else {
      name = getStringAttribute(attributes, "name", "");
      desc = getStringAttribute(attributes, "desc", "_UNDEF_");

      if (name == "" || desc == "_UNDEF_")
      {
        throw Exception("invalid values at <sector>");
      }
    }
  }
  else {
    throw Exception("unexpected tag " + string(name_));
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
    throw Exception("unexpected end tag " + string(name_));
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
  ret += "desc ='" + desc + "'";
  ret += "/>\n";

  return ret;
}
