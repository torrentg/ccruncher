
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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
#include <climits>
#include <algorithm>
#include <cassert>
#include "portfolio/Portfolio.hpp"
#include "portfolio/Asset.hpp"
#include "params/Segmentations.hpp"
#include "params/Factors.hpp"
#include "params/Ratings.hpp"
#include "params/Interest.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Portfolio::epstart(ExpatUserData &eu, const char *tag, const char **attributes)
{
  if (isEqual(tag,"values")) {
    if (mStage != 4) throw Exception("syntax error");
    epstartValues(eu, attributes);
  }
  else if (isEqual(tag,"data")) {
    if (mStage != 3) throw Exception("syntax error");
    mStage = 4;
  }
  else if (isEqual(tag,"belongs-to")) {
    epstartBelongsto(eu, attributes);
  }
  else if (isEqual(tag,"asset")) {
    if (mStage != 2) throw Exception("syntax error");
    epstartAsset(eu, attributes);
    mStage = 3;
  }
  else if (isEqual(tag,"obligor")) {
    if (mStage != 1) throw Exception("syntax error");
    epstartObligor(eu, attributes);
    mStage = 2;
  }
  else if (isEqual(tag,"portfolio")) {
    if (mStage != 0) throw Exception("syntax error");
    epstartPortfolio(eu, attributes);
    mStage = 1;
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 */
void ccruncher::Portfolio::epend(ExpatUserData &eu, const char *tag)
{
  if (isEqual(tag,"values")) {
    assert(mStage == 4);
  }
  else if (isEqual(tag,"data")) {
    assert(mStage == 4);
    mStage--;
  }
  else if (isEqual(tag,"belongs-to")) {
    assert(mStage == 2 || mStage == 3);
  }
  else if (isEqual(tag,"asset")) {
    assert(mStage == 3);
    mStage--;
    ependAsset(eu);
  }
  else if (isEqual(tag,"obligor")) {
    assert(mStage == 2);
    mStage--;
    ependObligor(eu);
  }
  else if (isEqual(tag,"portfolio")) {
    assert(mStage == 1);
    mStage--;
    ependPortfolio(eu);
  }
  else {
    assert(false);
  }
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @param[in] attributes Element attributes.
 * @throw Exception if eu don't contain required objects (segmentations, date, etc.)
 */
void ccruncher::Portfolio::epstartPortfolio(ExpatUserData &eu, const char **attributes)
{
  if (getNumAttributes(attributes) != 0) {
    throw Exception("attributes are not allowed in tag portfolio");
  }

  // check parameters needed to parse assets/obligors
  if (eu.ratings == nullptr || eu.factors == nullptr || eu.segmentations == nullptr) {
    throw Exception("required parameters not found");
  }

  // sort segmentations by enabled (first) / disabled (last)
  stable_sort(eu.segmentations->begin(), eu.segmentations->end());
  mNumSegmentations = Segmentations::numEnabledSegmentations(*(eu.segmentations));
  obligorSegments.assign(mNumSegmentations, 0);
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @param[in] attributes Element attributes.
 * @throw Exception Error parsing obligor
 */
void ccruncher::Portfolio::epstartObligor(ExpatUserData &eu, const char **attributes)
{
  string id = getStringAttribute(attributes, "id");
  if(mIdObligors.find(id) != mIdObligors.end()) {
    string msg = "obligor id '" + id + "' repeated";
    throw Exception(msg);
  }
  else {
    mIdObligors[id] = mIdObligors.size() + 1;
  }

  Obligor obligor;
  const char *str = nullptr;

  str = getAttributeValue(attributes, "rating");
  obligor.irating = eu.ratings->indexOf(str);

  str = getAttributeValue(attributes, "factor");
  obligor.ifactor = eu.factors->indexOf(str);

  str = getAttributeValue(attributes, "lgd", nullptr);
  if (str != nullptr) {
    obligor.lgd = LGD(str);
  }

  mObligors.push_back(obligor);
  fill(obligorSegments.begin(), obligorSegments.end(), 0);
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @param[in] attributes Element attributes.
 * @throw Exception Error parsing asset
 */
void ccruncher::Portfolio::epstartAsset(ExpatUserData &, const char **attributes)
{
  string id = getStringAttribute(attributes, "id");
  if (mIdAssets.find(id) != mIdAssets.end()) {
    string msg = "asset id '" + id + "' repeated";
    throw Exception(msg);
  }
  else {
    mIdAssets[id] = mIdAssets.size() + 1;
  }

  Date date = getDateAttribute(attributes, "date");

  const char *str = getAttributeValue(attributes, "lgd", nullptr);
  if (str != nullptr) assetLGD = LGD(str);
  else assetLGD = LGD(LGD::Fixed,NAN);

  Asset asset(mNumSegmentations);
  asset.values.push_back(DateValues(date, NAN, 1.0));
  assert(!mObligors.empty());
  mObligors.back().assets.push_back(asset);
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @param[in] attributes Element attributes.
 * @throw Exception Syntax error or invalid segmentation-segment
 */
void ccruncher::Portfolio::epstartBelongsto(ExpatUserData &eu, const char **attributes)
{
  const char *ssegmentation = getAttributeValue(attributes, "segmentation");
  ushort isegmentation = eu.segmentations->indexOf(ssegmentation);

  const Segmentation &segmentation = eu.segmentations->at(isegmentation);

  const char *ssegment = getAttributeValue(attributes, "segment");
  size_t isegment = segmentation.indexOf(ssegment);

  vector<ushort> *segments = nullptr;

  if (mStage == 2) // obligor
  {
    if (segmentation.getType() != Segmentation::ComponentsType::obligor) {
      throw Exception("trying to assign an obligor to a segmentation composed of assets");
    }
    assert(!mObligors.empty());
    segments = &(obligorSegments);
  }
  else if (mStage == 3) //asset
  {
    if (segmentation.getType() != Segmentation::ComponentsType::asset) {
      throw Exception("trying to assign an asset to a segmentation composed of obligors");
    }
    assert(!mObligors.empty());
    assert(!mObligors.back().assets.empty());
    segments = &(mObligors.back().assets.back().segments);
  }
  else {
    throw Exception("syntax error");
  }

  // only enabled segmentations are considered
  if (isegmentation < mNumSegmentations)
  {
    assert(segments->size() == mNumSegmentations);
    if ((*segments)[isegmentation] > 0) {
      throw Exception("belongs-to defined twice");
    }
    (*segments)[isegmentation] = isegment;
  }
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @param[in] attributes Element attributes.
 * @throw Exception Error parsing asset
 */
void ccruncher::Portfolio::epstartValues(ExpatUserData &, const char **attributes)
{
  assert(!mObligors.empty());
  assert(!mObligors.back().assets.empty());
  assert(!mObligors.back().assets.back().values.empty());
  Asset &asset = mObligors.back().assets.back();

  const char *str;
  DateValues item;

  str = getAttributeValue(attributes, "t");
  if (isInterval(str)) {
    item.date = asset.values[0].date;
    item.date.add(str);
  }
  else {
    item.date = Date(str);
  }

  str = getAttributeValue(attributes, "ead");
  item.ead = EAD(str);

  str = getAttributeValue(attributes, "lgd", nullptr);
  if (str != nullptr) item.lgd = LGD(str);
  else item.lgd = assetLGD;

  if (item.date <= asset.values[0].date) {
    throw Exception("values with date previous or equal to asset creation date");
  }

  asset.values.push_back(item);
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @throw Exception Error consolidating obligor
 */
void ccruncher::Portfolio::ependObligor(ExpatUserData &eu)
{
  assert(!mObligors.empty());
  Obligor &obligor = mObligors.back();

  if (obligor.assets.empty()) {
    throw Exception("obligor without assets");
  }

  // assign obligor-segmentations to his assets
  for(ushort isegmentation=0; isegmentation<obligorSegments.size(); isegmentation++) {
    if (obligorSegments[isegmentation] > 0) {
      for(Asset &asset : obligor.assets) {
        asset.segments[isegmentation] = obligorSegments[isegmentation];
      }
    }
  }

  // remove obligor if inactive
  if (!obligor.isActive(eu.date1, eu.date2)) {
    mObligors.pop_back();
  }
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @throw Exception Error consolidating asset
 */
void ccruncher::Portfolio::ependAsset(ExpatUserData &eu)
{
  assert(!mObligors.empty());
  assert(!mObligors.back().assets.empty());
  Asset &asset = mObligors.back().assets.back();
  vector<DateValues> &values = asset.values;

  // checking data size
  if (values.empty() || (values.size()==1 && values[0].ead==EAD(0.0))) {
    throw Exception("asset without data");
  }

  // obtain asset creation date
  Date date0 = values[0].date;

  // sorting events by date
  sort(values.begin(), values.end());

  // checking for dates previous to asset date
  if (values[0].date < date0) {
    throw Exception("exist a date values previous to asset creation date");
  }

  // checking for repeated dates
  for(size_t i=1; i<values.size(); i++) {
    if (values[i-1].date == values[i].date) {
      throw Exception("repeated date values");
    }
  }

  // removing unused values and computing EAD current net value
  asset.prepare(eu.date1, eu.date2, *(eu.interest));
}

/**************************************************************************//**
 * @param[in] eu Xml parsing data.
 * @throw Exception Error consolidating asset
 */
void ccruncher::Portfolio::ependPortfolio(ExpatUserData &eu)
{
  if (mObligors.empty()) {
    throw Exception("portfolio without obligors");
  }

  mIdAssets.clear();
  mIdObligors.clear();

  assert(eu.segmentations != nullptr);
  Segmentations &segmentations = *(eu.segmentations);
  vector<Segmentation> aux = recodeSegments(segmentations);
  segmentations.swap(aux);
}

/**************************************************************************//**
 * @details Removes unused segmentations and unused segments.
 */
void ccruncher::Portfolio::reset()
{
  mStage = 0;
  mNumSegmentations = 0;
  assetLGD = LGD();
  vector<Obligor>().swap(mObligors);
  vector<ushort>().swap(obligorSegments);
  mIdAssets = map<std::string,size_t>();
  mIdObligors = map<std::string,size_t>();
}

/**************************************************************************//**
 * @param[in] segmentations List of segmentations.
 * @details Removes unused segments.
 */
vector<Segmentation> ccruncher::Portfolio::recodeSegments(const vector<Segmentation> &segmentations)
{
  if (mObligors.empty()) return segmentations;

  // TODO: sort segmentations reallocating obligors segments
  if (!is_sorted(segmentations.begin(), segmentations.end())) {
    throw Exception("segmentations are not sorted");
  }
  
  vector<vector<ushort>> table = getRecodeTable(segmentations);
  recodePortfolioSegments(table);
  vector<Segmentation> ret = getSegmentationsRecoded(segmentations, table);
  return ret;
}

/**************************************************************************//**
 * @param[in] segmentations List of segmentations to recode.
 * @return A list of arrays containing the new segment indexes. Note that
 *         segments order is preserved. USHRT_MAX means that this index is
 *         unused.
 */
vector<vector<ushort>> ccruncher::Portfolio::getRecodeTable(const vector<Segmentation> &segmentations)
{
  vector<vector<size_t>> hits = getHitsPerSegment(segmentations);
  vector<vector<ushort>> table(hits.size());

  for(size_t isegmentation=0; isegmentation<hits.size(); isegmentation++) {
    recodeSegmentation(hits[isegmentation], table[isegmentation]);
  }

  return table;
}

/**************************************************************************//**
 * @param[in] segmentations List of segmentations.
 * @return A list of arrays where the i-th row is the number of obligors
 *         per segment of the i-th segmentation.
 * @throw Exception There is an obligor with an unrecognized segmentation/segment.
 */
vector<vector<size_t>> ccruncher::Portfolio::getHitsPerSegment(const vector<Segmentation> &segmentations)
{
  vector<vector<size_t>> hits(segmentations.size());
  for(size_t i=0; i<segmentations.size(); i++) {
    hits[i].assign(segmentations[i].size(), 0);
  }

  for(Obligor &obligor : mObligors) {
    for(Asset &asset : obligor.assets) {
      for(size_t isegmentation=0; isegmentation<asset.segments.size(); isegmentation++) {
        ushort isegment = asset.segments[isegmentation];
        if (isegmentation >= hits.size() || isegment >= hits[isegmentation].size()) {
          throw Exception("portfolio contains an obligor with unrecognized segmentation/segment");
        }
        hits[isegmentation][isegment]++;
      }
    }
  }

  return hits;
}

/**************************************************************************//**
 * @param[in] hits Number of counts per segment.
 * @param[in] table Recode table to fill.
 */
void ccruncher::Portfolio::recodeSegmentation(const vector<size_t> &hits, vector<ushort> &table)
{
  ushort numSegments = 0;
  
  // recoding segment indexes
  for(size_t i=0; i<hits.size(); i++)
  {
    if (hits[i] == 0) {
      table.push_back(USHRT_MAX);
    }
    else {
      table.push_back(numSegments);
      numSegments++;
    }
  }

  // moving unassigned segment to the end
  if (hits[0] > 0)
  {
    assert(numSegments > 0);
    table[0] = numSegments-1;
    for(size_t i=1; i<table.size(); i++) {
      assert(table[i] != 0);
      if (table[i] != USHRT_MAX) table[i]--;
    }
  }
}

/**************************************************************************//**
 * @param[in] table Transformation map.
 */
void ccruncher::Portfolio::recodePortfolioSegments(const vector<vector<ushort>> &table)
{
  for(Obligor &obligor : mObligors) {
    for(Asset &asset : obligor.assets) {
      vector<ushort> &segments = asset.segments;
      for(size_t isegmentation=0; isegmentation<segments.size(); isegmentation++) {
        ushort oldSegment = segments[isegmentation];
        ushort newSegment = table[isegmentation][oldSegment];
        segments[isegmentation] = newSegment;
      }
    }
  }
}

/**************************************************************************/
vector<Segmentation> ccruncher::Portfolio::getSegmentationsRecoded(const vector<Segmentation> &segmentations, const vector<vector<ushort>> &table)
{
  vector<Segmentation> ret;
  
  for(size_t isegmentation=0; isegmentation<segmentations.size(); isegmentation++)
  {
    const Segmentation &segmentation = segmentations[isegmentation];
    if (segmentation.isEnabled()) 
    {
      ret.push_back(Segmentation(segmentation.getName(), segmentation.getType(), true, false));
      Segmentation &current = ret.back();
      bool hasUnassignedSegment = (table[isegmentation][0] != USHRT_MAX);
      for(size_t i=(hasUnassignedSegment?1:0); i<table[isegmentation].size(); i++)
      {
        if (table[isegmentation][i] != USHRT_MAX)
        {
          // recode table preserves order (except unassigned)
          assert(current.size() == table[isegmentation][i]);
          string segmentName = segmentation.getSegment(i);
          current.addSegment(segmentName);
        }
      }
      if (hasUnassignedSegment) {
        assert(current.size() == table[isegmentation][0]);
        string segmentName = segmentation.getSegment(0);
        current.addSegment(segmentName);
      }
    }
  }
  
  return ret;
}

