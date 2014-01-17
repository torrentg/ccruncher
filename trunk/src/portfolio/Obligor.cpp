
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
#include "portfolio/Obligor.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @see Obligor::operator=()
 * @param[in] o Object to copy.
 */
ccruncher::Obligor::Obligor(const Obligor &o)
{
  *this = o;
}

/**************************************************************************//**
 * @param[in] ratings_ List of ratings.
 * @param[in] factors_ List of factors.
 * @param[in] segmentations_ List of segmentations.
 * @param[in] interest_ Yield curve.
 * @param[in] d1 Starting simulation date.
 * @param[in] d2 Ending simulation date.
 */
ccruncher::Obligor::Obligor(const Ratings &ratings_, const Factors &factors_,
               Segmentations &segmentations_, const Interest &interest_,
               const Date &d1, const Date &d2) :
    vsegments(segmentations_.size(), 0), vassets()
{
  // setting external objects references
  ratings = &(ratings_);
  factors = &(factors_);
  segmentations = &(segmentations_);
  interest = &(interest_);
  date1 = d1;
  date2 = d2;

  // setting default values
  irating = -1;
  ifactor = -1;
  id = "NON_ID";
  lgd = LGD(LGD::Fixed,NAN);
}

/**************************************************************************/
ccruncher::Obligor::~Obligor()
{
  for(unsigned int i=0; i<vassets.size(); i++)
  {
    if (vassets[i] != NULL) delete vassets[i];
  }
  vassets.clear();
}

/**************************************************************************//**
 * @see Obligor::operator=()
 * @param[in] o Object to assign.
 */
Obligor& ccruncher::Obligor::operator=(const Obligor &o)
{
  irating = o.irating;
  ifactor = o.ifactor;
  id = o.id;
  lgd = o.lgd;

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

/**************************************************************************//**
 * @details Checks that asset identifier is not repeated.
 * @throw Exception Error inserting asset.
 */
void ccruncher::Obligor::insertAsset() throw(Exception)
{
  int ila = vassets.size()-1;

  // checking coherence
  for (int i=0; i<ila; i++)
  {
    if (vassets[i]->getId() == vassets[ila]->getId())
    {
      throw Exception("asset identifier '" + vassets[ila]->getId() + "' repeated");
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

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] name_ Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
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
    if (irating < 0) throw Exception("rating '" + string(str) + "' not found");

    str = getAttributeValue(attributes, "factor");
    ifactor = factors->getIndex(str);
    if (ifactor < 0) throw Exception("factor '" + string(str) + "' not found");

    str = getAttributeValue(attributes, "lgd", NULL);
    if (str != NULL) {
      lgd = LGD(str);
    }
  }
  else
  {
    throw Exception("unexpected tag '" + string(name_) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] name_ Element name.
 */
void ccruncher::Obligor::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"asset")) {
    insertAsset();
  }
  else if (isEqual(name_,"obligor")) {

    // check lgd values
    if (hasLGD() && !LGD::isvalid(lgd)) {
      throw Exception("obligor hasn't lgd, but has an asset that assumes obligor lgd");
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

/**************************************************************************//**
 * Checks if any of its assets is active in the given time range.
 * @param[in] from Starting simulation date.
 * @param[in] to Ending simulation date.
 */
bool ccruncher::Obligor::isActive(const Date &from, const Date &to)
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

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @param[in] isegment Segment index.
 * @throw Exception Redefined relation.
 */
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

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @param[in] isegment Segment index.
 * @return true if this asset belongs to the indicated segmentation-segment.
 */
bool ccruncher::Obligor::belongsTo(int isegmentation, int isegment)
{
  return (vsegments[isegmentation]==isegment);
}

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @return Segment index.
 */
int ccruncher::Obligor::getSegment(int isegmentation)
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int)vsegments.size());
  return vsegments[isegmentation];
}

/**************************************************************************//**
 * @return List of assets.
 */
vector<Asset *> & ccruncher::Obligor::getAssets()
{
  return vassets;
}

/**************************************************************************//**
 * @return True = exist an asset that requires the obligor lgd,
 *         false = otherwise.
 */
bool ccruncher::Obligor::hasLGD() const
{
  for(unsigned int i=0; i<vassets.size(); i++)
  {
    if (vassets[i]->requiresObligorLGD())
    {
      return true;
    }
  }
  return false;
}

