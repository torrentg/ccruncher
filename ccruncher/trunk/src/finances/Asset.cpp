
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
// Asset.cpp - Asset code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#include <cmath>
#include <algorithm>
#include "Asset.hpp"
#include "Bond.hpp"

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::Asset::Asset()
{
  vevents = NULL; 
  nevents=0;
}

//***************************************************************************
// destructor
//***************************************************************************
ccruncher::Asset::~Asset()
{
  if (vevents != NULL) delete [] vevents; 
}

//***************************************************************************
// setId
//***************************************************************************
void ccruncher::Asset::setId(string id_)
{
  id = id_;
}

//***************************************************************************
// getId
//***************************************************************************
string ccruncher::Asset::getId(void)
{
  return id;
}

//***************************************************************************
// getVCashFlow between date1 and date2
//***************************************************************************
double ccruncher::Asset::getVCashFlow(Date date1, Date date2, DateValues *events, int n)
{
  double ret = 0.0;
  
  if (date2 < events[0].date)
  {
    return 0.0;
  }

  for(int i=0;i<n;i++)
  {
    if (date1 == date2 && date1 == events[i].date)
    {
      ret += events[i].cashflow;
      break;
    }
    if (date1 < events[i].date && events[i].date <= date2)
    {
      ret += events[i].cashflow;
    }
    if (date2 < events[i].date)
    {
      break;
    }
  }

  //TODO: pendent actualitzar els fluxes respecte el tipus de interes

  return ret;
}

//***************************************************************************
// getVExposure at date2
//***************************************************************************
double ccruncher::Asset::getVExposure(Date date1, Date date2, DateValues *events, int n)
{
  double ret = 0.0;

  if (date2 < events[0].date)
  {
    return 0.0;
  }

  if (date1 < events[n-1].date && events[n-1].date < date2)
  {
    double val1 = events[n-1].date - date1;
    double val2 = date2 - date1;
    
    return events[n-1].exposure * val1/val2;
  }

  for(int i=1;i<n;i++)
  {
    if (date2 <= events[i].date)
    {
      Date datex = events[i-1].date;
      double ex = events[i-1].exposure;
      Date datey = events[i].date;
      double ey = events[i].exposure;

      ret = ex + (date2-datex)*(ey - ex)/(datey - datex);
      
      return ret;
    }
  }

  //TODO: pendent actualitzar els fluxes respecte el tipus de interes

  throw Exception("Asset::interpole(): panic!!!");
}

//***************************************************************************
// getVertexes
//***************************************************************************
void ccruncher::Asset::getVertexes(Date *dates, int n, DateValues *ret)
{
  sort(dates, dates+n);

  for (int i=0;i<n;i++)
  {
    ret[i].date = dates[i];
    ret[i].cashflow = getVCashFlow(dates[max(i-1,0)], dates[i], vevents, nevents);
    ret[i].exposure = getVExposure(dates[max(i-1,0)], dates[i], vevents, nevents);
  }
}

//***************************************************************************
// getInstanceByClassName
//***************************************************************************
Asset* ccruncher::Asset::getInstanceByClassName(string classname) throw(Exception)
{
  Asset *ret = NULL;
  
  if (classname == "bond") 
  {
    ret = new Bond();
  }
  else 
  {
    throw Exception("Asset::getInstanceByClassName(): " + classname + " not registered");
  }  

  // return object
  return ret;
}

//***************************************************************************
// interpreta un node XML de tipus asset
//***************************************************************************
Asset* ccruncher::Asset::parseDOMNode(const DOMNode& node, Segmentations *segs, Interests *ints) throw(Exception)
{
  Asset *ret = NULL;

  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "asset"))
  {
    string msg = "Asset::parseDOMNode(): Invalid tag. Expected: asset. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();
  string strid = XMLUtils::getStringAttribute(attributes, "id", "");  
  string strclass = XMLUtils::getStringAttribute(attributes, "class", "");

  if (strid == "" || strclass == "")
  {
    throw Exception("Asset::parseDOMNode(): invalid attributes at <asset>");
  }

  try
  {  
    // creating instance by name
    ret = Asset::getInstanceByClassName(strclass);

    // setting asset id and interests
    ret->setId(strid);
    ret->setInterests(ints);

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
          
          ret->insertBelongsTo(iconcept, isegment);
        }
        else if (XMLUtils::isNodeName(child, "property"))
        {
          DOMNamedNodeMap &tmpnodemap = *child.getAttributes();
          string name = XMLUtils::getStringAttribute(tmpnodemap, "name", "");  
          string value = XMLUtils::getStringAttribute(tmpnodemap, "value", "");

          ret->setProperty(name, value);
        }
        else
        {
          throw Exception("Asset::parseDOMNode(): invalid data structure at <asset>");
        }
      }
    }

    if (ret->validate() == false)
    {
      throw Exception("Asset::parseDOMNode(): asset with conflictive properties");
    }
    else
    {
      // computing events
      ret->initialize();

      // filling implicit segment
      if (segs->getSegmentation("asset") >= 0)
      {
        if (segs->getComponents("asset") == COMPONENTS_ASSET)
        {
          segs->addSegment("asset", strid);
          int iconcept = segs->getSegmentation("asset");
          int isegment = segs->getSegment("asset", strid);
          ret->insertBelongsTo(iconcept, isegment);
        }
      }

      // filling implicit segment
      if (segs->getSegmentation("portfolio") >= 0)
      {
        if (segs->getComponents("portfolio") == COMPONENTS_ASSET)
        {
          int iconcept = segs->getSegmentation("portfolio");
          int isegment = segs->getSegment("portfolio", "rest");
          ret->insertBelongsTo(iconcept, isegment);
        }
      }
    }

    return ret;
  }
  catch(Exception &e)
  {
    if (ret != NULL) delete ret;
    throw e;
  }
}

//***************************************************************************
// addBelongsTo
//***************************************************************************
void ccruncher::Asset::addBelongsTo(int iconcept, int isegment) throw(Exception)
{
  insertBelongsTo(iconcept, isegment);
}

//***************************************************************************
// insertBelongsTo
//***************************************************************************
void ccruncher::Asset::insertBelongsTo(int iconcept, int isegment) throw(Exception)
{
  if (getSegment(iconcept) >= 0)
  {
    throw Exception("Asset::insertBelongsTo(): trying to reinsert a concept defined");
  }
  else if (iconcept >= 0 || isegment >= 0)  
  {
    belongsto[iconcept] = isegment;
  }
  else
  {
    throw Exception("Asset::insertBelongsTo(): concept or segment not exist");
  }
}

//***************************************************************************
// belongsTo
//***************************************************************************
bool ccruncher::Asset::belongsTo(int iconcept, int isegment)
{
  return ((getSegment(iconcept)==isegment)?true:false);
}

//***************************************************************************
// getSegment
//***************************************************************************
int ccruncher::Asset::getSegment(int iconcept)
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
// initialize
//***************************************************************************
void ccruncher::Asset::initialize() throw(Exception)
{
  if (vevents != NULL) delete [] vevents;

  vevents = getIEvents();
  nevents = getISize();
}

//***************************************************************************
// compute
//***************************************************************************
void ccruncher::Asset::setInterests(Interests *ints)
{
  interests = ints;
}

//***************************************************************************
// getSize
//***************************************************************************
int ccruncher::Asset::getSize()
{
  return nevents;
}

//***************************************************************************
// getEvents
//***************************************************************************
DateValues* ccruncher::Asset::getEvents()
{
  return vevents;
}
