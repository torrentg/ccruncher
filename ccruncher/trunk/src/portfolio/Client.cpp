
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
//===========================================================================

#include <cmath>
#include <algorithm>
#include <cassert>
#include "Client.hpp"
#include "utils/XMLUtils.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-12

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
// constructor
// TODO: this method will be removed
//===========================================================================
ccruncher::Client::Client(Ratings *ratings_, Sectors *sectors_, Segmentations *segmentations_,
               Interests *interests_, const DOMNode& node) throw(Exception)
{
  // initializing class
  reset(ratings_, sectors_, segmentations_, interests_);

  // recollim la informacio del node
  parseDOMNode(ratings, sectors, segmentations, interests, node);
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
// insercio nova operacio en la llista
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
      id = getStringAttribute(attributes, "id", "");
      name = getStringAttribute(attributes, "name", "");
      string strrating = getStringAttribute(attributes, "rating", "");
      string strsector= getStringAttribute(attributes, "sector", "");

      irating = ratings->getIndex(strrating);
      isector = sectors->getIndex(strsector);

      if (id == "" || name == "" || irating < 0 || isector < 0) {
        throw eperror(eu, "invalid attributes at <client>");
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
        int iconcept = segmentations->getSegmentation("client");
        int isegment = segmentations->getSegment("client", id);
        insertBelongsTo(iconcept, isegment);
      }
    }
    // filling implicit segment
    if (segmentations->getSegmentation("portfolio") >= 0) {
      if (segmentations->getComponents("portfolio") == client) {
        int iconcept = segmentations->getSegmentation("portfolio");
        int isegment = segmentations->getSegment("portfolio", "rest");
        insertBelongsTo(iconcept, isegment);
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
// interpreta un node XML params
// TODO: this method will be removed
//===========================================================================
void ccruncher::Client::parseDOMNode(Ratings *ratings_, Sectors *sectors_, Segmentations *segs_,
                          Interests *interests_, const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "client"))
  {
    string msg = "Client::parseDOMNode(): Invalid tag. Expected: client. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();
  id = XMLUtils::getStringAttribute(attributes, "id", "");
  name = XMLUtils::getStringAttribute(attributes, "name", "");
  string strrating = XMLUtils::getStringAttribute(attributes, "rating", "");
  string strsector= XMLUtils::getStringAttribute(attributes, "sector", "");

  irating = ratings->getIndex(strrating);
  isector = sectors->getIndex(strsector);

  if (id == "" || name == "" || irating < 0 || isector < 0)
  {
    throw Exception("Client::parseDOMNode(): invalid attributes at <client>");
  }

  // recorrem tots els items
  DOMNodeList &children = *node.getChildNodes();

  if (&children != NULL)
  {
    for(unsigned int i=0;i<children.getLength();i++)
    {
      DOMNode &child = *children.item(i);

      if (XMLUtils::isVoidTextNode(child) || XMLUtils::isCommentNode(child))
      {
        continue;
      }
      else if (XMLUtils::isNodeName(child, "belongs-to"))
      {
        DOMNamedNodeMap &tmpnodemap = *child.getAttributes();
        string sconcept = XMLUtils::getStringAttribute(tmpnodemap, "concept", "");
        string ssegment = XMLUtils::getStringAttribute(tmpnodemap, "segment", "");

        int iconcept = segmentations->getSegmentation(sconcept);
        int isegment = segmentations->getSegment(sconcept, ssegment);

        insertBelongsTo(iconcept, isegment);
      }
      else if (XMLUtils::isNodeName(child, "asset"))
      {
        Asset aux = Asset(child, segmentations);
        insertAsset(aux);
      }
      else
      {
        throw Exception("Client::parseDOMNode(): invalid data structure at <client>");
      }
    }
  }

  // completem segmentacio client si existeix
  if (segmentations->getSegmentation("client") >= 0)
  {
    if (segmentations->getComponents("client") == client)
    {
      segmentations->addSegment("client", id);
      int iconcept = segmentations->getSegmentation("client");
      int isegment = segmentations->getSegment("client", id);
      insertBelongsTo(iconcept, isegment);
    }
  }

  // completem segmentacio portfolio si existeix
  if (segmentations->getSegmentation("portfolio") >= 0)
  {
    if (segmentations->getComponents("portfolio") == client)
    {
      int iconcept = segmentations->getSegmentation("portfolio");
      int isegment = segmentations->getSegment("portfolio", "rest");
      insertBelongsTo(iconcept, isegment);
    }
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
// group clients by sector and rating, always non active clients at last
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
void ccruncher::Client::addBelongsTo(int iconcept, int isegment) throw(Exception)
{
  insertBelongsTo(iconcept, isegment);
}

//===========================================================================
// insertBelongsTo
//===========================================================================
void ccruncher::Client::insertBelongsTo(int iconcept, int isegment) throw(Exception)
{
  if (getSegment(iconcept) >= 0)
  {
    throw Exception("Client::insertBelongsTo(): trying to reinsert a concept defined");
  }
  else if (iconcept >= 0 || isegment >= 0)
  {
    belongsto[iconcept] = isegment;
  }
  else
  {
    throw Exception("Client::insertBelongsTo(): concept or segment not exist");
  }
}

//===========================================================================
// belongsTo
//===========================================================================
bool ccruncher::Client::belongsTo(int iconcept, int isegment)
{
  return ((getSegment(iconcept)==isegment)?true:false);
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Client::getSegment(int iconcept)
{
  map<int,int>::iterator pos = belongsto.find(iconcept);

  if (pos != belongsto.end())
  {
    return pos->second;
  }
  else
  {
    return -1;
  }
}

//===========================================================================
// less (used for sorting pointer clients)
//===========================================================================
bool ccruncher::Client::less(const Client *left, const Client *right)
{
  return *left < *right;
}
