
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

#include <cmath>
#include "interests/Interests.hpp"
#include "utils/Strings.hpp"
#include <cassert>

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

  throw Exception("interest " + name + " not found");
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

  throw Exception("interest 'spot' not defined");
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
      throw Exception("interest name " + val.getName() + " repeated");
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
      throw Exception("found attributes in interests");
    }
  }
  else if (isEqual(name_,"interest")) {
    // setting new handlers
    auxinterest = Interest();
    eppush(eu, &auxinterest, name_, attributes);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interests::epend(ExpatUserData &eu, const char *name_)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"interests")) {
    auxinterest = Interest();
    validate();
  }
  else if (isEqual(name_,"interest")) {
    insertInterest(auxinterest);
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
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
