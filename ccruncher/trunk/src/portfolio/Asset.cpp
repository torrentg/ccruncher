
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
//===========================================================================

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Asset.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Asset::Asset(const DOMNode &node, Segmentations *segs) throw(Exception)
{
  // parsing DOM node
  parseDOMNode(node, segs);
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
double ccruncher::Asset::getVCashFlow(Date date1, Date date2, Interests *ints)
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
      ret += data[i].cashflow;
      break;
    }
    if (date1 < data[i].date && data[i].date <= date2)
    {
      ret += data[i].cashflow;
    }
    if (date2 < data[i].date)
    {
      break;
    }
  }

  //TODO: pendent actualitzar els fluxes respecte el tipus de interes

  return ret;
}

//===========================================================================
// getVExposure at date2
//===========================================================================
double ccruncher::Asset::getVExposure(Date date1, Date date2, Interests *ints)
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

    return data[n-1].exposure * val1/val2;
  }

  for(int i=1;i<n;i++)
  {
    if (date2 <= data[i].date)
    {
      Date datex = data[i-1].date;
      double ex = data[i-1].exposure;
      Date datey = data[i].date;
      double ey = data[i].exposure;

      ret = ex + (date2-datex)*(ey - ex)/(datey - datex);

      return ret;
    }
  }
  
  // assertion
  assert(true);

  //TODO: pendent actualitzar els fluxes respecte el tipus de interes

  throw Exception("Asset::getVExposure(): panic!!!");
}

//===========================================================================
// getVRecovery at date2
//===========================================================================
double ccruncher::Asset::getVRecovery(Date date1, Date date2, Interests *ints)
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

    return data[n-1].recovery * val1/val2;
  }

  for(int i=1;i<n;i++)
  {
    if (date2 <= data[i].date)
    {
      Date datex = data[i-1].date;
      double ex = data[i-1].recovery;
      Date datey = data[i].date;
      double ey = data[i].recovery;

      ret = ex + (date2-datex)*(ey - ex)/(datey - datex);

      return ret;
    }
  }

  // assertion
  assert(true);

  //TODO: pendent actualitzar els fluxes respecte el tipus de interes

  throw Exception("Asset::getVRecovery(): panic!!!");
}

//===========================================================================
// getVertexes
//===========================================================================
void ccruncher::Asset::getVertexes(Date *dates, int n, Interests *ints, DateValues *ret)
{
  sort(dates, dates+n);

  for (int i=0;i<n;i++)
  {
    ret[i].date = dates[i];
    ret[i].cashflow = getVCashFlow(dates[max(i-1,0)], dates[i], ints);
    ret[i].exposure = getVExposure(dates[max(i-1,0)], dates[i], ints);
    ret[i].recovery = getVRecovery(dates[max(i-1,0)], dates[i], ints);
  }
}

//===========================================================================
// interpreta un node XML de tipus asset
//===========================================================================
void ccruncher::Asset::parseDOMNode(const DOMNode& node, Segmentations *segs) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "asset"))
  {
    string msg = "Asset::parseDOMNode(): Invalid tag. Expected: asset. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();
  id = XMLUtils::getStringAttribute(attributes, "id", "");
  name = XMLUtils::getStringAttribute(attributes, "name", "");

  if (id == "" || name == "")
  {
    throw Exception("Asset::parseDOMNode(): invalid attributes at <asset>");
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
      else if (XMLUtils::isNodeName(child, "data"))
      {
        parseData(child);
      }
      else
      {
        throw Exception("Asset::parseDOMNode(): invalid data structure at <asset>");
      }
    }
  }

  // error if data tag not exist
  if (data.size() == 0)
  {
    throw Exception("Asset::parseDOMNode(): asset without data");
  }
  else
  {
    // sorting data by date
    sort(data.begin(), data.end());
  }

  // filling implicit segment
  if (segs->getSegmentation("asset") >= 0)
  {
    if (segs->getComponents("asset") == asset)
    {
      segs->addSegment("asset", id);
      int iconcept = segs->getSegmentation("asset");
      int isegment = segs->getSegment("asset", id);
      insertBelongsTo(iconcept, isegment);
    }
  }

  // filling implicit segment
  if (segs->getSegmentation("portfolio") >= 0)
  {
    if (segs->getComponents("portfolio") == asset)
    {
      int iconcept = segs->getSegmentation("portfolio");
      int isegment = segs->getSegment("portfolio", "rest");
      insertBelongsTo(iconcept, isegment);
    }
  }
}

//===========================================================================
// interpreta un node XML de tipus data
//===========================================================================
void ccruncher::Asset::parseData(const DOMNode& node) throw(Exception)
{
  assert(XMLUtils::isNodeName(node, "data"));

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
      else if (XMLUtils::isNodeName(child, "values"))
      {
        DateValues aux = DateValues(child);
        insertDateValues(aux);
      }
      else
      {
        throw Exception("Asset::parseData(): invalid data structure at <data>");
      }
    }
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
    return -1;
  }
}

//===========================================================================
// getData
//===========================================================================
vector<DateValues>* ccruncher::Asset::getData()
{
  return &data;
}
