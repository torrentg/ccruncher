
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
 * @return Number of factors.
 */
int ccruncher::Factors::size() const
{
  return vfactors.size();
}

/**************************************************************************//**
 * @param[in] i Index of the factor.
 * @return Factor name.
 */
const string& ccruncher::Factors::getName(int i) const
{
  assert(i >= 0 && i < (int) vfactors.size());
  return vfactors[i].name;
}

/**************************************************************************//**
 * @param[in] i Index of the factor.
 * @return Factor description.
 */
const string& ccruncher::Factors::getDescription(int i) const
{
  assert(i >= 0 && i < (int) vfactors.size());
  return vfactors[i].desc;
}

/**************************************************************************//**
 * @param[in] i Index of the factor.
 * @return Factor loading in range [0,1].
 */
double ccruncher::Factors::getLoading(int i) const
{
  assert(i >= 0 && i < (int) vfactors.size());
  return vfactors[i].loading;
}

/**************************************************************************//**
 * @return Factor loadings list.
 */
vector<double> ccruncher::Factors::getLoadings() const
{
  vector<double> w(vfactors.size(), NAN);
  for(size_t i=0; i<vfactors.size(); i++)
  {
    w[i] = vfactors[i].loading;
  }
  return w;
}

/**************************************************************************//**
 * @param[in] name Factor name.
 * @return Index of the given factor, -1 if not found.
 */
int ccruncher::Factors::getIndex(const char *name) const
{
  assert(name != nullptr);
  for(size_t i=0; i<vfactors.size(); i++)
  {
    if (vfactors[i].name.compare(name) == 0)
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
int ccruncher::Factors::getIndex(const std::string &name) const
{
  return getIndex(name.c_str());
}

/**************************************************************************//**
 * @param[in] val Factor to insert.
 * @throw Exception Factor repeated or loading out-of-range.
 */
void ccruncher::Factors::insertFactor(const Factor &val)
{
  // checking coherence
  for(Factor &factor : vfactors)
  {
    if (factor.name == val.name)
    {
      throw Exception("factor '" + val.name + "' repeated");
    }
  }

  // checking factor loading
  if (val.loading < 0.0 || 1.0 < val.loading)
  {
    string msg = "factor loading [" + val.name + "] out of range [0,1]";
    throw Exception(msg);
  }

  try
  {
    vfactors.push_back(val);
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
      insertFactor(Factor(name,desc,loading));
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
    if (vfactors.empty()) {
      throw Exception("'factors' have no elements");
    }
  }
}

