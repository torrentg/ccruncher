
//***************************************************************************
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
//***************************************************************************

#include <cmath>
#include <algorithm>
#include "Client.hpp"
#include "utils/XMLUtils.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-12

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::Client::Client(Ratings *ratings, Sectors *sectors, Segmentations *segmentations, 
               Interests *interests, const DOMNode& node) throw(Exception)
{
  // inicialitzem el vector de rates
  vassets = vector<Asset*>();

  // recollim la informacio del node
  parseDOMNode(ratings, sectors, segmentations, interests, node);
}

//***************************************************************************
// destructor
//***************************************************************************
ccruncher::Client::~Client()
{
  // dropping assets
  for(unsigned int i=0;i<vassets.size();i++)
  {
    delete vassets[i];
  }
}

//***************************************************************************
// getAssets
//***************************************************************************
vector<Asset*> * ccruncher::Client::getAssets() 
{
  return &vassets;
}

//***************************************************************************
// insercio nova operacio en la llista
//***************************************************************************
void ccruncher::Client::insertAsset(Asset *val) throw(Exception) 
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

//***************************************************************************
// interpreta un node XML params
//***************************************************************************
void ccruncher::Client::parseDOMNode(Ratings *ratings, Sectors *sectors, Segmentations *segs, 
                          Interests *interests, const DOMNode& node) throw(Exception)
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

        int iconcept = segs->getSegmentation(sconcept);
        int isegment = segs->getSegment(sconcept, ssegment);
          
        insertBelongsTo(iconcept, isegment);
      }
      else if (XMLUtils::isNodeName(child, "asset"))
      {
        Asset *aux = Asset::parseDOMNode(child, segs, interests);
        insertAsset(aux);
      }
      else
      {
        throw Exception("Client::parseDOMNode(): invalid data structure at <client>");
      }
    }
  }

  // completem segmentacio client si existeix  
  if (segs->getSegmentation("client") >= 0)
  {
    if (segs->getComponents("client") == COMPONENTS_CLIENT)
    {
      segs->addSegment("client", id);
      int iconcept = segs->getSegmentation("client");
      int isegment = segs->getSegment("client", id);
      insertBelongsTo(iconcept, isegment);
    }
  }

  // completem segmentacio portfolio si existeix  
  if (segs->getSegmentation("portfolio") >= 0)
  {
    if (segs->getComponents("portfolio") == COMPONENTS_CLIENT)
    {
      int iconcept = segs->getSegmentation("portfolio");
      int isegment = segs->getSegment("portfolio", "rest");
      insertBelongsTo(iconcept, isegment);
    }
  }
}

//***************************************************************************
// isActive
//***************************************************************************
bool ccruncher::Client::isActive(Date from, Date to) throw(Exception)
{
  if (vassets.size() == 0)
  {
    return false;
  }

  for(unsigned int i=0;i<vassets.size();i++)
  {
    DateValues *events = vassets[i]->getEvents();

    if (vassets[i]->getSize() > 0)
    {
      Date date1 = events[0].date;
      Date date2 = events[vassets[i]->getSize()-1].date;
      
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

//***************************************************************************
// comparation operator (used by sort function)
// group clients by sector, order by rating, always non active clients at last
//***************************************************************************
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

//***************************************************************************
// addBelongsTo
//***************************************************************************
void ccruncher::Client::addBelongsTo(int iconcept, int isegment) throw(Exception)
{
  insertBelongsTo(iconcept, isegment);
}

//***************************************************************************
// insertBelongsTo
//***************************************************************************
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

//***************************************************************************
// belongsTo
//***************************************************************************
bool ccruncher::Client::belongsTo(int iconcept, int isegment)
{
  return ((getSegment(iconcept)==isegment)?true:false);
}

//***************************************************************************
// getSegment
//***************************************************************************
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

//***************************************************************************
// less (used for sorting pointer clients)
//***************************************************************************
bool ccruncher::Client::less(const Client *left, const Client *right)
{
  return *left < *right;
}
