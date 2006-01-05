
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
// Ratings.cpp - Ratings code - $Rev$
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
//   . class refactoring
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "ratings/Ratings.hpp"
#include "utils/Strings.hpp"
#include "utils/Format.hpp"
#include <cassert>

//===========================================================================
// constructor privat
//===========================================================================
ccruncher::Ratings::Ratings()
{
  // nothing to do
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Ratings::~Ratings()
{
  // cal assegurar que es destrueix vratings;
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Ratings::size() const
{
  return vratings.size();
}

//===========================================================================
// [] operator
//===========================================================================
Rating& ccruncher::Ratings::operator []  (int i)
{
  // assertions
  assert(i >= 0 && i < (int) vratings.size());

  // return i-th rating
  return vratings[i];
}

//===========================================================================
// [] operator. returns rating by name
//===========================================================================
Rating& ccruncher::Ratings::operator []  (const string &name) throw(Exception)
{
  for (unsigned int i=0;i<vratings.size();i++)
  {
    if (vratings[i].name == name)
    {
      return vratings[i];
    }
  }

  throw Exception("Ratings::[]: rating " + name + " not found");
}

//===========================================================================
// insert a rating into list
//===========================================================================
void ccruncher::Ratings::insertRating(const Rating &val) throw(Exception)
{
  // checking coherence
  for (unsigned int i=0;i<vratings.size();i++)
  {
    Rating aux = vratings[i];

    if (aux.name == val.name)
    {
      string msg = "Ratings::insertRating(): rating name ";
      msg += val.name;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (aux.order == val.order)
    {
      string msg = "Ratings::insertRating(): rating order ";
      msg += val.order;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (aux.desc == val.desc)
    {
      string msg = "Ratings::insertRating(): rating desc ";
      msg += val.desc;
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vratings.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Ratings::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"ratings")) {
    if (getNumAttributes(attributes) != 0) {
      throw eperror(eu, "attributes are not allowed in tag ratings");
    }
  }
  else if (isEqual(name_,"rating")) {
    auxrating.reset();
    eppush(eu, &auxrating, name_, attributes);
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Ratings::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"ratings")) {
    validations();
    auxrating.reset();
  }
  else if (isEqual(name_,"rating")) {
    insertRating(auxrating);
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// global validations
//===========================================================================
void ccruncher::Ratings::validations() throw(Exception)
{
  // checking number of ratings
  if (vratings.size() == 0)
  {
    throw Exception("Ratings::validations(): ratings have no elements");
  }

  // sorting ratings by filed 'order'
  sort(vratings.begin(), vratings.end());

  // checking that first 'order' is 1 and don't exist holes
  for(unsigned int i=0;i<vratings.size();i++)
  {
    Rating aux = vratings[i];

    if (aux.order != (int)(i))
    {
      string msg = "Ratings::validations(): incorrect order rating at or near order = ";
      msg += Format::int2string(aux.order);
      throw Exception(msg);
    }
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Ratings::getXML(int ilevel) const throw(Exception)
{
  string spc = Strings::blanks(ilevel);
  string ret = "";

  ret += spc + "<ratings>\n";

  for (unsigned int i=0;i<vratings.size();i++)
  {
    ret += vratings[i].getXML(ilevel+2);
  }

  ret += spc + "</ratings>\n";

  return ret;
}
