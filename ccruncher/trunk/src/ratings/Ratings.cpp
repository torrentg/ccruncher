
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
// Ratings.cpp - Ratings code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "Ratings.hpp"
#include "utils/Utils.hpp"
#include "utils/XMLUtils.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// constructor privat
//===========================================================================
ccruncher::Ratings::Ratings()
{
  // inicialitzem el vector de ratings
  vratings = vector<Rating>();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Ratings::Ratings(const DOMNode& node) throw(Exception)
{
  // inicialitzem el vector de rates
  vratings = vector<Rating>();

  // recollim els parametres de la simulacio
  parseDOMNode(node);

  // validem la llista recollida
  validations();
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Ratings::~Ratings()
{
  // cal assegurar que es destrueix vratings;
}

//===========================================================================
// destructor
//===========================================================================
vector<Rating> * ccruncher::Ratings::getRatings()
{
  return &vratings;
}

//===========================================================================
// insercio nou rating en la llista
//===========================================================================
void ccruncher::Ratings::insertRating(Rating &val) throw(Exception)
{
  // validem coherencia
  for (unsigned int i=0;i<vratings.size();i++)
  {
    Rating aux = vratings[i];

    if (aux.name == val.name)
    {
      string msg = "Ratings::insertRating(): rating name ";
      msg += val.name;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (aux.order == val.order)
    {
      string msg = "Ratings::insertRating(): rating order ";
      msg += val.order;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (aux.desc == val.desc)
    {
      string msg = "Ratings::insertRating(): rating desc ";
      msg += val.desc;
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vratings.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// interpreta un node XML params
//===========================================================================
void ccruncher::Ratings::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "ratings"))
  {
    string msg = "Ratings::parseDOMNode(): Invalid tag. Expected: ratings. Found: ";
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
      else if (XMLUtils::isNodeName(child, "rating"))
      {
        Rating aux = Rating(child);
        insertRating(aux);
      }
      else
      {
        throw Exception("Ratings::parseDOMNode(): invalid data structure at <ratings>");
      }
    }
  }
}

//===========================================================================
// validacions de la llista de ratings recollida
//===========================================================================
void ccruncher::Ratings::validations() throw(Exception)
{
  // validacio longitud
  if (vratings.size() == 0)
  {
    throw Exception("Ratings::validations(): ratings have no elements");
  }

  // ordenem la llista de ratings per camp order
  sort(vratings.begin(), vratings.end());

  // comprovem que el camp order comença per 1 i no hi ha buits
  for(unsigned int i=0;i<vratings.size();i++)
  {
    Rating aux = vratings[i];

    if (aux.order != (int)(i+1))
    {
      string msg = "Ratings::validations(): incorrect order rating at or near order = ";
      msg += Parser::int2string(aux.order);
      throw Exception(msg);
    }
  }
}

//===========================================================================
// retorna el index del rating dins la llista (-1 si no es troba)
//===========================================================================
int ccruncher::Ratings::getIndex(const string &rating_name)
{

  for (unsigned int i=0;i<vratings.size();i++)
  {
    if (vratings[i].name == rating_name)
    {
      return (int) i;
    }
  }

  return -1;
}

//===========================================================================
// retorna el nom del rating dins la llista (-1 si no es troba)
//===========================================================================
string ccruncher::Ratings::getName(int index) throw(Exception)
{
  if (index < 0 || index >= (int) vratings.size())
  {
    throw Exception("Ratings::getName(): index out of range");
  }
  else
  {
    return vratings[index].name;
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Ratings::getXML(int ilevel) throw(Exception)
{
  string spc = Utils::blanks(ilevel);
  string ret = "";

  ret += spc + "<ratings>\n";

  for (unsigned int i=0;i<vratings.size();i++)
  {
    ret += vratings[i].getXML(ilevel+2);
  }

  ret += spc + "</ratings>\n";

  return ret;
}
