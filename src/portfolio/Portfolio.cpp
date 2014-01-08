
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
#include "portfolio/Portfolio.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Portfolio::Portfolio() : ratings(NULL), factors(NULL),
    segmentations(NULL), interest(NULL), auxobligor(NULL)
{
  date1 = NAD;
  date2 = NAD;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Portfolio::Portfolio(const Ratings &ratings_, const Factors &factors_,
             Segmentations &segmentations_, const Interest &interest_, 
             const Date &date1_, const Date &date2_) : ratings(NULL), factors(NULL),
    segmentations(NULL), interest(NULL), auxobligor(NULL)
{
  init(ratings_, factors_, segmentations_, interest_, date1_, date2_);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Portfolio::~Portfolio()
{
  if (auxobligor != NULL) {
    delete auxobligor;
  }

  // dropping obligors
  for(unsigned int i=0;i<vobligors.size();i++)
  {
    delete vobligors[i];
  }
}

//===========================================================================
// init
//===========================================================================
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

//===========================================================================
// returns obligor list
//===========================================================================
vector<Obligor *> & ccruncher::Portfolio::getObligors()
{
  return vobligors;
}

//===========================================================================
// inserting a obligor into list
//===========================================================================
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

//===========================================================================
// validations
//===========================================================================
void ccruncher::Portfolio::validations() throw(Exception)
{
  if (vobligors.empty())
  {
    throw Exception("portfolio without obligors");
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
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

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Portfolio::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"portfolio")) {
    assert(auxobligor == NULL);
    validations();
  }
  else if (isEqual(name_,"obligor")) {
    assert(auxobligor != NULL);
    insertObligor(auxobligor);
    auxobligor = NULL;
  }
}

