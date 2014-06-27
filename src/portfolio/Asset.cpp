
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
#include <algorithm>
#include <cassert>
#include "portfolio/Asset.hpp"
#include "params/Segmentations.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] nsegmentations Number of segmentations. If this object is
 *            initialized using epstart-epend then segmentations will be
 *            retrieved from the ExpatUserData object. Otherwise you need
 *            to specify the number of segmentations.
 */
ccruncher::Asset::Asset(size_t nsegmentations) : mSegments(nsegmentations, 0), mValues()
{
  mId = "NON_ASSIGNED";
  have_data = false;
  mDate = NAD;
  mDefaultLgd = LGD(LGD::Fixed,NAN);
  // initial reserve to avoid continuous reallocation
  mValues.reserve(256);
}

/**************************************************************************//**
 * @details Prepare data once they have been readed from input file.
 *          Data preparation consist on:
 *          - Remove data previous to starting date analysis.
 *          - Remove data just after ending analysis date (except the first one).
 *          - Compute Current Net Value of EAD's at starting date analysis.
 * @param[in] d1 Initial analysis date.
 * @param[in] d2 Ending analysis date.
 * @param[in] interest Yield curve in the analyzed time range.
 */
void ccruncher::Asset::prepare(const Date &d1, const Date &d2, const Interest &interest)
{
  int pos1=-1, pos2=-1;

  assert(d1 <= d2);

  // assumes that data is sorted.
  for(int i=0; i<(int)mValues.size(); i++) 
  {
    if (d1 < mValues[i].date)
    {
      if (pos1 < 0) {
        pos1 = i;
      }
      pos2 = i;
      if (d2 <= mValues[i].date) {
        break;
      }
    }
  }

  assert(pos1 <= pos2);

  // memory shrink
  if (pos1 < 0) {
    vector<DateValues>(0).swap(mValues);
  }
  else {
    vector<DateValues>(mValues.begin()+pos1, mValues.begin()+pos2+1).swap(mValues);
  }
  
  // computing Current Net Value
  for(DateValues &dv : mValues)
  {
    dv.ead.mult(interest.getFactor(dv.date));
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Asset::epstart(ExpatUserData &eu, const char *tag, const char **attributes)
{
  if (isEqual(tag,"values") && have_data == true)
  {
    const char *str;
    DateValues values;

    str = getAttributeValue(attributes, "t");
    if (isInterval(str)) {
      values.date = mDate;
      values.date.add(str);
    }
    else {
      values.date = Date(str);
    }

    str = getAttributeValue(attributes, "ead");
    values.ead = EAD(str);

    str = getAttributeValue(attributes, "lgd", nullptr);
    if (str != nullptr) values.lgd = LGD(str);
    else values.lgd = mDefaultLgd;
    
    mValues.push_back(values);
  }
  else if (isEqual(tag,"belongs-to"))
  {
    assert(eu.segmentations != nullptr);
    const char *ssegmentation = getAttributeValue(attributes, "segmentation");
    int isegmentation = eu.segmentations->indexOfSegmentation(ssegmentation);

    const Segmentation &segmentation = eu.segmentations->getSegmentation(isegmentation);
    if (segmentation.getType() != Segmentation::asset) {
      throw Exception("trying to assign an asset to a segmentation composed of obligors");
    }

    const char *ssegment = getAttributeValue(attributes, "segment");
    size_t isegment = segmentation.indexOf(ssegment);

    addBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(tag,"asset"))
  {
    mId = getStringAttribute(attributes, "id");
    mDate = getDateAttribute(attributes, "date");
    mValues.push_back(DateValues(mDate, EAD(EAD::Fixed,0.0), LGD(LGD::Fixed,0.0)));
    const char *str = getAttributeValue(attributes, "lgd", nullptr);
    if (str != nullptr) {
      mDefaultLgd = LGD(str);
    }
    assert(eu.segmentations != nullptr);
    mSegments.resize(eu.segmentations->size(), 0);
  }
  else if (isEqual(tag,"data"))
  {
    if (getNumAttributes(attributes) != 0) throw Exception("attributes are not allowed in tag data");
    else have_data = true;
  }
  else
  {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 */
void ccruncher::Asset::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"asset"))
  {
    // checking data size
    if (mValues.empty() || (mValues.size()==1 && mValues[0].date==mDate)) {
      throw Exception("asset without data");
    }

    // sorting events by date
    sort(mValues.begin(), mValues.end());

    // checking for dates previous to asset date
    if (mValues[0].date < mDate)
    {
      throw Exception("exist a date values previous to asset creation date");
    }

    // checking for repeated dates
    for(unsigned int i=1; i<mValues.size(); i++)
    {
      if (mValues[i-1].date == mValues[i].date)
      {
        throw Exception("repeated date values");
      }
    }
  }
}

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @param[in] isegment Segment index.
 * @throw Exception Redefined relation.
 */
void ccruncher::Asset::addBelongsTo(int isegmentation, int isegment)
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
bool ccruncher::Asset::belongsTo(int isegmentation, int isegment) const
{
  assert(0 <= isegmentation && isegmentation < (int)mSegments.size());
  return (mSegments[isegmentation]==isegment);
}

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @return Segment index.
 */
int ccruncher::Asset::getSegment(int isegmentation) const
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int) mSegments.size());
  return mSegments[isegmentation];
}

