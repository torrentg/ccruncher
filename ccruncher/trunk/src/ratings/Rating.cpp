
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
// Rating.cpp - Rating code - $Rev$
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
// 2006/02/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed method ExpatHandlers::eperror()
//
// 2007/07/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed rating.order tag
//
//===========================================================================

#include "ratings/Rating.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"

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
  name = "";
  desc = "";
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Rating::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"rating")) {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("invalid number of attributes in rating tag");
    }
    else {
      name = getStringAttribute(attributes, "name", "");
      desc = getStringAttribute(attributes, "desc", "_UNDEF_");

      if (name == "" || desc == "_UNDEF_")
      {
        throw Exception("invalid values at <rating>");
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
void ccruncher::Rating::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"rating")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Rating::getXML(int ilevel) const throw(Exception)
{
  string ret = Strings::blanks(ilevel);

  ret += "<rating ";
  ret += "name='" + name + "' ";
  ret += "desc='" + desc + "'";
  ret += "/>\n";

  return ret;
}
