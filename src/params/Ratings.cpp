
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#include <climits>
#include <cassert>
#include "params/Ratings.hpp"
#include "utils/Format.hpp"

using namespace std;

/**************************************************************************//**
 * @param[in] name Rating name.
 * @return Index of the rating (-1 if rating not found).
 */
unsigned char ccruncher::Ratings::indexOf(const char *name) const
{
  assert(name != nullptr);
  assert(this->size() <= UCHAR_MAX);
  for(size_t i=0; i<this->size(); i++) {
    if ((*this)[i].name.compare(name) == 0) {
      return (unsigned char) i;
    }
  }
  throw Exception("rating '" + string(name) + "' not found");
}

/**************************************************************************//**
 * @param[in] name Rating name.
 * @return Index of the rating (-1 if rating not found).
 */
unsigned char ccruncher::Ratings::indexOf(const std::string &name) const
{
  return indexOf(name.c_str());
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Ratings::epstart(ExpatUserData &, const char *tag, const char **attributes)
{
  if (isEqual(tag,"ratings")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag 'ratings'");
    }
  }
  else if (isEqual(tag,"rating")) {
    if (getNumAttributes(attributes) < 1 || getNumAttributes(attributes) > 2) {
      throw Exception("invalid number of attributes in tag 'rating'");
    }
    else {
      string name = getStringAttribute(attributes, "name");
      string desc = getStringAttribute(attributes, "description", "");
      this->push_back(Rating(name,desc));
    }
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 */
void ccruncher::Ratings::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"ratings")) {
    isValid(*this, true);
  }
}

/**************************************************************************//**
 * @param[in] ratings List of ratings.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Invalid number of ratings or rating name repeated.
 */
bool ccruncher::Ratings::isValid(const std::vector<Rating> &ratings, bool throwException)
{
  try
  {
    if (ratings.size() < 2) {
      throw Exception("required a minimum of 2 ratings");
    }
    if (ratings.size() > UCHAR_MAX) {
      throw Exception("number of ratings bigger than " + Format::toString(UCHAR_MAX));
    }

    // checking for duplicated elements
    for(size_t i=0; i<ratings.size(); i++) {
      for(size_t j=i+1; j<ratings.size(); j++) {
        if (ratings[i].name == ratings[j].name) {
          throw Exception("rating name '" + ratings[i].name + "' repeated");
        }
      }
    }
    return true;
  }
  catch(Exception &)
  {
    if (throwException) throw;
    else return false;
  }
}

