
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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
//===========================================================================

#include <cmath>
#include <algorithm>
#include "portfolio/Portfolio.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Portfolio::Portfolio(const Ratings &ratings_, const Sectors &sectors_,
             Segmentations &segmentations_, const Interests &interests_, 
             const Date &date1_, const Date &date2_)
{
  auxborrower = NULL;
  vborrowers.clear();
  // setting external objects
  ratings = &ratings_;
  sectors = &sectors_;
  segmentations = &segmentations_;
  interests = &interests_;
  date1 = date1_;
  date2 = date2_;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Portfolio::~Portfolio()
{
  // dropping borrowers
  for(unsigned int i=0;i<vborrowers.size();i++)
  {
    delete vborrowers[i];
  }
}

//===========================================================================
// returns borrower list
//===========================================================================
vector<Borrower *> & ccruncher::Portfolio::getBorrowers()
{
  return vborrowers;
}

//===========================================================================
// inserting a borrower into list
//===========================================================================
void ccruncher::Portfolio::insertBorrower(Borrower &val) throw(Exception)
{
  unsigned int vcs = vborrowers.size();
  unsigned long chkey = val.hkey;
  Borrower *curr = NULL;

  // checking coherence
  for (unsigned int i=0;i<vcs;i++)
  {
    curr = vborrowers[i];

    if (curr->hkey == chkey) // checking borrower id uniqueness
    {
      // resolving hash key collision
      if (curr->id == val.id)
      {
        string msg = "borrower id " + val.id + " repeated";
        delete &val;
        throw Exception(msg);
      }
    }
    else // checking asset id uniqueness
    {
      vector<Asset> &currassets = curr->getAssets();
      for(unsigned int j=0; j<currassets.size(); j++)
      {
        for(unsigned int k=0; k<val.getAssets().size(); k++)
        {
          if (currassets[j].hkey == val.getAssets()[k].hkey)
          {
            // resolving hash key collision
            if (currassets[j].getId() == val.getAssets()[k].getId())
            {
              string msg = "borrowers " + curr->name + " and " + val.name + 
                            " have a asset with same id (" + val.getAssets()[k].getId() + ")";
              delete &val;
              throw Exception(msg);
            }
          }
        }
      }
    }
  }

  try
  {
    vborrowers.push_back(&val);
  }
  catch(std::exception &e)
  {
    delete &val;
    throw Exception(e);
  }
}

//===========================================================================
// validations
//===========================================================================
void ccruncher::Portfolio::validations() throw(Exception)
{
  if (vborrowers.size() == 0)
  {
    throw Exception("portfolio without borrowers");
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Portfolio::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"portfolio")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag portfolio");
    }
  }
  else if (isEqual(name_,"borrower")) {
    auxborrower = new Borrower(*ratings, *sectors, *segmentations, *interests, date1, date2);
    eppush(eu, auxborrower, name_, attributes);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Portfolio::epend(ExpatUserData &eu, const char *name_)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"portfolio")) {
    // cleaning temp objects
    auxborrower = NULL;
    // checking coherence
    validations();
  }
  else if (isEqual(name_,"borrower")) {
    assert(auxborrower != NULL);
    insertBorrower(*auxborrower);
    auxborrower = NULL;
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// getNumActiveBorrowers
//===========================================================================
int ccruncher::Portfolio::getNumActiveBorrowers(const Date &from, const Date &to) throw(Exception)
{
  int ret = 0;

  for (int i=vborrowers.size()-1;i>=0;i--)
  {
    if (vborrowers[i]->isActive(from, to))
    {
      ret++;
    }
  }

  return ret;
}

//===========================================================================
// sortBorrowers
//===========================================================================
void ccruncher::Portfolio::sortBorrowers(const Date &from, const Date &to, bool onlyactive) throw(Exception)
{
  // sorting borrower by sector and rating
  sort(vborrowers.begin(), vborrowers.end(), Borrower::less);

  if (onlyactive == true)
  {
    // we move non-active borrowers to last position of array
    for(unsigned int cont=0,i=0;cont<vborrowers.size();cont++)
    {
      if (!(*vborrowers[i]).isActive(from,to))
      {
        mtlp(i);
        i--;
      }

      i++;
    }
  }
}

//===========================================================================
// mtlp. move to last position
//===========================================================================
void ccruncher::Portfolio::mtlp(unsigned int pos)
{
  Borrower *p = vborrowers[pos];

  for(unsigned int i=pos;i<vborrowers.size()-1;i++)
  {
    vborrowers[i] = vborrowers[i+1];
  }

  vborrowers[vborrowers.size()-1] = p;
}

