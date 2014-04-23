
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
ccruncher::Asset::Asset(size_t nsegmentations) : vsegments(nsegmentations, 0), data()
{
  id = "NON_ASSIGNED";
  have_data = false;
  date = NAD;
  dlgd = LGD(LGD::Fixed,NAN);
  // initial reserve to avoid continuous reallocation
  data.reserve(256);
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
  for(int i=0; i<(int)data.size(); i++) 
  {
    if (d1 < data[i].date)
    {
      if (pos1 < 0) {
        pos1 = i;
      }
      pos2 = i;
      if (d2 <= data[i].date) {
        break;
      }
    }
  }

  assert(pos1 <= pos2);

  // memory shrink
  if (pos1 < 0) {
    vector<DateValues>(0).swap(data);
  }
  else {
    vector<DateValues>(data.begin()+pos1, data.begin()+pos2+1).swap(data);
  }
  
  // computing Current Net Value
  for(DateValues &dv : data)
  {
    dv.ead.mult(interest.getFactor(dv.date));
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] name_ Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Asset::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"values") && have_data == true)
  {
    const char *str;
    DateValues values;

    str = getAttributeValue(attributes, "t");
    if (isInterval(str)) {
      values.date = date;
      values.date.add(str);
    }
    else {
      values.date = Date(str);
    }

    str = getAttributeValue(attributes, "ead");
    values.ead = EAD(str);

    str = getAttributeValue(attributes, "lgd", nullptr);
    if (str != nullptr) values.lgd = LGD(str);
    else values.lgd = dlgd;
    
    data.push_back(values);
  }
  else if (isEqual(name_,"belongs-to"))
  {
    assert(eu.segmentations != nullptr);
    const char *ssegmentation = getAttributeValue(attributes, "segmentation");
    int isegmentation = eu.segmentations->indexOfSegmentation(ssegmentation);

    const Segmentation &segmentation = eu.segmentations->getSegmentation(isegmentation);
    if (segmentation.components != Segmentation::asset) {
      throw Exception("trying to assign an asset to a segmentation composed of obligors");
    }

    const char *ssegment = getAttributeValue(attributes, "segment");
    int isegment = segmentation.indexOfSegment(ssegment);

    addBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(name_,"asset"))
  {
    id = getStringAttribute(attributes, "id");
    date = getDateAttribute(attributes, "date");
    data.push_back(DateValues(date, EAD(EAD::Fixed,0.0), LGD(LGD::Fixed,0.0)));
    const char *str = getAttributeValue(attributes, "lgd", nullptr);
    if (str != nullptr) {
      dlgd = LGD(str);
    }
    assert(eu.segmentations != nullptr);
    vsegments.resize(eu.segmentations->size(), 0);
  }
  else if (isEqual(name_,"data"))
  {
    if (getNumAttributes(attributes) != 0) throw Exception("attributes are not allowed in tag data");
    else have_data = true;
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
void ccruncher::Asset::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"asset"))
  {
    // checking data size
    if (data.empty() || (data.size()==1 && data[0].date==date)) {
      throw Exception("asset without data");
    }

    // sorting events by date
    sort(data.begin(), data.end());

    // checking for dates previous to asset date
    if (data[0].date < date)
    {
      throw Exception("exist a date values previous to asset creation date");
    }

    // checking for repeated dates
    for(unsigned int i=1; i<data.size(); i++)
    {
      if (data[i-1].date == data[i].date)
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
bool ccruncher::Asset::belongsTo(int isegmentation, int isegment) const
{
  assert(0 <= isegmentation && isegmentation < (int)vsegments.size());
  return (vsegments[isegmentation]==isegment);
}

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @return Segment index.
 */
int ccruncher::Asset::getSegment(int isegmentation) const
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int) vsegments.size());
  return vsegments[isegmentation];
}

/**************************************************************************//**
 * @return List of segments adscribed to this asset.
 */
const std::vector<int>& ccruncher::Asset::getSegments() const
{
  return vsegments;
}

/**************************************************************************//**
 * @param[in] isegmentation Segmentation index.
 * @param[in] isegment Segment index.
 */
void ccruncher::Asset::setSegment(int isegmentation, int isegment)
{
  assert(0 <= isegmentation && isegmentation < (int)vsegments.size());
  vsegments[isegmentation] = isegment;
}

/**************************************************************************//**
 * @note Be sure that Asset::prepare() is called before this method call.
 * @return Minimum date.
 */
Date ccruncher::Asset::getMinDate() const
{
  return date;
}

/**************************************************************************//**
 * @note Be sure that Asset::prepare() is called before this method call.
 * @return Maximum date.
 */
Date ccruncher::Asset::getMaxDate() const
{
  return data.back().date;
}

/**************************************************************************//**
 * @param[in] from Starting analysis date.
 * @param[in] to Ending analysis date.
 * @return True if this asset has credit risk in the given time range,
 *         false otherwise.
 */
bool ccruncher::Asset::isActive(const Date &from, const Date &to)
{
  if (data.empty())
  {
    return false;
  }
  if (from <= date && date <= to)
  {
    return true;
  }
  else if (from <= data.back().date && data.back().date <= to)
  {
    return true;
  }
  else if (date <= from && to <= data.back().date)
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
  for(const DateValues &dv : data)
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
  return data;
}

/**************************************************************************//**
 * @details Free remaining data.
 * @see http://www.cplusplus.com/reference/vector/vector/clear/
 */
void ccruncher::Asset::clearData()
{
  vector<DateValues>(0).swap(data);
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
  
  if (t <= date || data.empty() || data.back().date < t)
  {
    return dvnf;
  }
  else
  {
    return *(lower_bound(data.begin(), data.end(), DateValues(t)));
  }
}

