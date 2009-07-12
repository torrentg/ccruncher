
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

#include "sectors/Sector.hpp"
#include "utils/Strings.hpp"
#include <cassert>

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
  assert(eu.getCurrentHandlers() != NULL);
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
  assert(eu.getCurrentHandlers() != NULL);
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