
//***************************************************************************
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
//***************************************************************************

#include <cmath>
#include <algorithm>
#include "Aggregators.hpp"
#include "utils/XMLUtils.hpp"

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::Aggregators::Aggregators(const DOMNode& node, Segmentations *segs) throw(Exception)
{
  // recollim els parametres de la simulacio
  parseDOMNode(node, segs);
  
  // making validations
  validate();
}

//***************************************************************************
// destructor
//***************************************************************************
ccruncher::Aggregators::~Aggregators()
{
  // nothing to do
}

//***************************************************************************
// return interests list
//***************************************************************************
vector<Aggregator> * ccruncher::Aggregators::getAggregators() 
{
  return &vaggregators;
}

//***************************************************************************
// validate
//***************************************************************************
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

//***************************************************************************
// insert a new interest in list
//***************************************************************************
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

//***************************************************************************
// interpreta un node XML 
//***************************************************************************
void ccruncher::Aggregators::parseDOMNode(const DOMNode& node, Segmentations *segs) throw(Exception)
{
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "aggregators"))
  {
    string msg = "Interests::parseDOMNode(): Invalid tag. Expected: aggregators. Found: ";
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
      else if (XMLUtils::isNodeName(child, "aggregator"))
      {
        Aggregator aux = Aggregator(child, segs);
        insertAggregator(aux);
      }
      else
      {
        throw Exception("Aggregators::parseDOMNode(): invalid data structure at <aggregators>");
      }
    }
  }
}

//***************************************************************************
// initialize
//***************************************************************************
void ccruncher::Aggregators::initialize(Segmentations *segmentations, Date *dates, int m, 
vector<Client *> *clients, int n, int indexdefault, Ratings *ratings) throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].initialize(segmentations, dates, m, clients, n, indexdefault, ratings);
  }
}

//***************************************************************************
// append
//***************************************************************************
void ccruncher::Aggregators::append(int **rpaths, int m, long n, vector<Client *> *clients) throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].append(rpaths, m, n, clients);
  }
}

//***************************************************************************
// print
//***************************************************************************
void ccruncher::Aggregators::flush(bool finalize) throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].flush(finalize);
  }
}

//***************************************************************************
// touch
//***************************************************************************
void ccruncher::Aggregators::touch() throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].touch();
  }
}

//***************************************************************************
// setOutputProperties
//***************************************************************************
void ccruncher::Aggregators::setOutputProperties(string path, bool force, int buffersize) throw(Exception)
{
  for(unsigned int i=0;i<vaggregators.size();i++)
  {
    vaggregators[i].setOutputProperties(path, force, buffersize);
  }
}
