
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
#include "params/Factors.hpp"

using namespace std;

/**************************************************************************//**
 * @return Factor loadings list.
 */
vector<double> ccruncher::Factors::getLoadings() const
{
  vector<double> w(size(), NAN);
  for(size_t i=0; i<this->size(); ++i) {
    w[i] = (*this)[i].getLoading();
  }
  return w;
}

/**************************************************************************//**
 * @param[in] name Factor name.
 * @return Index of the given factor, -1 if not found.
 */
int ccruncher::Factors::indexOf(const char *name) const
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
 * @param[in] name Factor name.
 * @return Index of the given factor, -1 if not found.
 */
int ccruncher::Factors::indexOf(const std::string &name) const
{
  return indexOf(name.c_str());
}

/**************************************************************************//**
 * @param[in] val Factor to insert.
 * @throw Exception Factor repeated or loading out-of-range.
 */
void ccruncher::Factors::add(const Factor &val)
{
  // checking coherence
  for(Factor &factor : (*this))
  {
    if (factor.getName() == val.getName())
    {
      throw Exception("factor '" + val.getName() + "' repeated");
    }
  }

  // checking factor loading
  if (val.getLoading() < 0.0 || 1.0 < val.getLoading())
  {
    string msg = "factor loading [" + val.getName() + "] out of range [0,1]";
    throw Exception(msg);
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
void ccruncher::Factors::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"factors")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag 'factors'");
    }
  }
  else if (isEqual(name_,"factor")) {
    if (getNumAttributes(attributes) < 2 || getNumAttributes(attributes) > 3) {
      throw Exception("invalid number of attributes at tag 'factor'");
    }
    else {
      string name = getStringAttribute(attributes, "name");
      string desc = getStringAttribute(attributes, "description", "");
      double loading = getDoubleAttribute(attributes, "loading");
      add(Factor(name,loading,desc));
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
void ccruncher::Factors::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"factors")) {
    if (this->empty()) {
      throw Exception("'factors' have no elements");
    }
  }
}

