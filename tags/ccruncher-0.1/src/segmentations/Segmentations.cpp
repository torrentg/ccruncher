
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
// Segmentations.cpp - Segmentations code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "Segmentations.hpp"
#include "utils/Utils.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segmentations::Segmentations(const DOMNode& node) throw(Exception)
{
  // recollim els parametres de la simulacio
  parseDOMNode(node);

  // making validations
  validate();
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Segmentations::~Segmentations()
{
  // nothing to do
}

//===========================================================================
// return segmentations list
//===========================================================================
vector<Segmentation> ccruncher::Segmentations::getSegmentations()
{
  return vsegmentations;
}

//===========================================================================
// return segmentation by name
//===========================================================================
int ccruncher::Segmentations::getSegmentation(string name)
{
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == name)
    {
      return i;
    }
  }

  return -1;
}

//===========================================================================
// return components of a segmentation by name
//===========================================================================
components_t ccruncher::Segmentations::getComponents(string name)
{
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == name)
    {
      return vsegmentations[i].components;
    }
  }

  return undefined;
}

//===========================================================================
// return components of a segmentation by name
//===========================================================================
components_t ccruncher::Segmentations::getComponents(int iseg)
{
  if (iseg < 0 || (unsigned int) iseg >= vsegmentations.size())
  {
    return undefined;
  }
  else
  {
    return vsegmentations[iseg].components;
  }
}

//===========================================================================
// return segment by name
//===========================================================================
int ccruncher::Segmentations::getSegment(string segmentation, string segment)
{
  int iconcept = getSegmentation(segmentation);

  if (iconcept >= 0)
  {
    return vsegmentations[iconcept].getSegment(segment);
  }
  else
  {
    return -1;
  }
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::Segmentations::validate() throw(Exception)
{
  if (vsegmentations.size() == 0)
  {
    throw Exception("Segmentations::validate(): no segmentations defined");
  }
}

//===========================================================================
// insert a new segmentation in list
//===========================================================================
void ccruncher::Segmentations::insertSegmentation(Segmentation &val) throw(Exception)
{
  // validem coherencia
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == val.name)
    {
      string msg = "Segmentations::insertSegmentation(): segmentation name ";
      msg += val.name;
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vsegmentations.push_back(val);
  }
  catch(std::exception &e)
  {
     throw Exception(e);
  }
}

//===========================================================================
// interpreta un node XML
//===========================================================================
void ccruncher::Segmentations::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "segmentations"))
  {
    string msg = "Segmentations::parseDOMNode(): Invalid tag. Expected: segmentations. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
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
      else if (XMLUtils::isNodeName(child, "segmentation"))
      {
        Segmentation aux = Segmentation(child);
        insertSegmentation(aux);
      }
      else
      {
        throw Exception("Segmentations::parseDOMNode(): invalid data structure at <segmentations>");
      }
    }
  }
}

//===========================================================================
// addSegment
//===========================================================================
void ccruncher::Segmentations::addSegment(string segmentation, string segment) throw(Exception)
{
  int isegmentation = getSegmentation(segmentation);

  if (isegmentation < 0)
  {
    throw Exception("Segmentations::addSegment(): segmentation " + segmentation + " not found");
  }
  else
  {
    vsegmentations[isegmentation].addSegment(segment);
  }
}

//===========================================================================
// getSegmentationName
//===========================================================================
string ccruncher::Segmentations::getSegmentationName(int isegmentation) throw(Exception)
{
  if (isegmentation < 0 || isegmentation >= (int) vsegmentations.size())
  {
    throw Exception("Segmentations::getSegmentationName(): index out of range");
  }
  else
  {
    return vsegmentations[isegmentation].name;
  }
}

//===========================================================================
// getSegmentName
//===========================================================================
string ccruncher::Segmentations::getSegmentName(int isegmentation, int isegment) throw(Exception)
{
  if (isegmentation < 0 || isegmentation >= (int) vsegmentations.size())
  {
    throw Exception("Segmentations::getSegmentationName(): index out of range");
  }
  else
  {
    return vsegmentations[isegmentation].getSegmentName(isegment);
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Segmentations::getXML(int ilevel) throw(Exception)
{
  string spc = Utils::blanks(ilevel);
  string ret = "";

  ret += spc + "<segmentations>\n";

  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    ret += vsegmentations[i].getXML(ilevel+2);
  }

  ret += spc + "</segmentations>\n";

  return ret;
}