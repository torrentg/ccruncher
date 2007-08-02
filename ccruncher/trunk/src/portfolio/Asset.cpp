
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
// 2006/01/05 - Gerard Torrent [gerard@fobos.generacio.com]
//   . netting replaced by recovery
//
// 2006/02/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed method ExpatHandlers::eperror()
//
// 2007/07/29 - Gerard Torrent [gerard@mail.generacio.com]
//   . added asset creation date
//   . getLosses function reviewed
//   . added precomputeLosses function
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "portfolio/Asset.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

//===========================================================================
// constructor (don't use it)
//===========================================================================
ccruncher::Asset::Asset()
{
  reset(NULL);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Asset::Asset(const Segmentations &segs)
{
  reset((Segmentations *) &segs);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Asset::~Asset()
{
  if (plosses != NULL)
  {
    Arrays<double>::deallocVector(plosses);
    plosses = NULL;
  }
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
  plosses = NULL;
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
// getDate
//===========================================================================
Date ccruncher::Asset::getDate(void) const
{
  return date;
}

//===========================================================================
// getRightIdx
//===========================================================================
int ccruncher::Asset::getRightIdx(Date d)
{
  int n = (int) data.size();

  if (d <= data[0].date)
  {
    return 0;
  }

  if (data[n-1].date < d)
  {
    return -1;
  }

  for(int i=0; i<n; i++)
  {
    if (d <= data[i].date) 
    {
      return i;
    }
  }

  assert(false);
  return -1;
}

//===========================================================================
// getCashflowSum
//===========================================================================
double ccruncher::Asset::getCashflowSum(Date d, const Interest &spot)
{
  double ufactor;
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
      ufactor =  spot.getUpsilon(data[i].date, d);
      ret += ufactor * data[i].cashflow;
    }
  }

  return ret;
}

//===========================================================================
// precomputeLoss
// precompute loss at time node d2 (d1 is the previous time node)
//===========================================================================
double ccruncher::Asset::precomputeLoss(Date d1, Date d2, Interest &spot)
{
  double ret = 0.0;
  double ufactor, tfactor, csum, recv;
  Date prevdate = d1;

  if (d2 < date) return 0.0;
  if (d1 < date) prevdate = date;
  int idx1 = getRightIdx(prevdate);
  if (idx1 == -1) return 0.0;
  int idx2 = getRightIdx(d2);
  if (idx2 == -1) idx2 = data.size()-1;

  for (int i=idx1; i<=idx2; i++)
  {
    if (data[i].date <= d2) 
    {
      ufactor = spot.getUpsilon(data[i].date, d2);
      tfactor = (double)(data[i].date-prevdate)/(double)(d2-d1);
      csum = getCashflowSum(data[i].date, spot);
      recv = data[i].recovery;
      ret += ufactor * (csum - recv) * tfactor;
    }
    else
    {
      tfactor = (double)(d2-prevdate)/(double)(d2-d1);
      csum = getCashflowSum(d2, spot);
      recv = data[i].recovery * spot.getUpsilon(data[i].date, d2);
      ret += (csum - recv) * tfactor;
    }
    prevdate = data[i].date;
  }

  return ret;
}

//===========================================================================
// precomputeLosses
//===========================================================================
void ccruncher::Asset::precomputeLosses(Date *dates, int m, Interests &interests)
{
  Interest &spot = interests["spot"];

  // allocating & initializing memory
  plosses = Arrays<double>::allocVector(m, 0.0);

  // sorting dates
  sort(dates, dates+m);

  // computing losses at given time nodes (array dates)
  for (int i=0;i<m;i++)
  {
    plosses[i] = precomputeLoss((i==0?Date(1,1,1):dates[i-1]), dates[i], spot);
  }
}

//===========================================================================
// getLoss
//===========================================================================
double ccruncher::Asset::getLoss(int k)
{
  if (plosses == NULL || k < 0)
  {
    return NAN;
  }
  else 
  {
    return plosses[k];
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Asset::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"asset")) {
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
  else if (isEqual(name_,"belongs-to")) {
    string ssegmentation = getStringAttribute(attributes, "segmentation", "");
    string ssegment = getStringAttribute(attributes, "segment", "");

    if (ssegmentation == "" || ssegment == "") {
      throw Exception("invalid attributes at <belongs-to> tag");
    }

    int isegmentation = (*segmentations)[ssegmentation].order;
    int isegment = (*segmentations)[ssegmentation][ssegment].order;

    insertBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(name_,"data")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag data");
    }
    else {
      have_data = true;
    }
  }
  else if (isEqual(name_,"values") && have_data == true) {
    Date at = getDateAttribute(attributes, "at", Date(1,1,1));
    double cashflow = getDoubleAttribute(attributes, "cashflow", NAN);
    double recovery = getDoubleAttribute(attributes, "recovery", NAN);

    if (at == Date(1,1,1) || isnan(cashflow) || isnan(recovery)) {
      throw Exception("invalid attributes at <values>");
    }
    else {
      DateValues aux(at, cashflow, recovery);
      insertDateValues(aux);
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
  if (isEqual(name_,"asset")) {

    // checking data size
    if (data.size() == 0) {
      throw Exception("asset without data");
    }
    else {
      try 
      { 
        // adding creation date as an event
        DateValues event0(date, 0.0, 0.0);
        insertDateValues(event0);
      }
      catch(...) 
      {
        // no problem, creation date exist
      }
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
    throw Exception("trying to reinsert a defined segmentation");
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
