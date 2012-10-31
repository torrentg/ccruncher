
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
#include <algorithm>
#include "portfolio/Asset.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Asset::Asset(Segmentations *segs) : vsegments(), data()
{
  assert(segs != NULL);
  id = "NON_ASSIGNED";
  segmentations = segs;
  vsegments = vector<int>(segs->size(), 0);
  have_data = false;
  date = NAD;
  drecovery = Recovery(Recovery::Fixed,NAN);
}

//===========================================================================
// getId
//===========================================================================
string ccruncher::Asset::getId() const
{
  return id;
}

//===========================================================================
// prepare data
// assumes that data is sorted
//===========================================================================
void ccruncher::Asset::prepare(const Date &d1, const Date &d2, const Interest &interest)
{
  int pos1=-1, pos2=-1;

  assert(d1 <= d2);

  for (int i=0; i<(int)data.size(); i++) 
  {
    if (d1 <= data[i].date)
    {
      if (pos1 < 0)
      {
        pos1 = i;
      }
      pos2 = i;
      if (d2 <= data[i].date)
      {
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
  for(unsigned int i=0; i<data.size(); i++)
  {
    data[i].exposure.mult(interest.getFactor(data[i].date));
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"values") && have_data == true)
  {
/*
    Date at(date);
    const char *str = getAttributeValue(attributes, "at");
    if (isInterval(str)) {
      at.addIncrement(str);
    }
    else {
      at = Date(str);
    }
*/
    const char *str;
    DateValues values;

    //TODO: rename 'at' to 't' and allow dates and intervals from t0
    values.date = getDateAttribute(attributes, "at");

    str = getAttributeValue(attributes, "exposure");
    values.exposure = Exposure(str);

    str = getAttributeValue(attributes, "recovery", NULL);
    if (str != NULL) values.recovery = Recovery(str);
    else values.recovery = drecovery;
    
    data.push_back(values);
  }
  else if (isEqual(name_,"belongs-to"))
  {
    const char *ssegmentation = getAttributeValue(attributes, "segmentation");
    int isegmentation = segmentations->indexOfSegmentation(ssegmentation);

    const char *ssegment = getAttributeValue(attributes, "segment");
    int isegment = segmentations->getSegmentation(isegmentation).indexOfSegment(ssegment);

    addBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(name_,"asset"))
  {
    id = getStringAttribute(attributes, "id");
    date = getDateAttribute(attributes, "date");
    const char *str = getAttributeValue(attributes, "recovery", NULL);
    if (str != NULL) {
      drecovery = Recovery(str);
    }
  }
  else if (isEqual(name_,"data"))
  {
    if (getNumAttributes(attributes) != 0) throw Exception("attributes are not allowed in tag data");
    else have_data = true;
  }
  else
  {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"values")) {
    // nothing to do
  }
  else if (isEqual(name_,"belongs-to")) {
    // nothing to do
  }
  else if (isEqual(name_,"asset")) 
  {
    // checking data size
    if (data.empty()) {
      throw Exception("asset without data");
    }

    // sorting events by date
    sort(data.begin(), data.end());

    // checking for dates previous to asset date
    if (data[0].date < date)
    {
      throw Exception("asset with date values previous to asset creation date");
    }

    // checking for repeated dates
    for(unsigned int i=1; i<data.size(); i++)
    {
      if (data[i-1].date == data[i].date)
      {
        throw Exception("asset with repeated date");
      }
    }
  }
  else if (isEqual(name_,"data")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// addBelongsTo
//===========================================================================
void ccruncher::Asset::addBelongsTo(int isegmentation, int isegment) throw(Exception)
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
bool ccruncher::Asset::belongsTo(int isegmentation, int isegment) const
{
  return (vsegments[isegmentation]==isegment);
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Asset::getSegment(int isegmentation) const
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int) vsegments.size());
  return vsegments[isegmentation];
}

//===========================================================================
// getMinDate
// be sure that prepare() is called before the execution of this method
//===========================================================================
Date ccruncher::Asset::getMinDate() const
{
  return date;
}

//===========================================================================
// getMaxDate
// be sure that prepare() is called before the execution of this method
//===========================================================================
Date ccruncher::Asset::getMaxDate() const
{
  return data.back().date;
}

//===========================================================================
// isActive
//===========================================================================
bool ccruncher::Asset::isActive(const Date &from, const Date &to) throw(Exception)
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

//===========================================================================
// says if use obligor recovery
//===========================================================================
bool ccruncher::Asset::hasObligorRecovery() const
{
  for(unsigned int i=0; i<data.size(); i++)
  {
    if (!Recovery::valid(data[i].recovery))
    {
      return true;
    }
  }
  
  return false;
}

//===========================================================================
// returns reference to data
//===========================================================================
const vector<DateValues>& ccruncher::Asset::getData() const
{
  return data;
}

//===========================================================================
// getValues
// returns:
//    > (NAD,0,1) if at <= asset creation date
//    > (NAD,0,1) if asset has 0 date-values
//    > (NAD,0,1) if at > last date-values
//    > otherwise, returns the smallest date-values that is not less than at
//===========================================================================
const DateValues& ccruncher::Asset::getValues(const Date at) const
{
  static const DateValues dvnf(NAD, Exposure(Exposure::Fixed,0.0), Recovery(Recovery::Fixed,1.0));
  
  if (at <= date || data.empty() || data.back().date < at)
  {
    return dvnf;
  }
  else
  {
    return *(lower_bound(data.begin(), data.end(), DateValues(at)));
  }
}

