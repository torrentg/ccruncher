
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

#include <limits>
#include <cassert>
#include "params/Factors.hpp"
#include "utils/Format.hpp"

using namespace std;

/**************************************************************************//**
 * @param[in] name Factor name.
 * @return Index of the given factor, -1 if not found.
 */
unsigned char ccruncher::Factors::indexOf(const char *name) const
{
  assert(name != nullptr);
  assert(this->size() <= numeric_limits<unsigned char>::max());
  for(size_t i=0; i<this->size(); i++) {
    if ((*this)[i].name.compare(name) == 0) {
      return (unsigned char) i;
    }
  }
  throw Exception("factor '" + string(name) + "' not found");
}

/**************************************************************************//**
 * @param[in] name Factor name.
 * @return Index of the given factor, -1 if not found.
 */
unsigned char ccruncher::Factors::indexOf(const std::string &name) const
{
  return indexOf(name.c_str());
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Factors::epstart(ExpatUserData &, const char *tag, const char **attributes)
{
  if (isEqual(tag,"factors")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag 'factors'");
    }
  }
  else if (isEqual(tag,"factor")) {
    if (getNumAttributes(attributes) < 2 || getNumAttributes(attributes) > 3) {
      throw Exception("invalid number of attributes at tag 'factor'");
    }
    else {
      string name = getStringAttribute(attributes, "name");
      string desc = getStringAttribute(attributes, "description", "");
      double loading = getDoubleAttribute(attributes, "loading");
      this->push_back(Factor(name,loading,desc));
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
void ccruncher::Factors::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"factors")) {
    isValid(*this, true);
  }
}

/**************************************************************************//**
 * @param[in] factors List of factors.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Invalid number of factors, or duplicated factor name,
 *        or factor loadings out of range.
 */
bool ccruncher::Factors::isValid(const std::vector<Factor> &factors, bool throwException)
{
  try
  {
    if (factors.empty()) {
      throw Exception("'factors' have no elements");
    }
    if (factors.size() > numeric_limits<unsigned char>::max()) {
      throw Exception("number of factors bigger than " + Format::toString(numeric_limits<unsigned char>::max()));
    }

    // checking for duplicated elements
    for(size_t i=0; i<factors.size(); i++) {
      for(size_t j=i+1; j<factors.size(); j++) {
        if (factors[i].name == factors[j].name) {
          throw Exception("factor name '" + factors[i].name + "' repeated");
        }
      }
    }
    
    // checking factor loading ranges
    vector<double> loadings = getLoadings(factors);
    isValid(loadings, true);
    
    return true;
  }
  catch(Exception &)
  {
    if (throwException) throw;
    else return false;
  }
}

/**************************************************************************//**
 * @param[in] loadings List of factors loadings.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Invalid number of factors, or factor loadings out of range.
 */
bool ccruncher::Factors::isValid(const std::vector<double> &loadings, bool throwException)
{
  try
  {
    if (loadings.empty() || loadings.size() > numeric_limits<unsigned char>::max()) {
      throw Exception("loadings has invalid length");
    }

    // checking factor loading ranges
    for(size_t i=0; i<loadings.size(); i++) {
      if (loadings[i] < 0.0 || 1.0 < loadings[i]) {
        string msg = "factor loading out of range [0,1]";
        throw Exception(msg);
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

/**************************************************************************//**
 * @param[in] factors List of factors.
 * @return Factor loadings list.
 */
vector<double> ccruncher::Factors::getLoadings(const vector<Factor> &factors)
{
  vector<double> w(factors.size(), NAN);
  for(size_t i=0; i<factors.size(); ++i) {
    w[i] = factors[i].loading;
  }
  return w;
}

