
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#include "portfolio/Borrower.hpp"
#include "utils/Utils.hpp"
#include <cmath>
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Borrower::Borrower(const Ratings &ratings_, const Sectors &sectors_,
               Segmentations &segmentations_, const Interest &interest_,
               const Date &d1, const Date &d2) : vsegments(), vassets()
{
  // setting external objects references
  ratings = &(ratings_);
  sectors = &(sectors_);
  segmentations = &(segmentations_);
  interest = &(interest_);
  date1 = d1;
  date2 = d2;

  // setting default values
  vsegments = vector<int>(segmentations_.size(), 0);
  irating = -1;
  isector = -1;
  id = "NON_ID";
  name = "NO_NAME";
  recovery = NAN;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Borrower::~Borrower()
{
  for(unsigned int i=0; i<vassets.size(); i++)
  {
    delete vassets[i];
  }
  vassets.clear();
}

//===========================================================================
// check last asset
//===========================================================================
void ccruncher::Borrower::prepareLastAsset() throw(Exception)
{
  int ila = vassets.size()-1;

  // checking coherence
  for (int i=0;i<ila;i++)
  {
    if (vassets[i]->getId() == vassets[ila]->getId())
    {
      throw Exception("asset identifier " + vassets[ila]->getId() + " repeated");
    }
  }

  // preparing asset
  try
  {
    vassets[ila]->precomputeLosses(date1, date2, *interest);
    vassets[ila]->deleteData();
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Borrower::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"asset")) {
    Asset *asset = new Asset(segmentations, recovery);
    vassets.push_back(asset);
    eppush(eu, asset, name_, attributes);
  }
  else if (isEqual(name_,"belongs-to")) {
    string ssegmentation = getStringAttribute(attributes, "segmentation", "");
    string ssegment = getStringAttribute(attributes, "segment", "");

    if (ssegmentation == "" || ssegment == "") {
      throw Exception("invalid attributes at <belongs-to> tag");
    }

    int isegmentation = segmentations->indexOfSegmentation(ssegmentation);
    int isegment = segmentations->getSegmentation(isegmentation).indexOfSegment(ssegment);

    addBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(name_,"borrower")) {
    // reading atributes
    id = getStringAttribute(attributes, "id", "");
    name = getStringAttribute(attributes, "name", "");
    string strrating = getStringAttribute(attributes, "rating", "");
    string strsector= getStringAttribute(attributes, "sector", "");
    if (getAttributeValue(attributes, "recovery") != NULL) {
      recovery = getDoubleAttribute(attributes, "recovery", NAN);
      if (recovery < 0.0 || 1.0 < recovery) {
        throw Exception("recovery value out of range [0%,100%]");
      }
    }

    // retrieving indexes
    irating = ratings->getIndex(strrating);
    isector = sectors->getIndex(strsector);

    // doing some checks
    if (id == "" || name == "" || irating < 0 || isector < 0) {
      throw Exception("invalid attributes at <borrower>");
    }
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Borrower::epend(ExpatUserData &eu, const char *name_)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"asset")) {
    prepareLastAsset();
  }
  else if (isEqual(name_,"belongs-to")) {
    // nothing to do
  }
  else if (isEqual(name_,"borrower")) {

    // shrinking memory
    vector<Asset*>(vassets.begin(),vassets.end()).swap(vassets);

    // filling implicit segment
    try {
      int isegmentation = segmentations->indexOfSegmentation("borrowers");
      int isegment = segmentations->getSegmentation(isegmentation).addSegment(id);
      addBelongsTo(isegmentation, isegment);
    } 
    catch(...) {
      // segmentation 'borrowers' not found
    }

    // important: coding borrower-segments as asset-segments
    for (int i=0; i<(int)segmentations->size(); i++) {
      if (segmentations->getSegmentation(i).components == borrower) {
        for(int j=0; j<(int)vassets.size(); j++) {
          vassets[j]->addBelongsTo(i, vsegments[i]);
        }
      }
    }
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// isActive
//===========================================================================
bool ccruncher::Borrower::isActive(const Date &from, const Date &to) throw(Exception)
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
void ccruncher::Borrower::addBelongsTo(int isegmentation, int isegment) throw(Exception)
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int)vsegments.size());
  assert(isegment >= 0);

  if (vsegments[isegmentation] > 0)
  {
    throw Exception("trying to reinsert a defined segmentation");
  }

  vsegments[isegmentation] = isegment;
}

//===========================================================================
// belongsTo
//===========================================================================
bool ccruncher::Borrower::belongsTo(int isegmentation, int isegment)
{
  return (vsegments[isegmentation]==isegment);
}

//===========================================================================
// getAssets
//===========================================================================
vector<Asset *> & ccruncher::Borrower::getAssets()
{
  return vassets;
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Borrower::getSegment(int isegmentation)
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int)vsegments.size());
  return vsegments[isegmentation];
}
      