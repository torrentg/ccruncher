
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
// Portfolio.cpp - Portfolio code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <algorithm>
#include "Portfolio.hpp"
#include "utils/XMLUtils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Portfolio::Portfolio(Ratings *ratings, Sectors *sectors, Segmentations *segmentations, 
                     Interests *interests, const DOMNode& node) throw(Exception)
{
  // recollim els parametres de la simulacio
  parseDOMNode(ratings, sectors, segmentations, interests, node);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Portfolio::~Portfolio()
{
  // dropping clients
  for(unsigned int i=0;i<vclients.size();i++)
  {
    delete vclients[i];
  }
}

//===========================================================================
// retorna la llista de clients
//===========================================================================
vector<Client *> * ccruncher::Portfolio::getClients() 
{
  return &vclients;
}

//===========================================================================
// insercio nou client en la llista
//===========================================================================
void ccruncher::Portfolio::insertClient(Client *val) throw(Exception) 
{
  // validem coherencia
  for (unsigned int i=0;i<vclients.size();i++)
  {
    if (vclients[i]->id == val->id)
    {
      string msg = "Portfolio::insertClient(): client id ";
      msg += val->id;
      msg += " repeated";    	
      throw Exception(msg);
    }
    else if (vclients[i]->name == val->name)
    {
      string msg = "Portfolio::insertClient(): client name ";
      msg += val->name;
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vclients.push_back(val);
  }
  catch(std::exception &e)
  {
     throw Exception(e);
  }
}


//===========================================================================
// interpreta un node XML params
//===========================================================================
void ccruncher::Portfolio::parseDOMNode(Ratings *ratings, Sectors *sectors, Segmentations *segmentations, 
                             Interests *interests, const DOMNode& node) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "portfolio"))
  {
    string msg = "Portfolio::parseDOMNode(): Invalid tag. Expected: portfolio. Found: ";
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
      else if (XMLUtils::isNodeName(child, "client"))
      {
        Client *aux = new Client(ratings, sectors, segmentations, interests, child);
        insertClient(aux);
      }
      else
      {
        throw Exception("Portfolio::parseDOMNode(): invalid data structure at <portfolio>");
      }
    }
  }
}

//===========================================================================
// getNumActiveClients
//===========================================================================
int ccruncher::Portfolio::getNumActiveClients(Date from, Date to) throw(Exception)
{
  int ret = 0;
  
  for (int i=vclients.size()-1;i>=0;i--)
  {
    if (vclients[i]->isActive(from, to))
    {
      ret++;
    }
  }
  
  return ret;
}

//===========================================================================
// sortClients
//===========================================================================
void ccruncher::Portfolio::sortClients(Date from, Date to) throw(Exception)
{
  // sorting clients by sector and rating
  sort(vclients.begin(), vclients.end(), Client::less);

  // we move non-active clients to last position of array
  for(unsigned int cont=0,i=0;cont<vclients.size();cont++)
  {
    if (!(*vclients[i]).isActive(from,to))
    {
      mtlp(i);
      i--;
    }

    i++;
  }
}

//===========================================================================
// mtlp. move to last position
//===========================================================================
void ccruncher::Portfolio::mtlp(unsigned int pos)
{
  Client *p = vclients[pos];

  for(unsigned int i=pos;i<vclients.size()-1;i++)
  {
    vclients[i] = vclients[i+1];
  }
  
  vclients[vclients.size()-1] = p;
}
