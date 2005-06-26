
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// Asset.cpp - Asset code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . asset refactoring
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/22 - Gerard Torrent [gerard@fobos.generacio.com]
//   . solved bug related to getSegment method (rest segment = default)
//
//===========================================================================

#include <cmath>
#include <cassert>
#include <algorithm>
#include "portfolio/Asset.hpp"

//===========================================================================
// constructor (don't use it)
//===========================================================================
ccruncher::Asset::Asset()
{
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Asset::Asset(Segmentations *segs)
{
  // setting default values
  reset(segs);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Asset::~Asset()
{
  // nothing to do
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Asset::reset(Segmentations *segs)
{
  id = "NON_ASSIGNED";
  name = "NO_NAME";
  segmentations = segs;
  data.clear();
  belongsto.clear();
  have_data = false;
}

//===========================================================================
// getId
//===========================================================================
string ccruncher::Asset::getId(void)
{
  return id;
}

//===========================================================================
// getName
//===========================================================================
string ccruncher::Asset::getName(void)
{
  return name;
}

//===========================================================================
// getVCashFlow between date1 and date2
//===========================================================================
double ccruncher::Asset::getVCashFlow(Date &date1, Date &date2, Interest *spot)
{
  int n = (int) data.size();
  double ret = 0.0;

  if (date2 < data[0].date)
  {
    return 0.0;
  }

  for(int i=0;i<n;i++)
  {
    if (date1 == date2 && date1 == data[i].date)
    {
      ret += data[i].cashflow * spot->getUpsilon(data[i].date, date2);
      break;
    }
    if (date1 < data[i].date && data[i].date <= date2)
    {
      ret += data[i].cashflow * spot->getUpsilon(data[i].date, date2);
    }
    if (date2 < data[i].date)
    {
      break;
    }
  }

  return ret;
}

//===========================================================================
// getVExposure at date2
//===========================================================================
double ccruncher::Asset::getVExposure(Date &date1, Date &date2, Interest *spot)
{
  int n = (int) data.size();
  double ret = 0.0;

  if (n == 0)
  {
    return 0.0;
  }

  if (date2 < data[0].date)
  {
    return 0.0;
  }

  if (date1 > data[n-1].date)
  {
    return 0.0;
  }

  if (date1 < data[n-1].date && data[n-1].date < date2)
  {
    double val1 = data[n-1].date - date1;
    double val2 = date2 - date1;

    return data[n-1].exposure * val1/val2 * spot->getUpsilon(data[n-1].date, date2);
  }

  for(int i=1;i<n;i++)
  {
    if (date2 <= data[i].date)
    {
      Date datex = data[i-1].date;
      double ex = data[i-1].exposure * spot->getUpsilon(data[i-1].date, date2);
      Date datey = data[i].date;
      double ey = data[i].exposure * spot->getUpsilon(data[i].date, date2);

      ret = ex + (date2-datex)*(ey - ex)/(datey - datex);

      return ret;
    }
  }

  // assertion, this line is never reached
  assert(false);
  return 0.0;
}

//===========================================================================
// getVRecovery at date2
//===========================================================================
double ccruncher::Asset::getVRecovery(Date &date1, Date &date2, Interest *spot)
{
  int n = (int) data.size();
  double ret = 0.0;

  if (n == 0)
  {
    return 0.0;
  }

  if (date2 < data[0].date)
  {
    return 0.0;
  }

  if (date1 > data[n-1].date)
  {
    return 0.0;
  }

  if (date1 < data[n-1].date && data[n-1].date < date2)
  {
    double val1 = data[n-1].date - date1;
    double val2 = date2 - date1;

    return data[n-1].recovery * val1/val2 * spot->getUpsilon(data[n-1].date, date2);
  }

  for(int i=1;i<n;i++)
  {
    if (date2 <= data[i].date)
    {
      Date datex = data[i-1].date;
      double ex = data[i-1].recovery * spot->getUpsilon(data[i-1].date, date2);
      Date datey = data[i].date;
      double ey = data[i].recovery * spot->getUpsilon(data[i].date, date2);

      ret = ex + (date2-datex)*(ey - ex)/(datey - datex);

      return ret;
    }
  }

  // assertion, this line is never reached
  assert(false);
  return 0.0;
}

//===========================================================================
// getVertexes
//===========================================================================
void ccruncher::Asset::getVertexes(Date *dates, int n, Interests *ints, DateValues *ret)
{
  double ufactor;
  Interest *spot = ints->getInterest("spot");

  sort(dates, dates+n);

  for (int i=0;i<n;i++)
  {
    ufactor =  spot->getUpsilon(dates[i], dates[0]);
    ret[i].date = dates[i];
    ret[i].cashflow = getVCashFlow(dates[max(i-1,0)], dates[i], spot) * ufactor;
    ret[i].exposure = getVExposure(dates[max(i-1,0)], dates[i], spot) * ufactor;
    ret[i].recovery = getVRecovery(dates[max(i-1,0)], dates[i], spot) * ufactor;
  }

  for (int i=1;i<n;i++)
  {
    ret[i].cashflow += ret[i-1].cashflow;
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"asset")) {
    if (getNumAttributes(attributes) != 2) {
      throw eperror(eu, "incorrect number of attributes in tag asset");
    }
    else {
      id = getStringAttribute(attributes, "id", "");
      name = getStringAttribute(attributes, "name", "");
      if (id == "" || name == "")
      {
        throw eperror(eu, "invalid attributes at <asset>");
      }
    }
  }
  else if (isEqual(name_,"belongs-to")) {
    string sconcept = getStringAttribute(attributes, "concept", "");
    string ssegment = getStringAttribute(attributes, "segment", "");

    int iconcept = segmentations->getSegmentation(sconcept);
    int isegment = segmentations->getSegment(sconcept, ssegment);

    insertBelongsTo(iconcept, isegment);
  }
  else if (isEqual(name_,"data")) {
    if (getNumAttributes(attributes) != 0) {
      throw eperror(eu, "attributes are not allowed in tag data");
    }
    else {
      have_data = true;
    }
  }
  else if (isEqual(name_,"values") && have_data == true) {
    Date date = getDateAttribute(attributes, "at", Date(1,1,1));
    double cashflow = getDoubleAttribute(attributes, "cashflow", NAN);
    double exposure = getDoubleAttribute(attributes, "exposure", NAN);
    double recovery = getDoubleAttribute(attributes, "recovery", NAN);

    if (date == Date(1,1,1) || isnan(cashflow) || isnan(exposure) || isnan(recovery)) {
      throw eperror(eu, "invalid attributes at <values>");
    }
    else {
      DateValues aux(date, cashflow, exposure, recovery);
      insertDateValues(aux);
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"asset")) {
    // checking data size
    if (data.size() == 0) {
      throw eperror(eu, "asset without data");
    }
    else {
      // sorting data by date
      sort(data.begin(), data.end());
    }
    // filling implicit segment
    if (segmentations->getSegmentation("asset") >= 0) {
      if (segmentations->getComponents("asset") == asset) {
        segmentations->addSegment("asset", id);
        int iconcept = segmentations->getSegmentation("asset");
        int isegment = segmentations->getSegment("asset", id);
        insertBelongsTo(iconcept, isegment);
      }
    }
    // filling implicit segment
    if (segmentations->getSegmentation("portfolio") >= 0) {
      if (segmentations->getComponents("portfolio") == asset) {
        int iconcept = segmentations->getSegmentation("portfolio");
        int isegment = segmentations->getSegment("portfolio", "rest");
        insertBelongsTo(iconcept, isegment);
      }
    }
  }
  else if (isEqual(name_,"belongs-to")) {
    // nothing to do
  }
  else if (isEqual(name_,"data")) {
    // nothing to do
  }
  else if (isEqual(name_,"values")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// insertDateValues
//===========================================================================
void ccruncher::Asset::insertDateValues(DateValues &val) throw(Exception)
{
  // checking if date exist
  for(unsigned int i=0;i<data.size();i++)
  {
    if (data[i].date == val.date)
    {
      throw Exception("Asset::insertDateValues(): trying to insert an existent date");
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
void ccruncher::Asset::addBelongsTo(int iconcept, int isegment) throw(Exception)
{
  insertBelongsTo(iconcept, isegment);
}

//===========================================================================
// insertBelongsTo
//===========================================================================
void ccruncher::Asset::insertBelongsTo(int iconcept, int isegment) throw(Exception)
{
  assert(iconcept >= 0);
  assert(isegment >= 0);

  if (getSegment(iconcept) > 0)
  {
    throw Exception("Asset::insertBelongsTo(): trying to reinsert a defined concept");
  }

  if (isegment > 0)
  {
    belongsto[iconcept] = isegment;
  }
  else
  {
    // isegment=0 (rest segment) is the default segment, not inserted
  }
}

//===========================================================================
// belongsTo
//===========================================================================
bool ccruncher::Asset::belongsTo(int iconcept, int isegment)
{
  return ((getSegment(iconcept)==isegment)?true:false);
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Asset::getSegment(int iconcept)
{
  map<int,int>::iterator pos = belongsto.find(iconcept);

  if (pos != belongsto.end())
  {
    return pos->second;
  }
  else
  {
    // by default belongs to segment 'rest' (0)
    return 0;
  }
}

//===========================================================================
// getData
//===========================================================================
vector<DateValues>* ccruncher::Asset::getData()
{
  return &data;
}
