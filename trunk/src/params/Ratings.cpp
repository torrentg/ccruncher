
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include "params/Ratings.hpp"
#include "utils/Strings.hpp"
#include <cassert>

//===========================================================================
// constructor privat
//===========================================================================
ccruncher::Ratings::Ratings()
{
  // nothing to do
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Ratings::size() const
{
  return vratings.size();
}

//===========================================================================
// return rating name
//===========================================================================
const string& ccruncher::Ratings::getName(int i) const
{
  assert(i >= 0 && i < (int) vratings.size());
  return vratings[i].name;
}

//===========================================================================
// return rating description
//===========================================================================
const string& ccruncher::Ratings::getDescription(int i) const
{
  assert(i >= 0 && i < (int) vratings.size());
  return vratings[i].desc;
}

//===========================================================================
// return the index of the rating (-1 if rating not found)
//===========================================================================
int ccruncher::Ratings::getIndex(const char *name) const
{
  assert(name != NULL);
  for (unsigned int i=0; i<vratings.size(); i++)
  {
    if (vratings[i].name.compare(name) == 0)
    {
      return i;
    }
  }
  return -1;
}

//===========================================================================
// return the index of the rating (-1 if rating not found)
//===========================================================================
int ccruncher::Ratings::getIndex(const string &name) const
{
  return getIndex(name.c_str());
}

//===========================================================================
// insert a rating into list
//===========================================================================
void ccruncher::Ratings::insertRating(const Rating &val) throw(Exception)
{
  // checking coherence
  for (unsigned int i=0; i<vratings.size(); i++)
  {
    Rating aux = vratings[i];

    if (aux.name == val.name)
    {
      throw Exception("rating name " + val.name + " repeated");
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
void ccruncher::Ratings::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"ratings")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag ratings");
    }
  }
  else if (isEqual(name_,"rating")) {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("invalid number of attributes in rating tag");
    }
    else {
      string name = getStringAttribute(attributes, "name");
      string desc = getStringAttribute(attributes, "description", "");
      insertRating(Rating(name,desc));
    }
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Ratings::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"ratings")) {
    validations();
  }
  else if (isEqual(name_,"rating")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// global validations
//===========================================================================
void ccruncher::Ratings::validations() throw(Exception)
{
  // checking number of ratings (minimum: default+non-default)
  if (vratings.size() < 2)
  {
    throw Exception("required a minimum of 2 ratings");
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
    ret += Strings::blanks(ilevel+2);
    ret += "<rating ";
    ret += "name='" + vratings[i].name + "' ";
    if (vratings[i].desc != "")
    ret += "description='" + vratings[i].desc + "'";
    ret += "/>\n";
  }

  ret += spc + "</ratings>\n";

  return ret;
}
