
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#include "interests/Rate.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Rate::Rate()
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
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name,"rate")) {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("incorrect number of attributes");
    }
    else
    {
      t = getDoubleAttribute(attributes, "t", -99.0);
      r = getDoubleAttribute(attributes, "r", -99.0);

      if (t == -99.0 || r == -99.0)
      {
        throw Exception("invalid attributes values at <rate>");
      }
      else if (r < -0.5 || 1.0 < r)
      {
        throw Exception("rate value " + Format::double2string(r) + " out of range [-0.5, +1.0]");
      }
    }
  }
  else {
    throw Exception("unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Rate::epend(ExpatUserData &eu, const char *name)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name,"rate")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Rate::getXML(int ilevel) const throw(Exception)
{
  string ret = Strings::blanks(ilevel);

  ret += "<rate ";
  ret += "t='" + Format::double2string(t) + "' ";
  ret += "r='" + Format::double2string(r) + "'";
  ret += "/>\n";

  return ret;
}
