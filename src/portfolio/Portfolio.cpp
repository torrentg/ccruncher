
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

#include <cmath>
#include <cassert>
#include "portfolio/Portfolio.hpp"
#include "params/Segmentations.hpp"
#include "params/Factors.hpp"
#include "params/Ratings.hpp"
#include "params/Interest.hpp"
#include "utils/Date.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************/
ccruncher::Portfolio::~Portfolio()
{
  for(unsigned int i=0; i<vobligors.size(); i++)
  {
    delete vobligors[i];
  }
}

/**************************************************************************//**
 * @return Obligors list.
 */
vector<Obligor *> & ccruncher::Portfolio::getObligors()
{
  return vobligors;
}

/**************************************************************************//**
 * @details Checks if obligors and assets identifiers are uniques.
 * @param[in] val Obligor to insert.
 * @throw Exception Repeated identifier.
 */
void ccruncher::Portfolio::checkObligor(Obligor *val) throw(Exception)
{
  // checking if obligor id is previously defined
  if(idobligors.find(val->id) != idobligors.end())
  {
    string msg = "obligor id '" + val->id + "' repeated";
    throw Exception(msg);
  }
  else
  {
    idobligors[val->id] = true;
  }

  // checking if assets id are previously defined
  for(int i=0; i<(int)val->getAssets().size(); i++)
  {
    const string &id = val->getAssets()[i]->getId();
    if (idassets.find(id) == idassets.end())
    {
      idassets[id] = true;
    }
    else 
    {
      string msg = "asset id '" + id + "' repeated";
      throw Exception(msg);
    }
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] name_ Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Portfolio::epstart(ExpatUserData &eu, const char *name_,
    const char **attributes)
{
  if (isEqual(name_,"portfolio")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag portfolio");
    }
    // check parameters needed to parse assets/obligors
    if (eu.date1 == nullptr || eu.date2 == nullptr || eu.interest == nullptr ||
        eu.ratings == nullptr || eu.factors == nullptr || eu.segmentations == nullptr) {
      throw Exception("required parameters not found");
    }
  }
  else if (isEqual(name_,"obligor")) {
    Obligor *obligor = new Obligor;
    vobligors.push_back(obligor);
    eppush(eu, obligor, name_, attributes);
  }
  else {
    throw Exception("unexpected tag '" + string(name_) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] name_ Element name.
 */
void ccruncher::Portfolio::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"portfolio")) {
    if (vobligors.empty()) {
      throw Exception("portfolio without obligors");
    }
    idassets.clear();
    idobligors.clear();
  }
  else if (isEqual(name_,"obligor")) {
    assert(!vobligors.empty());
    checkObligor(vobligors.back());
  }
}

