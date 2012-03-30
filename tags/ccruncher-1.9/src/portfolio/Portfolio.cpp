
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

#include <cmath>
#include "portfolio/Portfolio.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Portfolio::Portfolio(const Ratings &ratings_, const Sectors &sectors_,
             Segmentations &segmentations_, const Interest &interest_, 
             const Date &date1_, const Date &date2_)
{
  auxobligor = NULL;
  vobligors.clear();
  // setting external objects
  ratings = &ratings_;
  sectors = &sectors_;
  segmentations = &segmentations_;
  interest = &interest_;
  date1 = date1_;
  date2 = date2_;
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
    string msg = "obligor id " + val->id + " repeated";
    delete val;
    throw Exception(msg);
  }
  else
  {
    idobligors[val->id] = true;
  }

  // checking if assets id are previously defined
  for(int i=0; i<(int)val->getAssets().size(); i++)
  {
    string id = val->getAssets()[i]->getId();
    if (idassets.find(id) != idassets.end())
    {
       string msg = "asset id " + id + " repeated";
       delete val;
       throw Exception(msg);
    }
    else 
    {
      idassets[id] = true;
    }
  }

  // inserting obligor in portfolio
  try
  {
    vobligors.push_back(val);
  }
  catch(std::exception &e)
  {
    delete val;
    throw Exception(e);
  }
}

//===========================================================================
// validations
//===========================================================================
void ccruncher::Portfolio::validations() throw(Exception)
{
  if (vobligors.size() == 0)
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
    auxobligor = new Obligor(*ratings, *sectors, *segmentations, *interest, date1, date2);
    eppush(eu, auxobligor, name_, attributes);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
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
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

