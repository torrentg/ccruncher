
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
// Asset.cpp - Asset code - $Rev$
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
// 2005/07/09 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed exposure/recovery by netting
//
// 2005/07/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . moved <cassert> include at last position
//
// 2005/08/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . tag concept renamed to segmentation
//
// 2005/09/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . update to t[0] -> update to t[n-1]
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Interests class refactoring
//   . Segmentations class refactoring
//   . Asset refactoring
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Changed Net Current Value computation (now all cashflows are
//     taked in account. Previously, only chasflows between initial
//     and end simulation date was taked in account). Thanks GG.
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "portfolio/Asset.hpp"
#include <cassert>

//===========================================================================
// constructor (don't use it)
//===========================================================================
ccruncher::Asset::Asset()
{
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Asset::Asset(const Segmentations &segs)
{
  // setting default values
  reset((Segmentations *) &segs);
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
// getVCashFlow between date1 and date2
//===========================================================================
double ccruncher::Asset::getVCashFlow(Date &date1, Date &date2, const Interest &spot, bool last)
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
      ret += data[i].cashflow * spot.getUpsilon(data[i].date, date2);
      break;
    }
    if (date1 < data[i].date && data[i].date <= date2)
    {
      ret += data[i].cashflow * spot.getUpsilon(data[i].date, date2);
    }
    if (date2 < data[i].date)
    {
      if (last == false)
      {
        break;
      }
      else
      {
        ret += data[i].cashflow * spot.getUpsilon(data[i].date, date2);
      }
    }
  }

  return ret;
}

//===========================================================================
// getVNetting at date2
//===========================================================================
double ccruncher::Asset::getVNetting(Date &date1, Date &date2, const Interest &spot)
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

    return data[n-1].netting * val1/val2 * spot.getUpsilon(data[n-1].date, date2);
  }

  for(int i=1;i<n;i++)
  {
    if (date2 <= data[i].date)
    {
      Date datex = data[i-1].date;
      double ex = data[i-1].netting * spot.getUpsilon(data[i-1].date, date2);
      Date datey = data[i].date;
      double ey = data[i].netting * spot.getUpsilon(data[i].date, date2);

      ret = ex + (date2-datex)*(ey - ex)/(datey - datex);

      return ret;
    }
  }

  // assertion, this line is never reached
  assert(false);
  return 0.0;
}

//===========================================================================
// getLosses
//===========================================================================
void ccruncher::Asset::getLosses(Date *dates, int n, Interests &interests, double *ret)
{
  double ufactor;
  Interest &spot = interests["spot"];

  // sorting dates
  sort(dates, dates+n);

  // computing losses (=unreceived cashflow-recovery)
  for (int i=0;i<n;i++)
  {
    ret[i] = 0.0;
    for(unsigned int j=0;j<data.size();j++)
    {
      if (dates[i] <= data[j].date)
      {
        ufactor =  spot.getUpsilon(data[j].date, dates[n-1]);
        ret[i] += ufactor * data[j].cashflow;
      }
    }
    ufactor =  spot.getUpsilon(dates[i], dates[n-1]);
    ret[i] -= getVNetting(dates[max(i-1,0)], dates[i], spot) * ufactor;
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
    string ssegmentation = getStringAttribute(attributes, "segmentation", "");
    string ssegment = getStringAttribute(attributes, "segment", "");

    if (ssegmentation == "" || ssegment == "") {
      throw eperror(eu, "invalid attributes at <belongs-to> tag");
    }

    int isegmentation = (*segmentations)[ssegmentation].order;
    int isegment = (*segmentations)[ssegmentation][ssegment].order;

    insertBelongsTo(isegmentation, isegment);
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
    double netting = getDoubleAttribute(attributes, "netting", NAN);

    if (date == Date(1,1,1) || isnan(cashflow) || isnan(netting)) {
      throw eperror(eu, "invalid attributes at <values>");
    }
    else {
      DateValues aux(date, cashflow, netting);
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
    try
    {
      if ((*segmentations)["asset"].components == asset)
      {
        segmentations->addSegment("asset", id);
        int isegmentation = (*segmentations)["asset"].order;
        int isegment = (*segmentations)["asset"][id].order;
        insertBelongsTo(isegmentation, isegment);
      }
    }
    catch(...)
    {
      // segmentation 'asset' not defined
    }

    // filling implicit segment
    try
    {
      if ((*segmentations)["portfolio"].components == asset)
      {
        int isegmentation = (*segmentations)["portfolio"].order;
        int isegment = (*segmentations)["portfolio"]["rest"].order;
        insertBelongsTo(isegmentation, isegment);
      }
    }
    catch(...)
    {
      // segmentation 'portfolio' not found
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
void ccruncher::Asset::insertDateValues(const DateValues &val) throw(Exception)
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
void ccruncher::Asset::addBelongsTo(int isegmentation, int isegment) throw(Exception)
{
  insertBelongsTo(isegmentation, isegment);
}

//===========================================================================
// insertBelongsTo
//===========================================================================
void ccruncher::Asset::insertBelongsTo(int isegmentation, int isegment) throw(Exception)
{
  assert(isegmentation >= 0);
  assert(isegment >= 0);

  if (getSegment(isegmentation) > 0)
  {
    throw Exception("Asset::insertBelongsTo(): trying to reinsert a defined segmentation");
  }

  if (isegment > 0)
  {
    belongsto[isegmentation] = isegment;
  }
  else
  {
    // isegment=0 (rest segment) is the default segment, not inserted
  }
}

//===========================================================================
// belongsTo
//===========================================================================
bool ccruncher::Asset::belongsTo(int isegmentation, int isegment)
{
  return ((getSegment(isegmentation)==isegment)?true:false);
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Asset::getSegment(int isegmentation)
{
  map<int,int>::iterator pos = belongsto.find(isegmentation);

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
vector<DateValues>& ccruncher::Asset::getData()
{
  return data;
}
