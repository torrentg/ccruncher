
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
// Borrower.cpp - Borrower code - $Rev$
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
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Sectors refactoring
//   . Ratings refactoring
//   . Segmentations refactoring
//   . Asset refactoring
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Client refactoring
//
// 2006/02/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed method ExpatHandlers::eperror()
//
// 2007/07/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed rating.order tag
//   . removed sector.order tag
//   . added unique asset id check
//
// 2007/08/03 - Gerard Torrent [gerard@mail.generacio.com]
//   . Client class renamed to Borrower
//
//===========================================================================

#include "portfolio/Borrower.hpp"
#include "utils/Utils.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Borrower::Borrower(Ratings &ratings_, Sectors &sectors_,
               Segmentations &segmentations_, Interests &interests_)
{
  // initializing class
  reset(ratings_, sectors_, segmentations_, interests_);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Borrower::~Borrower()
{
  // nothing to do
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Borrower::reset(Ratings &ratings_, Sectors &sectors_,
               Segmentations &segmentations_, Interests &interests_)
{
  // setting external objects references
  ratings = &(ratings_);
  sectors = &(sectors_);
  segmentations = &(segmentations_);
  interests = &(interests_);
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
vector<Asset> & ccruncher::Borrower::getAssets()
{
  return vassets;
}

//===========================================================================
// insert an asset into list
//===========================================================================
void ccruncher::Borrower::insertAsset(const Asset &val) throw(Exception)
{
  // checking coherence
  for (unsigned int i=0;i<vassets.size();i++)
  {
    Asset aux = vassets[i];

    if (aux.getId() == val.getId())
    {
      throw Exception("asset identifier " + val.getId() + " repeated");
    }
  }

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
void ccruncher::Borrower::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"borrower")) {
    if (getNumAttributes(attributes) != 4) {
      throw Exception("incorrect number of attributes in tag borrower");
    }
    else {
      // reading atributes
      id = getStringAttribute(attributes, "id", "");
      name = getStringAttribute(attributes, "name", "");
      string strrating = getStringAttribute(attributes, "rating", "");
      string strsector= getStringAttribute(attributes, "sector", "");

      // retrieving indexes
      irating = (*ratings).getIndex(strrating);
      isector = (*sectors).getIndex(strsector);

      // doing some checks
      if (id == "" || name == "" || irating < 0 || isector < 0) {
        throw Exception("invalid attributes at <borrower>");
      }

      // computing hash key
      hkey = Utils::hash(id);
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
  else if (isEqual(name_,"asset")) {
    auxasset.reset(segmentations);
    eppush(eu, &auxasset, name_, attributes);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Borrower::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"borrower")) {

    // reseting auxiliar variables (flushing data)
    auxasset.reset(NULL);

    // filling implicit segment
    try
    {
      if ((*segmentations)["borrower"].components == borrower) {
        segmentations->addSegment("borrower", id);
        int isegmentation = (*segmentations)["borrower"].order;
        int isegment = (*segmentations)["borrower"][id].order;
        insertBelongsTo(isegmentation, isegment);
      }
    }
    catch(...)
    {
      // segmentation 'borrower' not found
    }

    // filling implicit segment
    try
    {
      if ((*segmentations)["portfolio"].components == borrower) {
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
  else if (isEqual(name_,"asset")) {
    insertAsset(auxasset);
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// isActive
//===========================================================================
bool ccruncher::Borrower::isActive(const Date &from, const Date &to) throw(Exception)
{
  if (vassets.size() == 0)
  {
    return false;
  }

  for(unsigned int i=0;i<vassets.size();i++)
  {
    vector<DateValues> &data = vassets[i].getData();

    if (data.size() > 0)
    {
      Date date1 = data[0].date;
      Date date2 = data[data.size()-1].date;

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
// used to group borrowers by sector and rating
//===========================================================================
bool ccruncher::operator < (const Borrower &x1, const Borrower &x2)
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
void ccruncher::Borrower::addBelongsTo(int isegmentation, int isegment) throw(Exception)
{
  insertBelongsTo(isegmentation, isegment);
}

//===========================================================================
// insertBelongsTo
//===========================================================================
void ccruncher::Borrower::insertBelongsTo(int isegmentation, int isegment) throw(Exception)
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
bool ccruncher::Borrower::belongsTo(int isegmentation, int isegment)
{
  return ((getSegment(isegmentation)==isegment)?true:false);
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Borrower::getSegment(int isegmentation)
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
// less (used for sorting pointer borrowers)
//===========================================================================
bool ccruncher::Borrower::less(const Borrower *left, const Borrower *right)
{
  return *left < *right;
}
