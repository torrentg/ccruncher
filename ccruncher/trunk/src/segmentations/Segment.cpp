
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
// Segment.cpp - Segment code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include "Segment.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segment::Segment()
{
  name = "";
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segment::Segment(string name_)
{
  name = name_;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segment::Segment(const DOMNode& node) throw(Exception)
{
  // recollim els parametres de la simulacio
  parseDOMNode(node);
}

//===========================================================================
// interpreta un node XML params
//===========================================================================
void ccruncher::Segment::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "segment"))
  {
    string msg = "Segment::parseDOMNode(): Invalid tag. Expected: segment. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem els atributs del node
  DOMNamedNodeMap &attributes = *node.getAttributes();
  name = XMLUtils::getStringAttribute(attributes, "name", "");

  if (name == "")
  {
    throw Exception("Segment::parseDOMNode(): invalid values at <segment>");
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
      else
      {
        string msg = "Segment::parseDOMNode(): invalid data structure at <segment>: ";
        msg += XMLUtils::XMLCh2String(child.getNodeName());
        throw Exception(msg);
      }
    }
  }
}
