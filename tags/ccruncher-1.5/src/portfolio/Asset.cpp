
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

#include <cmath>
#include <algorithm>
#include "portfolio/Asset.hpp"
#include "utils/Arrays.hpp"
#include "utils/Utils.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Asset::Asset(Segmentations *segs) : vsegments(), data(), ptimes(), plosses()
{
  assert(segs != NULL);
  id = "NON_ASSIGNED";
  name = "NO_NAME";
  segmentations = segs;
  vsegments = vector<int>(segs->size(), 0);
  have_data = false;
  date = Date(1,1,1);
  lastdate = Date(1,1,1);
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
double ccruncher::Asset::getRecovery(Date d)
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
  int n = (int) data.size();
  double ret = 0.0;

  if (d < date)
  {
    return 0.0;
  }

  for(int i=0; i<n; i++)
  {
    if (d <= data[i].date)
    {
      ret += data[i].cashflow;
    }
  }

  ret *= (1.0 - getRecovery(d));

  return ret;
}

//===========================================================================
// precomputeLosses
//===========================================================================
void ccruncher::Asset::precomputeLosses(const Date &d1, const Date &d2, const Interest &interest)
{
  // allocating & initializing memory
  ptimes.clear();
  plosses.clear();
  ptimes.reserve(data.size()+2);
  plosses.reserve(data.size()+2);

  // computing cashflow/recoveries Current Net Value
  for (int i=0; i<(int)data.size(); i++) 
  {
    if (d1 <= data[i].date && data[i].date <= d2) 
    {
      double ufactor =  interest.getUpsilon(data[i].date, d1);
      data[i].cashflow *= ufactor;
    }
  }

  // precomputing losses (d1 <= t <= d2)
  for (int i=0; i<(int)data.size(); i++) 
  {
    if (d1 <= data[i].date && data[i].date <= d2) 
    {
      ptimes.push_back(data[i].date);
      plosses.push_back(getLossX(data[i].date));
    }
  }
  
  // adding minimum event date
  Date mindate = max(date, d1);
  if (ptimes.size() == 0 || mindate < ptimes.front())
  {
    ptimes.insert(ptimes.begin(), mindate);
    plosses.insert(plosses.begin(), getLossX(mindate));
  }
  
  // adding maximum event date
  Date maxdate = min(data.back().date, d2);
  if (ptimes.back() < maxdate)
  {
    ptimes.push_back(maxdate);
    plosses.push_back(getLossX(maxdate));
  }

  // deallocating unused memory  
  vector<Date>(ptimes).swap(ptimes);
  vector<double>(plosses).swap(plosses);
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"values") && have_data == true) {
    Date at = getDateAttribute(attributes, "at", Date(1,1,1));
    double cashflow = getDoubleAttribute(attributes, "cashflow", NAN);
    double recovery = getDoubleAttribute(attributes, "recovery", NAN);

    if (at == Date(1,1,1) || isnan(cashflow) || isnan(recovery)) {
      throw Exception("invalid attributes at <values>");
    }
    else if (recovery < 0.0 || 1.0 < recovery) {
      throw Exception("recovery value out of range [0%,100%]");
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
    if (getNumAttributes(attributes) != 3) {
      throw Exception("incorrect number of attributes in tag asset");
    }
    else {
      id = getStringAttribute(attributes, "id", "");
      name = getStringAttribute(attributes, "name", "");
      date = getDateAttribute(attributes, "date", Date(1,1,1));
      if (id == "" || name == "" || date == Date(1,1,1))
      {
        throw Exception("invalid attributes at <asset>");
      }
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

  // checking if date exist
  for(unsigned int i=0;i<data.size();i++)
  {
    if (data[i].date == val.date)
    {
      throw Exception("trying to insert an existent date");
    }
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
bool ccruncher::Asset::belongsTo(int isegmentation, int isegment)
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
Date ccruncher::Asset::getMinDate()
{
  return ptimes.front();
}

//===========================================================================
// getMaxDate
// be sure that precomputeLosses is called before the execution of this method
//===========================================================================
Date ccruncher::Asset::getMaxDate()
{
  return ptimes.back();
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
        
