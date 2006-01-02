
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
// Interests.cpp - Interests code - $Rev$
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
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . class refactoring
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "interests/Interests.hpp"
#include "utils/Strings.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Interests::Interests()
{
  ispot = -1;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Interests::~Interests()
{
  // nothing to do
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Interests::size() const
{
  // return size
  return vinterests.size();
}

//===========================================================================
// [] operator
//===========================================================================
Interest& ccruncher::Interests::operator []  (int i)
{
  // assertions
  assert(i >= 0 && i < (int) vinterests.size());

  // return i-th symbol
  return vinterests[i];
}

//===========================================================================
// [] operator
//===========================================================================
Interest& ccruncher::Interests::operator []  (const string &name) throw(Exception)
{
  for (unsigned int i=0;i<vinterests.size();i++)
  {
    if (vinterests[i].getName() == name)
    {
      return vinterests[i];
    }
  }

  throw Exception("Interests::[]: interest " + name + " not found");
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::Interests::validate() throw(Exception)
{
  for (unsigned int i=0;i<vinterests.size();i++)
  {
    if (vinterests[i].getName() == "spot")
    {
      ispot = i;
      return;
    }
  }

  throw Exception("Interests::validate(): interest spot not found");
}

//===========================================================================
// insert a new interest in list
//===========================================================================
void ccruncher::Interests::insertInterest(const Interest &val) throw(Exception)
{
  // validem coherencia
  for (unsigned int i=0;i<vinterests.size();i++)
  {
    if (vinterests[i].getName() == val.getName())
    {
      string msg = "Interests::insertInterest(): interest name ";
      msg += val.getName();
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vinterests.push_back(val);
  }
  catch(std::exception &e)
  {
     throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interests::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"interests")) {
    if (getNumAttributes(attributes) != 0) {
      throw eperror(eu, "found attributes in interests");
    }
  }
  else if (isEqual(name_,"interest")) {
    // setting new handlers
    auxinterest.reset();
    eppush(eu, &auxinterest, name_, attributes);
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interests::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"interests")) {
    auxinterest.reset();
    validate();
  }
  else if (isEqual(name_,"interest")) {
    insertInterest(auxinterest);
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Interests::getXML(int ilevel) const throw(Exception)
{
  string spc = Strings::blanks(ilevel);
  string ret = "";

  ret += spc + "<interests>\n";

  for (unsigned int i=0;i<vinterests.size();i++)
  {
    ret += vinterests[i].getXML(ilevel+2);
  }

  ret += spc + "</interests>\n";

  return ret;
}