/**************************************************************************//**
 * @return List of segments adscribed to this asset.
 */
const std::vector<int>& ccruncher::Asset::getSegments() const
{
  return mSegments;
}

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @param[in] isegment Segment index.
 */
void ccruncher::Asset::setSegment(int isegmentation, int isegment)
{
  assert(0 <= isegmentation && isegmentation < (int)mSegments.size());
  mSegments[isegmentation] = isegment;
}

/**************************************************************************//**
 * @note Be sure that Asset::prepare() is called before this method call.
 * @return Minimum date.
 */
Date ccruncher::Asset::getMinDate() const
{
  return mDate;
}

/**************************************************************************//**
 * @note Be sure that Asset::prepare() is called before this method call.
 * @return Maximum date.
 */
Date ccruncher::Asset::getMaxDate() const
{
  return mValues.back().date;
}

/**************************************************************************//**
 * @param[in] from Starting analysis date.
 * @param[in] to Ending analysis date.
 * @return True if this asset has credit risk in the given time range,
 *         false otherwise.
 */
bool ccruncher::Asset::isActive(const Date &from, const Date &to)
{
  if (mValues.empty())
  {
    return false;
  }
  if (from <= mDate && mDate <= to)
  {
    return true;
  }
  else if (from <= mValues.back().date && mValues.back().date <= to)
  {
    return true;
  }
  else if (mDate <= from && to <= mValues.back().date)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**************************************************************************//**
 * @details Check if all asset's DateValues have defined the LGD. If exist
 *          a DateValue without LGD this means that obligor's LGD will be
 *          used.
 * @return true = exist a DateValue that requires obligor LGD, false otherwise.
 */
bool ccruncher::Asset::requiresObligorLGD() const
{
  for(const DateValues &dv : mValues)
  {
    if (!LGD::isvalid(dv.lgd))
    {
      return true;
    }
  }
  return false;
}

/**************************************************************************/
const vector<DateValues>& ccruncher::Asset::getData() const
{
  return mValues;
}

/**************************************************************************//**
 * @details Free remaining data.
 * @see http://www.cplusplus.com/reference/vector/vector/clear/
 */
void ccruncher::Asset::clearData()
{
  vector<DateValues>(0).swap(mValues);
}

/**************************************************************************//**
 * @deprecated Currently this method is unused in the simulation. This task
 *          is done in the SimulationThread::simuleObligorLoss(). The
 *          method is preserved to test the procedure.
 * @return The DateValue at given date:
 *         - (NAD,0,1) if t <= asset creation date
 *         - (NAD,0,1) if asset has 0 date-values
 *         - (NAD,0,1) if t > last date-values
 *         - otherwise, returns the smallest date-values that is not less than t
 */
const DateValues& ccruncher::Asset::getValues(const Date t) const
{
  static const DateValues dvnf(NAD, EAD(EAD::Fixed,0.0), LGD(LGD::Fixed,1.0));
  
  if (t <= mDate || mValues.empty() || mValues.back().date < t)
  {
    return dvnf;
  }
  else
  {
    return *(lower_bound(mValues.begin(), mValues.end(), DateValues(t)));
  }
}

