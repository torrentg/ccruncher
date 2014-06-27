
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
#include "params/Segmentations.hpp"
#include "params/Interest.hpp"
#include "params/Ratings.hpp"
#include "params/Factors.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"

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
 * @param[in] nsegmentations Number of segmentations. If this object is
 *            initialized using epstart-epend then segmentations will be
 *            retrieved from the ExpatUserData object. Otherwise you need
 *            to specify the number of segmentations.
 */
ccruncher::Obligor::Obligor(size_t nsegmentations) :
    mSegments(nsegmentations, 0), mAssets()
{
  irating = 0;
  ifactor = 0;
  id = "NON_ID";
  lgd = LGD(LGD::Fixed,NAN);
}

/**************************************************************************/
ccruncher::Obligor::~Obligor()
{
  for(Asset *asset : mAssets)
  {
    if (asset != nullptr) delete asset;
  }
  mAssets.clear();
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

  mSegments = o.mSegments;

  for(Asset *asset : mAssets) {
    if (asset != nullptr) delete asset;
  }
  mAssets.assign(o.mAssets.size(), nullptr);
  for(size_t i=0; i<mAssets.size(); i++)
  {
    if (o.mAssets[i] != nullptr)
    {
      mAssets[i] = new Asset;
      *(mAssets[i]) = *(o.mAssets[i]);
    }
  }

  return *this;
}

/**************************************************************************//**
 * @details Checks that asset identifier is not repeated.
 * @throw Exception Error inserting asset.
 */
void ccruncher::Obligor::insertAsset(ExpatUserData &eu)
{
  int ila = mAssets.size()-1;

  // checking coherence
  // TODO: remove this check because Portfolio rechecks
  for(int i=0; i<ila; i++)
  {
    if (mAssets[i]->getId() == mAssets[ila]->getId())
    {
      throw Exception("asset identifier '" + mAssets[ila]->getId() + "' repeated");
    }
  }

  // preparing asset
  try
  {
    assert(eu.date1 != NAD && eu.date2 != NAD && eu.interest != nullptr);
    mAssets[ila]->prepare(eu.date1, eu.date2, *(eu.interest));
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Obligor::epstart(ExpatUserData &eu, const char *tag, const char **attributes)
{
  if (isEqual(tag,"asset"))
  {
    Asset *asset = new Asset;
    mAssets.push_back(asset);
    eppush(eu, asset, tag, attributes);
  }
  else if (isEqual(tag,"belongs-to"))
  {
    assert(eu.segmentations != nullptr);
    const char *ssegmentation = getAttributeValue(attributes, "segmentation");
    int isegmentation = eu.segmentations->indexOfSegmentation(ssegmentation);

    const Segmentation &segmentation = eu.segmentations->getSegmentation(isegmentation);
    if (segmentation.getType() != Segmentation::obligor) {
      throw Exception("trying to assign an obligor to a segmentation composed of assets");
    }

    const char *ssegment = getAttributeValue(attributes, "segment");
    size_t isegment = segmentation.indexOf(ssegment);

    addBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(tag,"obligor"))
  {
    const char *str = nullptr;

    id = getStringAttribute(attributes, "id");

    assert(eu.ratings != nullptr);
    str = getAttributeValue(attributes, "rating");
    irating = eu.ratings->indexOf(str);

    assert(eu.factors != nullptr);
    str = getAttributeValue(attributes, "factor");
    ifactor = eu.factors->indexOf(str);

    str = getAttributeValue(attributes, "lgd", nullptr);
    if (str != nullptr) {
      lgd = LGD(str);
    }

    assert(eu.segmentations != nullptr);
    mSegments.resize(eu.segmentations->size(), 0);
  }
  else
  {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 */
void ccruncher::Obligor::epend(ExpatUserData &eu, const char *tag)
{
  if (isEqual(tag,"asset")) {
    insertAsset(eu);
  }
  else if (isEqual(tag,"obligor")) {

    // check lgd values
    if (hasLGD() && !LGD::isvalid(lgd)) {
      throw Exception("obligor hasn't lgd, but has an asset that assumes obligor lgd");
    }
    
    // shrinking memory
    vector<Asset*>(mAssets.begin(),mAssets.end()).swap(mAssets);

    // important: coding obligor-segments as asset-segments
    for(int i=0; i<(int)eu.segmentations->size(); i++) {
      if (eu.segmentations->getSegmentation(i).getType() == Segmentation::obligor) {
        for(Asset *asset : mAssets) {
          asset->addBelongsTo(i, mSegments[i]);
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
  for(Asset *asset : mAssets)
  {
    if (asset->isActive(from,to))
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
void ccruncher::Obligor::addBelongsTo(int isegmentation, int isegment)
{
  assert(isegmentation < (int) mSegments.size());
  assert(isegment >= 0);

  if (isegmentation < 0) return;

  if (mSegments[isegmentation] > 0)
  {
    throw Exception("belongs-to defined twice");
  }

  mSegments[isegmentation] = isegment;
}

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @param[in] isegment Segment index.
 * @return true if this asset belongs to the indicated segmentation-segment.
 */
bool ccruncher::Obligor::belongsTo(int isegmentation, int isegment)
{
  return (mSegments[isegmentation]==isegment);
}

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @return Segment index.
 */
int ccruncher::Obligor::getSegment(int isegmentation)
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int)mSegments.size());
  return mSegments[isegmentation];
}

/**************************************************************************//**
 * @return List of assets.
 */
vector<Asset *> & ccruncher::Obligor::getAssets()
{
  return mAssets;
}

/**************************************************************************//**
 * @return True = exist an asset that requires the obligor lgd,
 *         false = otherwise.
 */
bool ccruncher::Obligor::hasLGD() const
{
  for(Asset *asset : mAssets)
  {
    if (asset->requiresObligorLGD())
    {
      return true;
    }
  }
  return false;
}

