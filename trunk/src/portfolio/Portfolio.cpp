
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

using namespace std;
using namespace ccruncher;

/**************************************************************************/
ccruncher::Portfolio::Portfolio() : ratings(NULL), factors(NULL),
    segmentations(NULL), interest(NULL), auxobligor(NULL)
{
  date1 = NAD;
  date2 = NAD;
}

/**************************************************************************//**
 * @param[in] ratings_ List of ratings.
 * @param[in] factors_ List of factors.
 * @param[in] segmentations_ List of segmentations.
 * @param[in] interest_ Yield curve.
 * @param[in] date1_ Starting simulation date.
 * @param[in] date2_ Ending simulation date.
 */
ccruncher::Portfolio::Portfolio(const Ratings &ratings_, const Factors &factors_,
             Segmentations &segmentations_, const Interest &interest_, 
             const Date &date1_, const Date &date2_) : ratings(NULL), factors(NULL),
    segmentations(NULL), interest(NULL), auxobligor(NULL)
{
  init(ratings_, factors_, segmentations_, interest_, date1_, date2_);
}

/**************************************************************************/
ccruncher::Portfolio::~Portfolio()
{
  if (auxobligor != NULL) {
    delete auxobligor;
  }

  for(unsigned int i=0;i<vobligors.size();i++)
  {
    delete vobligors[i];
  }
}

/**************************************************************************//**
 * @param[in] ratings_ List of ratings.
 * @param[in] factors_ List of factors.
 * @param[in] segmentations_ List of segmentations.
 * @param[in] interest_ Yield curve.
 * @param[in] date1_ Starting simulation date.
 * @param[in] date2_ Ending simulation date.
 */
void ccruncher::Portfolio::init(const Ratings &ratings_, const Factors &factors_,
             Segmentations &segmentations_, const Interest &interest_,
             const Date &date1_, const Date &date2_)
{
  auxobligor = NULL;
  vobligors.clear();
  // setting external objects
  if (ratings_.size() == 0) throw Exception("ratings not defined");
  else ratings = &ratings_;
  if (factors_.size() == 0) throw Exception("factors not defined");
  else factors = &factors_;
  if (segmentations_.size() == 0) throw Exception("segmentations not defined");
  else segmentations = &segmentations_;
  interest = &interest_;
  if (date1_ == NAD) throw Exception("time.0 not defined");
  else date1 = date1_;
  if (date2_ == NAD) throw Exception("time.T not defined");
  else date2 = date2_;
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
void ccruncher::Portfolio::insertObligor(Obligor *val) throw(Exception)
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

  // inserting obligor in portfolio
  try
  {
    vobligors.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] name_ Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Portfolio::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"portfolio")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag portfolio");
    }
  }
  else if (isEqual(name_,"obligor")) {
    assert(auxobligor == NULL);
    auxobligor = new Obligor(*ratings, *factors, *segmentations, *interest, date1, date2);
    eppush(eu, auxobligor, name_, attributes);
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
    assert(auxobligor == NULL);
    if (vobligors.empty()) {
      throw Exception("portfolio without obligors");
    }
  }
  else if (isEqual(name_,"obligor")) {
    assert(auxobligor != NULL);
    insertObligor(auxobligor);
    auxobligor = NULL;
  }
}

