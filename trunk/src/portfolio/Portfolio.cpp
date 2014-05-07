
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
  for(Obligor *obligor : vobligors)
  {
    delete obligor;
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
 * @throw Exception Repeated obligor identifier.
 */
void ccruncher::Portfolio::checkObligor(Obligor *val)
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
  for(Asset *asset : val->getAssets())
  {
    const string &id = asset->getId();
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
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Portfolio::epstart(ExpatUserData &eu, const char *tag, const char **attributes)
{
  if (isEqual(tag,"portfolio")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag portfolio");
    }
    // check parameters needed to parse assets/obligors
    if (eu.date1 == NAD || eu.date2 == NAD || eu.interest == nullptr ||
        eu.ratings == nullptr || eu.factors == nullptr || eu.segmentations == nullptr) {
      throw Exception("required parameters not found");
    }
  }
  else if (isEqual(tag,"obligor")) {
    Obligor *obligor = new Obligor;
    vobligors.push_back(obligor);
    eppush(eu, obligor, tag, attributes);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 */
void ccruncher::Portfolio::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"portfolio")) {
    if (vobligors.empty()) {
      throw Exception("portfolio without obligors");
    }
    idassets.clear();
    idobligors.clear();
  }
  else if (isEqual(tag,"obligor")) {
    assert(!vobligors.empty());
    checkObligor(vobligors.back());
  }
}

