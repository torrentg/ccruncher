
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
// Segmentation.cpp - Segmentation code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "Segmentation.hpp"
#include "utils/XMLUtils.hpp"
#include "utils/Parser.hpp"
#include "utils/Utils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segmentation::Segmentation()
{
  // default values
  reset();
}

//===========================================================================
// constructor
// TODO: this method will be removed
//===========================================================================
ccruncher::Segmentation::Segmentation(const DOMNode& node) throw(Exception)
{
  // default values
  reset();

  // recollim els parametres de la simulacio
  parseDOMNode(node);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Segmentation::~Segmentation()
{
  // cal assegurar que es destrueix vsegments
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Segmentation::reset()
{
  vsegments.clear();
  modificable = false;
  name = "";
  components = client;
  
  // adding catcher segment
  Segment catcher = Segment("rest");
  insertSegment(catcher);
}

//===========================================================================
// insertSegment
//===========================================================================
void ccruncher::Segmentation::insertSegment(Segment &val) throw(Exception)
{
  if (val.name == "")
  {
    throw Exception("Segmentation::insertSegment(): invalid name value");
  }

  // validem coherencia
  for (unsigned int i=0;i<vsegments.size();i++)
  {
    if (vsegments[i].name == val.name)
    {
      string msg = "Segmentation::insertSegment(): segment ";
      msg += vsegments[i].name;
      msg += " repeated";
      throw Exception(msg);
    }
  }

  // checking for patterns
  if (val.name == "*")
  {
    if (name != "client" && name != "asset")
    {
      throw Exception("Segmentation::insertSegment(): invalid segment name '*'");
    }
    else
    {
      modificable = true;
      return;
    }
  }

  // inserim el valor
  try
  {
    vsegments.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentation::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"segmentation")) {
    if (getNumAttributes(attributes) != 2) {
      throw eperror(eu, "incorrect number of attributes in tag segmentation");
    }
    else {
      name = getStringAttribute(attributes, "name", "");
      string strcomp = getStringAttribute(attributes, "components", "");
      
      // checking name
      if (name == "") {
        throw eperror(eu, "tag <segmentation> with invalid name attribute");
      }
 
      // filling components variable
      if (strcomp == "asset") {
        components = asset;
      }
      else if (strcomp == "client") {
        components = client;
      }
      else {
        throw eperror(eu, "tag <segmentation> with invalid components attribute");
      }
    }
  }
  else if (isEqual(name_,"segment")) {
    string sname = getStringAttribute(attributes, "name", "");
    // checking segment name
    if (sname == "") {
      throw eperror(eu, "tag <segment> with invalid name attribute");
    }
    else {
      Segment aux(sname);
      insertSegment(aux);
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentation::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"segmentation")) {
    // nothing to do
  }
  else if (isEqual(name_,"segment")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// interpreta un node XML params
// TODO: this method will be removed
//===========================================================================
void ccruncher::Segmentation::parseDOMNode(const DOMNode& node) throw(Exception)
{
  string strcomp;

  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "segmentation"))
  {
    string msg = "Segmentation::parseDOMNode(): Invalid tag. Expected: segmentation. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem la llista d'atributs
  DOMNamedNodeMap &attributes = *node.getAttributes();
  name = XMLUtils::getStringAttribute(attributes, "name", "");
  strcomp = XMLUtils::getStringAttribute(attributes, "components", "");
  if (name == "")
  {
    throw Exception("Segmentation::parseDOMNode(): tag <segmentation> with invalid name attribute");
  }

  // filling components variable
  if (strcomp == "asset")
  {
    components = asset;
  }
  else if (strcomp == "client")
  {
    components = client;
  }
  else
  {
    throw Exception("Segmentation::parseDOMNode(): tag <segmentation> with invalid components attribute");
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
      else if (XMLUtils::isNodeName(child, "segment"))
      {
        Segment aux = Segment(child);
        insertSegment(aux);
      }
      else
      {
        throw Exception("Segmentation::parseDOMNode(): invalid data structure at <segmentation>");
      }
    }
  }
}

//===========================================================================
// getSegment
//===========================================================================
int ccruncher::Segmentation::getSegment(string segname) const
{
  for (unsigned int i=0;i<vsegments.size();i++)
  {
    if (vsegments[i].name == segname)
    {
      return i;
    }
  }

  return -1;
}

//===========================================================================
// getSegments
//===========================================================================
vector<Segment> ccruncher::Segmentation::getSegments() const
{
  return vsegments;
}

//===========================================================================
// addSegment
//===========================================================================
void ccruncher::Segmentation::addSegment(string segname) throw(Exception)
{
  if (modificable == false)
  {
    throw Exception("Segmentation::addSegment(): fixed segments");
  }
  else
  {
    Segment aux = Segment(segname);
    insertSegment(aux);
  }
}

//===========================================================================
// getSegmentName
//===========================================================================
string ccruncher::Segmentation::getSegmentName(int isegment) throw(Exception)
{
  if (isegment < 0 || isegment >= (int) vsegments.size())
  {
    throw Exception("Segmentation::getSegmentName(): index out of range");
  }
  else
  {
    return vsegments[isegment].name;
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Segmentation::getXML(int ilevel) throw(Exception)
{
  string spc = Utils::blanks(ilevel);
  string ret = "";

  ret += spc + "<segmentation name='" + name + "' components='";
  ret += (components==asset?"asset":"client");
  ret += "'>\n";

  if (name == "portfolio")
  {
    // nothing to do
  }
  else if (name == "client" || name == "asset")
  {
    ret += Utils::blanks(ilevel+2) + "<segment name='*'/>\n";
  }
  else
  {
    for (unsigned int i=0;i<vsegments.size();i++)
    {
      if (vsegments[i].name != "rest")
      {
        ret += Utils::blanks(ilevel+2) + "<segment name='" + vsegments[i].name + "'/>\n";
      }
    }
  }

  ret += spc + "</segmentation>\n";

  return ret;
}
