
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
// Client.cpp - Client code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . asset refactoring
//
// 2005/04/03 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/22 - Gerard Torrent [gerard@fobos.generacio.com]
//   . solved bug related to default segment (rest segment = default)
//
// 2005/07/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added hash key (to accelerate comparations)
//
// 2005/07/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . moved <cassert> include at last position
//
// 2005/08/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . tag concept renamed to segmentation
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "portfolio/Client.hpp"
#include "utils/Utils.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Client::Client(Ratings *ratings_, Sectors *sectors_,
               Segmentations *segmentations_, Interests *interests_)
{
  // initializing class
  reset(ratings_, sectors_, segmentations_, interests_);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Client::~Client()
{
  // nothing to do
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Client::reset(Ratings *ratings_, Sectors *sectors_,
               Segmentations *segmentations_, Interests *interests_)
{
  // setting external objects references
  ratings = ratings_;
  sectors = sectors_;
  segmentations = segmentations_;
  interests = interests_;
  hkey = 0UL;

  // cleaning containers
  vassets.clear();
  belongsto.clear();

  // setting default values
  irating = -1;
  isector = -1;
  id = "NON_ID";
  name = "NO_NAME";
}

//===========================================================================
// getAssets
//===========================================================================
vector<Asset> * ccruncher::Client::getAssets()
{
  return &vassets;
}

//===========================================================================
// insert an asset into list
//===========================================================================
void ccruncher::Client::insertAsset(Asset &val) throw(Exception)
{
  try
  {
    vassets.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Client::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"client")) {
    if (getNumAttributes(attributes) != 4) {
      throw eperror(eu, "incorrect number of attributes in tag client");
    }
    else {
      // reading atributes
      id = getStringAttribute(attributes, "id", "");
      name = getStringAttribute(attributes, "name", "");
      string strrating = getStringAttribute(attributes, "rating", "");
      string strsector= getStringAttribute(attributes, "sector", "");

      // retrieving indexes
      irating = ratings->getIndex(strrating);
      isector = sectors->getIndex(strsector);

      // doing some checks
      if (id == "" || name == "" || irating < 0 || isector < 0) {
        throw eperror(eu, "invalid attributes at <client>");
      }

      // computing hash key
      hkey = Utils::hash(id);
    }
  }
  else if (isEqual(name_,"belongs-to")) {
    string ssegmentation = getStringAttribute(attributes, "segmentation", "");
    string ssegment = getStringAttribute(attributes, "segment", "");

    if (ssegmentation == "" || ssegment == "") {
      throw eperror(eu, "invalid attributes at <belongs-to> tag");
    }

    int isegmentation = segmentations->getSegmentation(ssegmentation);
    int isegment = segmentations->getSegment(ssegmentation, ssegment);

    insertBelongsTo(isegmentation, isegment);
  }
  else if (isEqual(name_,"asset")) {
    auxasset.reset(segmentations);
    eppush(eu, &auxasset, name_, attributes);
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Client::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"client")) {
    // reseting auxiliar variables (flushing data)
    auxasset.reset(NULL);
    // filling implicit segment
    if (segmentations->getSegmentation("client") >= 0) {
      if (segmentations->getComponents("client") == client) {
        segmentations->addSegment("client", id);
        int isegmentation = segmentations->getSegmentation("client");
        int isegment = segmentations->getSegment("client", id);
        insertBelongsTo(isegmentation, isegment);
      }
    }
    // filling implicit segment
    if (segmentations->getSegmentation("portfolio") >= 0) {
      if (segmentations->getComponents("portfolio") == client) {
        int isegmentation = segmentations->getSegmentation("portfolio");
        int isegment = segmentations->getSegment("portfolio", "rest");
        insertBelongsTo(isegmentation, isegment);
      }
    }
  }
  else if (isEqual(name_,"belongs-to")) {
    // nothing to do
  }
  else if (isEqual(name_,"asset")) {
    insertAsset(auxasset);
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// isActive
//===========================================================================
bool ccruncher::Client::isActive(Date from, Date to) throw(Exception)
{
  if (vassets.size() == 0)
  {
    return false;
  }

  for(unsigned int i=0;i<vassets.size();i++)
  {
    vector<DateValues> *data = vassets[i].getData();

    if ((*data).size() > 0)
    {
      Date date1 = (*data)[0].date;
      Date date2 = (*data)[(*data).size()-1].date;

      if (from <= date1 && date1 <= to)
      {
        return true;
      }
      else if (from <= date2 && date2 <= to)
      {
        return true;
      }
      else if (date1 <= from && to <= date2)
      {
        return true;
      }
    }
  }

  return false;
}

//===========================================================================
// comparation operator (used by sort function)
// used to group clients by sector and rating
//===========================================================================
bool ccruncher::operator < (const Client &x1, const Client &x2)
{
  if (x1.isector < x2.isector)
  {
    return true;
  }
  else if (x1.isector == x2.isector)
  {
    if (x1.irating < x2.irating)
    {
      return true;
    }
    else if (x1.irating == x2.irating)
    {
      return false;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

//===========================================================================
// addBelongsTo
//===========================================================================
void ccruncher::Client::addBelongsTo(int isegmentation, int isegment) throw(Exception)
{
  insertBelongsTo(isegmentation, isegment);
}

//===========================================================================
// insertBelongsTo
//===========================================================================
void ccruncher::Client::insertBelongsTo(int isegmentation, int isegment) throw(Exception)
{
  assert(isegmentation >= 0);
  assert(isegment >= 0);

  if (getSegment(isegmentation) > 0)
  {
    throw Exception("Client::insertBelongsTo(): trying to reinsert a defined segmentation");
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
bool ccruncher::Client::belongsTo(int isegmentation, int isegment)
{
  return ((getSegment(isegmentation)==isegment)?true:false);
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Client::getSegment(int isegmentation)
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
// less (used for sorting pointer clients)
//===========================================================================
bool ccruncher::Client::less(const Client *left, const Client *right)
{
  return *left < *right;
}
