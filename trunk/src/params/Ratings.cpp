
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#include <cassert>
#include "params/Ratings.hpp"

using namespace std;

/**************************************************************************//**
 * @param[in] name Rating name.
 * @return Index of the rating (-1 if rating not found).
 */
int ccruncher::Ratings::indexOf(const char *name) const
{
  assert(name != nullptr);
  for(size_t i=0; i<this->size(); i++)
  {
    if ((*this)[i].getName().compare(name) == 0)
    {
      return i;
    }
  }
  return -1;
}

/**************************************************************************//**
 * @param[in] name Rating name.
 * @return Index of the rating (-1 if rating not found).
 */
int ccruncher::Ratings::indexOf(const std::string &name) const
{
  return indexOf(name.c_str());
}

/**************************************************************************//**
 * @param[in] val Rating to insert.
 * @throw Exception Rating name repeated.
 */
void ccruncher::Ratings::add(const Rating &val)
{
  // checking coherence
  for(Rating &rating : (*this))
  {
    if (rating.getName() == val.getName())
    {
      throw Exception("rating name '" + val.getName() + "' repeated");
    }
  }

  try
  {
    this->push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] name_ Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Ratings::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"ratings")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag 'ratings'");
    }
  }
  else if (isEqual(name_,"rating")) {
    if (getNumAttributes(attributes) < 1 || getNumAttributes(attributes) > 2) {
      throw Exception("invalid number of attributes in tag 'rating'");
    }
    else {
      string name = getStringAttribute(attributes, "name");
      string desc = getStringAttribute(attributes, "description", "");
      add(Rating(name,desc));
    }
  }
  else {
    throw Exception("unexpected tag '" + string(name_) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] name_ Element name.
 */
void ccruncher::Ratings::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"ratings")) {
    // minimum number of rating: default+non-default
    if (this->size() < 2) {
      throw Exception("required a minimum of 2 ratings");
    }
  }
}

