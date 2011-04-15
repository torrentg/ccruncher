
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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
  name = "NO_NAME";
  segmentations = segs;
  vsegments = vector<int>(segs->size(), 0);
  have_data = false;
  date = NAD;
  drecovery = Recovery(Recovery::Fixed,NAN);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Asset::~Asset()
{
  // nothing to do
}

//===========================================================================
// getId
//===========================================================================
string ccruncher::Asset::getId(void) const
{
  return id;
}

//===========================================================================
// getName
//===========================================================================
string ccruncher::Asset::getName(void) const
{
  return name;
}
#include "utils/Format.hpp"
//===========================================================================
// prepare data
//===========================================================================
void ccruncher::Asset::prepare(const Date &d1, const Date &d2, const Interest &interest)
{
  vector<DateValues> pdata;
  pdata.reserve(data.size()+2);

  // clipping data
  for (int i=0; i<(int)data.size(); i++) 
  {
    if (d1 <= data[i].date && data[i].date <= d2) 
    {
      DateValues val(data[i]);
      pdata.push_back(val);
    }
  }
  
  // adding minimum event date
  Date mindate = max(date, d1);
  if (pdata.size() == 0 || mindate < pdata.front().date)
  {
    DateValues val(mindate, Exposure(Exposure::Fixed,0.0), Recovery(Recovery::Fixed,1.0));
    for(unsigned int i=0; i<data.size(); i++)
    {
      if (data[i].date < mindate) continue;
      val.exposure = data[i].exposure;
      val.recovery = data[i].recovery;
      break;
    }
    pdata.insert(pdata.begin(), val);
  }
  
  // adding maximum event date
  Date maxdate = min(data.back().date, d2);
  if (pdata.back().date < maxdate)
  {
    DateValues val(maxdate, Exposure(Exposure::Fixed,0.0), Recovery(Recovery::Fixed,1.0));
    for(unsigned int i=0; i<data.size(); i++)
    {
      if (data[i].date < maxdate) continue;
      val.exposure = data[i].exposure;
      val.recovery = data[i].recovery;
      break;
    }
    pdata.push_back(val);
  }

  // computing Current Net Value
  for(unsigned int i=0; i<pdata.size(); i++)
  {
    //TODO: compute exposure current net value
    //pdata[i].exposure *= interest.getFactor(pdata[i].date, d1);
  }

  // reassigning data (with memory shrink)
  vector<DateValues>(pdata).swap(data);
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"values") && have_data == true)
  {
    Date at(date);
    char *str = getAttributeValue(attributes, "at");
    if (isInterval(str)) {
      at.addIncrement(str);
    }
    else {
      at = Date(str);
    }
    
    Exposure exposure(Exposure::Fixed, NAN);
    str = getAttributeValue(attributes, "exposure");
    if (str != NULL) {
      exposure = Exposure(str);
    }
    else {
      throw Exception("exposure not found");
    }

    Recovery recovery = drecovery;
    str = getAttributeValue(attributes, "recovery");
    if (str != NULL) {
      recovery = Recovery(str);
    }
    
    data.push_back(DateValues(at, exposure, recovery));
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
  else if (isEqual(name_,"asset")) {
    id = getStringAttribute(attributes, "id", "");
    name = getStringAttribute(attributes, "name", "");
    date = getDateAttribute(attributes, "date", NAD);
    char *str = getAttributeValue(attributes, "recovery");
    if (str != NULL) {
      drecovery = Recovery(str);
    }
    if (id == "" || name == "" || date == NAD)
    {
      throw Exception("invalid attributes at <asset>");
    }
  }
  else if (isEqual(name_,"data")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag data");
    }
    else {
      have_data = true;
    }
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epend(ExpatUserData &eu, const char *name_)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"values")) {
    // nothing to do
  }
  else if (isEqual(name_,"belongs-to")) {
    // nothing to do
  }
  else if (isEqual(name_,"asset")) 
  {
    // checking data size
    if (data.size() == 0) {
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
    
    // filling implicit segment
    try {
      int isegmentation = segmentations->indexOfSegmentation("assets");
      int isegment = segmentations->getSegmentation(isegmentation).addSegment(id);
      addBelongsTo(isegmentation, isegment);
    }
    catch(...) {
      // segmentation 'assets' not defined
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
  assert(isegmentation >= 0);
  assert(isegmentation < (int) vsegments.size());
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
bool ccruncher::Asset::belongsTo(int isegmentation, int isegment) const
{
  return (vsegments[isegmentation]==isegment);
}

//===========================================================================
// getMinDate
// be sure that precomputeData is called before the execution of this method
//===========================================================================
Date ccruncher::Asset::getMinDate() const
{
  return data.front().date;
}

//===========================================================================
// getMaxDate
// be sure that precomputeData is called before the execution of this method
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

