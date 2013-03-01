
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
#include "portfolio/Obligor.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::Obligor::Obligor(const Obligor &o)
{
  *this = o;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Obligor::Obligor(const Ratings &ratings_, const Factors &factors_,
               Segmentations &segmentations_, const Interest &interest_,
               const Date &d1, const Date &d2) : vsegments(), vassets()
{
  // setting external objects references
  ratings = &(ratings_);
  factors = &(factors_);
  segmentations = &(segmentations_);
  interest = &(interest_);
  date1 = d1;
  date2 = d2;

  // setting default values
  vsegments = vector<int>(segmentations_.size(), 0);
  irating = -1;
  ifactor = -1;
  id = "NON_ID";
  recovery = Recovery(Recovery::Fixed,NAN);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Obligor::~Obligor()
{
  for(unsigned int i=0; i<vassets.size(); i++)
  {
    if (vassets[i] != NULL) delete vassets[i];
  }
  vassets.clear();
}

//===========================================================================
// assignment operator
//===========================================================================
Obligor& ccruncher::Obligor::operator=(const Obligor &o)
{
  irating = o.irating;
  ifactor = o.ifactor;
  id = o.id;
  recovery = o.recovery;

  vsegments = o.vsegments;
  ratings = o.ratings;
  factors = o.factors;
  segmentations = o.segmentations;
  interest = o.interest;
  date1 = o.date1;
  date2 = o.date2;

  for(unsigned int i=0; i<vassets.size(); i++) {
    if (vassets[i] != NULL) delete vassets[i];
  }
  vassets.assign(o.vassets.size(), NULL);
  for(size_t i=0; i<vassets.size(); i++)
  {
    if (o.vassets[i] != NULL)
    {
      vassets[i] = new Asset(NULL);
      *(vassets[i]) = *(o.vassets[i]);
    }
  }

  return *this;
}

//===========================================================================
// check last asset
//===========================================================================
void ccruncher::Obligor::prepareLastAsset() throw(Exception)
{
  int ila = vassets.size()-1;

  // checking coherence
  for (int i=0; i<ila; i++)
  {
    if (vassets[i]->getId() == vassets[ila]->getId())
    {
      throw Exception("asset identifier " + vassets[ila]->getId() + " repeated");
    }
  }

  // preparing asset
  try
  {
    vassets[ila]->prepare(date1, date2, *interest);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Obligor::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"asset"))
  {
    Asset *asset = new Asset(segmentations);
    vassets.push_back(asset);
    eppush(eu, asset, name_, attributes);
  }
  else if (isEqual(name_,"belongs-to"))
  {
    const char *ssegmentation = getAttributeValue(attributes, "segmentation");
    int isegmentation = segmentations->indexOfSegmentation(ssegmentation);

    const char *ssegment = getAttributeValue(attributes, "segment");
    int isegment = segmentations->getSegmentation(isegmentation).indexOfSegment(ssegment);

    addBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(name_,"obligor"))
  {
    const char *str = NULL;
    id = getStringAttribute(attributes, "id");

    str = getAttributeValue(attributes, "rating");
    irating = ratings->getIndex(str);
    if (irating < 0) throw Exception("rating not found");

    str = getAttributeValue(attributes, "factor");
    ifactor = factors->getIndex(str);
    if (ifactor < 0) throw Exception("factor not found");

    str = getAttributeValue(attributes, "recovery", NULL);
    if (str != NULL) {
      recovery = Recovery(str);
    }
  }
  else
  {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Obligor::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"asset")) {
    prepareLastAsset();
  }
  else if (isEqual(name_,"obligor")) {

    // check recovery values
    if (hasRecovery() && !Recovery::isvalid(recovery)) {
      throw Exception("obligor hasn't recovery, but has asset that assumes obligor recovery");
    }
    
    // shrinking memory
    vector<Asset*>(vassets.begin(),vassets.end()).swap(vassets);

    // important: coding obligor-segments as asset-segments
    for (int i=0; i<(int)segmentations->size(); i++) {
      if (segmentations->getSegmentation(i).components == Segmentation::obligor) {
        for(int j=0; j<(int)vassets.size(); j++) {
          vassets[j]->addBelongsTo(i, vsegments[i]);
        }
      }
    }
  }
}

//===========================================================================
// isActive
//===========================================================================
bool ccruncher::Obligor::isActive(const Date &from, const Date &to) throw(Exception)
{
  for(unsigned int i=0;i<vassets.size();i++)
  {
    if (vassets[i]->isActive(from,to))
    {
      return true;
    }
  }

  return false;
}

//===========================================================================
// addBelongsTo
//===========================================================================
void ccruncher::Obligor::addBelongsTo(int isegmentation, int isegment) throw(Exception)
{
  assert(isegmentation < (int) vsegments.size());
  assert(isegment >= 0);

  if (isegmentation < 0) return;

  if (vsegments[isegmentation] > 0)
  {
    throw Exception("belongs-to defined twice");
  }

  vsegments[isegmentation] = isegment;
}

//===========================================================================
// belongsTo
//===========================================================================
bool ccruncher::Obligor::belongsTo(int isegmentation, int isegment)
{
  return (vsegments[isegmentation]==isegment);
}

//===========================================================================
// getAssets
//===========================================================================
vector<Asset *> & ccruncher::Obligor::getAssets()
{
  return vassets;
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Obligor::getSegment(int isegmentation)
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int)vsegments.size());
  return vsegments[isegmentation];
}

//===========================================================================
// hasRecovery
//===========================================================================
bool ccruncher::Obligor::hasRecovery() const
{
  for(unsigned int i=0; i<vassets.size(); i++)
  {
    if (vassets[i]->hasObligorRecovery())
    {
      return true;
    }
  }
  return false;
}

