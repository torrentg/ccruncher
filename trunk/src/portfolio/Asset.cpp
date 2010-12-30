
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
ccruncher::Asset::Asset(Segmentations *segs, Recovery drecovery_) : vsegments(), data(), pdata()
{
  assert(segs != NULL);
  id = "NON_ASSIGNED";
  name = "NO_NAME";
  segmentations = segs;
  vsegments = vector<int>(segs->size(), 0);
  have_data = false;
  date = NAD;
  lastdate = NAD;
  drecovery = drecovery_;
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

//===========================================================================
// get recovery at date d
//===========================================================================
Recovery ccruncher::Asset::getRecovery(Date d)
{
  int n = (int) data.size();

  if (d < date)
  {
    return 0.0;
  }

  for(int i=0; i<n; i++)
  {
    if (d <= data[i].date)
    {
      return data[i].recovery;
    }
  }

  return 0.0;
}

//===========================================================================
// getLossX
//===========================================================================
double ccruncher::Asset::getLossX(Date d)
{
  if (d < date)
  {
    return 0.0;
  }

  int n = (int) data.size();
  double ret = 0.0;

  for(int i=0; i<n; i++)
  {
    if (d <= data[i].date)
    {
      ret += data[i].cashflow;
    }
  }

  return ret;
}

//===========================================================================
// precomputeLosses
//===========================================================================
void ccruncher::Asset::precomputeLosses(const Date &d1, const Date &d2, const Interest &interest)
{
  // allocating & initializing memory
  pdata.clear();
  pdata.reserve(data.size()+2);

  // computing cashflow/recoveries Current Net Value
  for (int i=0; i<(int)data.size(); i++) 
  {
    if (d1 <= data[i].date && data[i].date <= d2) 
    {
      double ufactor =  interest.getFactor(data[i].date, d1);
      data[i].cashflow *= ufactor;
    }
  }

  // precomputing losses (d1 <= t <= d2)
  for (int i=0; i<(int)data.size(); i++) 
  {
    Date d = data[i].date;
    
    if (d1 <= d && d <= d2) 
    {
      DateValues val(d, getLossX(d), getRecovery(d));
      pdata.push_back(val);
    }
  }
  
  // adding minimum event date
  Date mindate = max(date, d1);
  if (pdata.size() == 0 || mindate < pdata.front().date)
  {
    DateValues val(mindate, getLossX(mindate), getRecovery(mindate));
    pdata.insert(pdata.begin(), val);
  }
  
  // adding maximum event date
  Date maxdate = min(data.back().date, d2);
  if (pdata.back().date < maxdate)
  {
    DateValues val(maxdate, getLossX(maxdate), getRecovery(maxdate));
    pdata.push_back(val);
  }

  // deallocating unused memory  
  vector<DateValues>(pdata).swap(pdata);
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"values") && have_data == true) 
  {
    Date at = getDateAttribute(attributes, "at", NAD);
    double cashflow = getDoubleAttribute(attributes, "cashflow", NAN);
    Recovery recovery = drecovery;
    char *str = getAttributeValue(attributes, "recovery");
    if (str != NULL) {
      recovery = Recovery(str);
    }
    if (at == NAD || isnan(cashflow) || Recovery::isnan(recovery)) {
      throw Exception("invalid attributes at <values>");
    }
    else {
      DateValues aux(at, cashflow, recovery);
      insertDateValues(aux);
    }
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
    lastdate = data.back().date;

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
// insertDateValues
//===========================================================================
void ccruncher::Asset::insertDateValues(const DateValues &val) throw(Exception)
{
  // checking if date is previous to creation date
  if (val.date < date)
  {
    throw Exception("trying to insert an event with date previous to asset creation date");
  }

  // inserting date-values
  try
  {
    data.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
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
// getData
//===========================================================================
vector<DateValues>& ccruncher::Asset::getData()
{
  return data;
}

//===========================================================================
// getData
//===========================================================================
void ccruncher::Asset::deleteData()
{
  data.clear();
  // incredible but true, this shrink memory
  //std::vector<DateValues>(data.begin(), data.end()).swap(data);
  std::vector<DateValues>(0).swap(data);
}

//===========================================================================
// getMinDate
// be sure that precomputeLosses is called before the execution of this method
//===========================================================================
Date ccruncher::Asset::getMinDate() const
{
  return pdata.front().date;
}

//===========================================================================
// getMaxDate
// be sure that precomputeLosses is called before the execution of this method
//===========================================================================
Date ccruncher::Asset::getMaxDate() const
{
  return pdata.back().date;
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
  else if (from <= lastdate && lastdate <= to)
  {
    return true;
  }
  else if (date <= from && to <= lastdate)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//===========================================================================
// getRecovery
//===========================================================================
Recovery ccruncher::Asset::getRecovery() const
{
  return drecovery;
}

