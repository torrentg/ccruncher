
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
// Rating.cpp - Rating code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include "Rating.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// inicialitzador
//===========================================================================
void ccruncher::Rating::init()
{
  order = -1;
  name = "";
  desc = "";
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Rating::Rating()
{
  init();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Rating::Rating(const DOMNode& node) throw(Exception)
{
  // inicialitzem les variables de la classe
  init();

  // recollim els parametres de la simulacio
  parseDOMNode(node);
}

//===========================================================================
// operador de comparacio (per permetre ordenacio)
//===========================================================================
bool ccruncher::operator <  (const Rating &x, const Rating &y)
{
  return (x.order < y.order);
}

//===========================================================================
// interpreta un node XML params
//===========================================================================
void ccruncher::Rating::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "rating"))
  {
    string msg = "Rating::parseDOMNode(): Invalid tag. Expected: rating. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem els atributs del node
  DOMNamedNodeMap &attributes = *node.getAttributes();
  order = XMLUtils::getIntAttribute(attributes, "order", -1);
  name = XMLUtils::getStringAttribute(attributes, "name", "");
  desc = XMLUtils::getStringAttribute(attributes, "desc", "_UNDEF_");

  if (order <= 0 || name == "" || desc == "_UNDEF_")
  {
    throw Exception("Rating::parseDOMNode(): invalid values at <rating>");
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
        string msg = "Rating::parseDOMNode(): invalid data structure at <rating>: ";
        msg += XMLUtils::XMLCh2String(child.getNodeName());
        throw Exception(msg);
      }
    }
  }
}
