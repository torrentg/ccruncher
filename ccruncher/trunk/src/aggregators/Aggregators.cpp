
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
// Aggregators.cpp - Aggregators code
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
#include "Aggregators.hpp"
#include "utils/Utils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Aggregators::Aggregators(Segmentations *segs)
{
  segmentations = segs;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Aggregators::~Aggregators()
{
  // nothing to do
}

//===========================================================================
// return interests list
//===========================================================================
vector<Aggregator> * ccruncher::Aggregators::getAggregators()
{
  return &vaggregators;
}

//===========================================================================
// getNumSegments
//===========================================================================
long ccruncher::Aggregators::getNumSegments() throw(Exception)
{
  long ret = 0L;
  
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    ret += vaggregators[i].getNumSegments();
  }
  
  return ret;
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::Aggregators::validate() throw(Exception)
{
  if (vaggregators.size() == 0)
  {
    throw Exception("Aggregators::validate(): non exist any aggregator");
  }

  // checking for repetitions
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    for(unsigned int j=i+1;j<vaggregators.size();j++)
    {
      if (vaggregators[i].getName() == vaggregators[j].getName())
      {
        throw Exception("Aggregators::validate(): aggregator name " +
                         vaggregators[i].getName() + " repeated");
      }
      else if (vaggregators[i] == vaggregators[j])
      {
        throw Exception("Aggregators::validate(): aggregators " +
                         vaggregators[i].getName() + " and " +
                         vaggregators[j].getName() + " are equals");
      }
      else
      {
        // nothing to do
      }
    }
  }
}

//===========================================================================
// insert a new interest in list
//===========================================================================
void ccruncher::Aggregators::insertAggregator(Aggregator &val) throw(Exception)
{
  // validem coherencia
  for (unsigned int i=0;i<vaggregators.size();i++)
  {
    if (vaggregators[i].getName() == val.getName())
    {
      string msg = "Aggregators::insertAggregator(): aggregator name ";
      msg += val.getName();
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vaggregators.push_back(val);
  }
  catch(std::exception &e)
  {
     throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Aggregators::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"aggregators")) {
    if (getNumAttributes(attributes) != 0) {
      throw eperror(eu, "attributes are not allowed in tag aggregators");
    }
  }
  else if (isEqual(name_,"aggregator")) {
    auxaggregator.reset(segmentations);
    eppush(eu, &auxaggregator, name_, attributes);
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Aggregators::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"aggregators")) {
    validate();
    auxaggregator.reset(NULL);
  }
  else if (isEqual(name_,"aggregator")) {
    insertAggregator(auxaggregator);
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::Aggregators::initialize(Date *dates, int m, vector<Client *> *clients,
int n, int indexdefault, Ratings *ratings, Interests *interests) throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].initialize(dates, m, clients, n, indexdefault, ratings, interests);
  }
}

//===========================================================================
// append
//===========================================================================
void ccruncher::Aggregators::append(int **rpaths, int m, long n, vector<Client *> *clients) throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].append(rpaths, m, n, clients);
  }
}

//===========================================================================
// print
//===========================================================================
void ccruncher::Aggregators::flush(bool finalize) throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].flush(finalize);
  }
}

//===========================================================================
// touch
//===========================================================================
void ccruncher::Aggregators::touch() throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].touch();
  }
}

//===========================================================================
// setOutputProperties
//===========================================================================
void ccruncher::Aggregators::setOutputProperties(string path, bool force, int buffersize) throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].setOutputProperties(path, force, buffersize);
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Aggregators::getXML(int ilevel) throw(Exception)
{
  string spc = Utils::blanks(ilevel);
  string ret = "";

  ret += spc + "<aggregators>\n";

  for (unsigned int i=0;i<vaggregators.size();i++)
  {
    ret += vaggregators[i].getXML(ilevel+2);
  }

  ret += spc + "</aggregators>\n";

  return ret;
}
