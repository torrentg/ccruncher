
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
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include "Rating.hpp"
#include "utils/Utils.hpp"
#include "utils/Parser.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Rating::Rating()
{
  reset();
}

//===========================================================================
// constructor
// TODO: this method will be removed
//===========================================================================
ccruncher::Rating::Rating(const DOMNode& node) throw(Exception)
{
  // inicialitzem les variables de la classe
  reset();

  // recollim els parametres de la simulacio
  parseDOMNode(node);
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Rating::reset()
{
  order = -1;
  name = "";
  desc = "";
}

//===========================================================================
// operador de comparacio (per permetre ordenacio)
//===========================================================================
bool ccruncher::operator <  (const Rating &x, const Rating &y)
{
  return (x.order < y.order);
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Rating::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"rating")) {
    if (getNumAttributes(attributes) != 3) {
      throw eperror(eu, "invalid number of attributes at rating");
    }
    else {
      order = getIntAttribute(attributes, "order", -1);
      name = getStringAttribute(attributes, "name", "");
      desc = getStringAttribute(attributes, "desc", "_UNDEF_");

      if (order <= 0 || name == "" || desc == "_UNDEF_")
      {
        throw eperror(eu, "invalid values at <rating>");
      }
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Rating::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"rating")) {
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

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Rating::getXML(int ilevel) throw(Exception)
{
  string ret = Utils::blanks(ilevel);

  ret += "<rating ";
  ret += "name='" + name + "' ";
  ret += "order='" + Parser::int2string(order) + "' ";
  ret += "desc='" + desc + "'";
  ret += "/>\n";

  return ret;
}
