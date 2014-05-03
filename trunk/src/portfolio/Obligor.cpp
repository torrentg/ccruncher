
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
    vsegments(nsegmentations, 0), vassets()
{
  // setting default values
  irating = -1;
  ifactor = -1;
  id = "NON_ID";
  lgd = LGD(LGD::Fixed,NAN);
}

/**************************************************************************/
ccruncher::Obligor::~Obligor()
{
  for(Asset *asset : vassets)
  {
    if (asset != nullptr) delete asset;
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

  for(Asset *asset : vassets) {
    if (asset != nullptr) delete asset;
  }
  vassets.assign(o.vassets.size(), nullptr);
  for(size_t i=0; i<vassets.size(); i++)
  {
    if (o.vassets[i] != nullptr)
    {
      vassets[i] = new Asset;
      *(vassets[i]) = *(o.vassets[i]);
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
  int ila = vassets.size()-1;

  // checking coherence
  // TODO: remove this check because Portfolio rechecks
  for(int i=0; i<ila; i++)
  {
    if (vassets[i]->getId() == vassets[ila]->getId())
    {
      throw Exception("asset identifier '" + vassets[ila]->getId() + "' repeated");
    }
  }

  // preparing asset
  try
  {
    assert(eu.date1 != nullptr && eu.date2 != nullptr && eu.interest != nullptr);
    vassets[ila]->prepare(*(eu.date1), *(eu.date2), *(eu.interest));
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
    vassets.push_back(asset);
    eppush(eu, asset, tag, attributes);
  }
  else if (isEqual(tag,"belongs-to"))
  {
    assert(eu.segmentations != nullptr);
    const char *ssegmentation = getAttributeValue(attributes, "segmentation");
    int isegmentation = eu.segmentations->indexOfSegmentation(ssegmentation);

    const Segmentation &segmentation = eu.segmentations->getSegmentation(isegmentation);
    if (segmentation.components != Segmentation::obligor) {
      throw Exception("trying to assign an obligor to a segmentation composed of assets");
    }

    const char *ssegment = getAttributeValue(attributes, "segment");
    int isegment = segmentation.indexOfSegment(ssegment);

    addBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(tag,"obligor"))
  {
    const char *str = nullptr;

    id = getStringAttribute(attributes, "id");

    assert(eu.ratings != nullptr);
    str = getAttributeValue(attributes, "rating");
    irating = eu.ratings->indexOf(str);
    if (irating < 0) throw Exception("rating '" + string(str) + "' not found");

    assert(eu.factors != nullptr);
    str = getAttributeValue(attributes, "factor");
    ifactor = eu.factors->indexOf(str);
    if (ifactor < 0) throw Exception("factor '" + string(str) + "' not found");

    str = getAttributeValue(attributes, "lgd", nullptr);
    if (str != nullptr) {
      lgd = LGD(str);
    }

    assert(eu.segmentations != nullptr);
    vsegments.resize(eu.segmentations->size(), 0);
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
    vector<Asset*>(vassets.begin(),vassets.end()).swap(vassets);

    // important: coding obligor-segments as asset-segments
    for(int i=0; i<(int)eu.segmentations->size(); i++) {
      if (eu.segmentations->getSegmentation(i).components == Segmentation::obligor) {
        for(Asset *asset : vassets) {
          asset->addBelongsTo(i, vsegments[i]);
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
  for(Asset *asset : vassets)
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
  for(Asset *asset : vassets)
  {
    if (asset->requiresObligorLGD())
    {
      return true;
    }
  }
  return false;
}

