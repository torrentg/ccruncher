
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
// 2005/04/03 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include <cmath>
#include <cassert>
#include <algorithm>
#include "Portfolio.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Portfolio::Portfolio(Ratings *ratings_, Sectors *sectors_, 
             Segmentations *segmentations_, Interests *interests_)
{
  // initializing class
  reset(ratings_, sectors_, segmentations_, interests_);
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
// reset
//===========================================================================
void ccruncher::Portfolio::reset(Ratings *ratings_, Sectors *sectors_, 
             Segmentations *segmentations_, Interests *interests_)
{
  auxclient = NULL;
  
  // setting external objects
  ratings = ratings_;
  sectors = sectors_;
  segmentations = segmentations_;
  interests = interests_;

  // dropping clients
  for(unsigned int i=0;i<vclients.size();i++) {
    delete vclients[i];
  }
  
  // flushing clients
  vclients.clear();
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
      delete val;
      string msg = "Portfolio::insertClient(): client id ";
      msg += val->id;
      msg += " repeated";
      throw Exception(msg);
    }
    else if (vclients[i]->name == val->name)
    {
      delete val;
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
    delete val;
    throw Exception(e);
  }
}

//===========================================================================
// validations
//===========================================================================
void ccruncher::Portfolio::validations() throw(Exception)
{
  if (vclients.size() == 0)
  {
    throw Exception("portfolio without clients");
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Portfolio::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"portfolio")) {
    if (getNumAttributes(attributes) != 0) {
      throw eperror(eu, "attributes are not allowed in tag portfolio");
    }
  }
  else if (isEqual(name_,"client")) {
    auxclient = new Client(ratings, sectors, segmentations, interests);
    eppush(eu, auxclient, name_, attributes);
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Portfolio::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"portfolio")) {
    auxclient = NULL;
    validations();
  }
  else if (isEqual(name_,"client")) {
    assert(auxclient != NULL);
    insertClient(auxclient);
    auxclient = NULL;
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
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
