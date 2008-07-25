
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
// Rate.cpp - Rate code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
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
//===========================================================================

#include "interests/Rate.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Rate::Rate()
{
  reset();
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Rate::reset()
{
  t = 0.0;
  r = 0.0;
}

//===========================================================================
// operator less-than (needed by sort functions)
//===========================================================================
bool ccruncher::operator <  (const Rate &x, const Rate &y)
{
  return (x.t < y.t);
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Rate::epstart(ExpatUserData &eu, const char *name, const char **attributes)
{
  if (isEqual(name,"rate")) {
    if (getNumAttributes(attributes) != 2) {
      throw eperror(eu, "incorrect number of attributes");
    }
    else
    {
      t = getDoubleAttribute(attributes, "t", -1.0);
      r = getDoubleAttribute(attributes, "r", -1.0);

      if (t <= -1.0+1E-14 || r <= -1.0+1E-14)
      {
        throw eperror(eu, "invalid attributes values at <rate>");
      }
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Rate::epend(ExpatUserData &eu, const char *name)
{
  if (isEqual(name,"rate")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Rate::getXML(int ilevel) throw(Exception)
{
  string ret = Strings::blanks(ilevel);

  ret += "<rate ";
  ret += "t='" + Format::double2string(t) + "' ";
  ret += "r='" + Format::double2string(r) + "'";
  ret += "/>\n";

  return ret;
}
