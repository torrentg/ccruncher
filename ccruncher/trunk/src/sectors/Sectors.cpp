
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
// Sectors.cpp - Sectors code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "Sectors.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// constructor privat
//===========================================================================
ccruncher::Sectors::Sectors()
{
  // inicialitzem el vector de sectors
  vsectors = vector<Sector>();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Sectors::Sectors(const DOMNode& node) throw(Exception)
{
  // inicialitzem el vector de sectors
  vsectors = vector<Sector>();

  // recollim els parametres de la simulacio
  parseDOMNode(node);

  // validem la llista recollida
  validations();
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Sectors::~Sectors()
{
  // cal assegurar que es destrueix vsectors;
}

//===========================================================================
// destructor
//===========================================================================
vector<Sector> * ccruncher::Sectors::getSectors()
{
  return &vsectors;
}

//===========================================================================
// insercio nou sector en la llista
//===========================================================================
void ccruncher::Sectors::insertSector(Sector &val) throw(Exception)
{
  // validem coherencia
  for (unsigned int i=0;i<vsectors.size();i++)
  {
    Sector aux = vsectors[i];

    if (aux.name == val.name)
    {
      string msg = "Sectors::insertSector(): sector name ";
      msg += val.name;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (aux.order == val.order)
    {
      string msg = "Sectors::insertSector(): sector order ";
      msg += val.order;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (aux.desc == val.desc)
    {
      string msg = "Sectors::insertSector(): sector desc ";
      msg += val.desc;
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vsectors.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// interpreta un node XML params
//===========================================================================
void ccruncher::Sectors::parseDOMNode(const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "sectors"))
  {
    string msg = "Sectors::parseDOMNode(): Invalid tag. Expected: sectors. Found: ";
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
      else if (XMLUtils::isNodeName(child, "sector"))
      {
        Sector aux = Sector(child);
        insertSector(aux);
      }
      else
      {
        throw Exception("Sectors::parseDOMNode(): invalid data structure at <sectors>");
      }
    }
  }
}

//===========================================================================
// validacions de la llista de sectors recollida
//===========================================================================
void ccruncher::Sectors::validations() throw(Exception)
{
  // validacio longitud
  if (vsectors.size() == 0)
  {
    throw Exception("Sectors::validations(): sectors have no elements");
  }

  // ordenem la llista de sectors per camp order
  sort(vsectors.begin(), vsectors.end());

  // comprovem que el camp order comença per 1 i no hi ha buits
  for(unsigned int i=0;i<vsectors.size();i++)
  {
    Sector aux = vsectors[i];

    if (aux.order != (int)(i+1))
    {
      string msg = "Sectors::validations(): incorrect order sector at or near order = ";
      msg += aux.order;
      throw Exception(msg);
    }
  }
}

//===========================================================================
// retorna el index del rating dins la llista (-1 si no es troba)
//===========================================================================
int ccruncher::Sectors::getIndex(const string &sector_name)
{

  for (unsigned int i=0;i<vsectors.size();i++)
  {
    if (vsectors[i].name == sector_name)
    {
      return (int) i;
    }
  }

  return -1;
}
