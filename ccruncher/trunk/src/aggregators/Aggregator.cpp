
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
// Aggregator.cpp - Aggregator code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#include <cmath>
#include <algorithm>
#include "Aggregator.hpp"
#include "utils/XMLUtils.hpp"

//***************************************************************************
// reset
//***************************************************************************
void ccruncher::Aggregator::reset()
{  
  numsegments = 0L;
  saggregators = NULL;
  name = "";
  isegmentation = -1;
  components = COMPONENTS_ASSET;
  bvalues = true;
  bfull = false;
}  

//***************************************************************************
// constructor privat
//***************************************************************************
ccruncher::Aggregator::Aggregator()
{
  reset();
}  

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::Aggregator::Aggregator(const DOMNode& node, Segmentations *segs) throw(Exception)
{
  // reseting default values
  reset();

  // recollim els parametres
  parseDOMNode(node, segs);

  // validating values
  validate(segs);
}

//***************************************************************************
// destructor
//***************************************************************************
ccruncher::Aggregator::~Aggregator()
{
  // deallocating saggregators
  if (saggregators != NULL)
  {
    delete [] saggregators;
    saggregators = NULL;
  }
}

//***************************************************************************
// interpreta un node XML params
//***************************************************************************
void ccruncher::Aggregator::parseDOMNode(const DOMNode& node, Segmentations *segs) throw(Exception)
{
  string stype;
  string sseg;
  
  // validem el node passat com argument
  if (!XMLUtils::isNodeName(node, "aggregator"))
  {
    string msg = "Aggregator::parseDOMNode(): Invalid tag. Expected: aggregator. Found: ";
    msg += XMLUtils::XMLCh2String(node.getNodeName());
    throw Exception(msg);
  }

  // agafem els atributs del node
  DOMNamedNodeMap &attributes = *node.getAttributes();  
  name = XMLUtils::getStringAttribute(attributes, "name", "");
  sseg = XMLUtils::getStringAttribute(attributes, "segmentation", "");
  stype = XMLUtils::getStringAttribute(attributes, "type", "");
  bfull = XMLUtils::getBooleanAttribute(attributes, "full", false);

  // setting name
  if (name == "")
  {
    throw Exception("Aggregator::parseDOMNode(): invalid name attribute at <aggregator>");
  } 

  // setting segmentation
  isegmentation = segs->getSegmentation(sseg);
  components = segs->getComponents(sseg);
  if (isegmentation < 0 || (components != COMPONENTS_ASSET && components != COMPONENTS_CLIENT))
  {
    throw Exception("Aggregator::parseDOMNode(): segmentation " + sseg + " not defined");
  }

  // setting type
  if (stype == "values")
  {
    bvalues = true;
  }
  else if (stype == "ratings")
  {
    bvalues = false;
  }
  else
  {
    throw Exception("Aggregator::parseDOMNode(): type " + stype + " not allowed (try values or ratings)");
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
        throw Exception("Aggregator::parseDOMNode(): invalid data structure at <aggregator>");
      }
    }
  }
}

//***************************************************************************
// validacions de la llista de items recollida
//***************************************************************************
void ccruncher::Aggregator::validate(Segmentations *segs) throw(Exception)
{
  // validating type segmentation
  if (segs->getComponents(isegmentation) == COMPONENTS_ASSET && bvalues == false)
  {
    throw Exception("Aggregator::validate(): trying to aggregate ratings in a asset segmentation");
  }
}

//***************************************************************************
// getName
//***************************************************************************
string ccruncher::Aggregator::getName() const
{
  return name;
}

//***************************************************************************
// getISegmentation
//***************************************************************************
int ccruncher::Aggregator::getISegmentation() const
{
  return isegmentation;
}

//***************************************************************************
// getBValues
//***************************************************************************
bool ccruncher::Aggregator::getBValues() const
{
  return bvalues;
}

//***************************************************************************
// getBFull
//***************************************************************************
bool ccruncher::Aggregator::getBFull() const
{
  return bfull;
}

//***************************************************************************
// initialize
//***************************************************************************
void ccruncher::Aggregator::initialize(Segmentations *segmentations, Date *dates, int m, 
  vector<Client *> *clients, long n, int indexdefault, Ratings *ratings) throw(Exception)
{
  try
  {
    // allocating SegmentAggregator's
    numsegments = (segmentations->getSegmentations())[isegmentation].getSegments().size();
    saggregators = new SegmentAggregator[numsegments];

    // initializing SegmentAggregator's  
    for(long i=0;i<numsegments;i++)
    {
      saggregators[i].define(isegmentation, i, components, bvalues, bfull);
      saggregators[i].setNames(name, segmentations->getSegmentationName(isegmentation), 
                                segmentations->getSegmentName(isegmentation, i));
      saggregators[i].initialize(dates, m, clients, n, indexdefault, ratings);
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, "Aggregator::initialize(): not enougth space");
  }
}

//***************************************************************************
// comparation operator
//***************************************************************************
bool ccruncher::operator == (const Aggregator& a1, const Aggregator& a2)
{
  if ((a1.getISegmentation()==a2.getISegmentation()) && 
      (a1.getBValues()==a2.getBValues()) && 
      (a1.getBFull()==a2.getBFull()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

//***************************************************************************
// append
//***************************************************************************
void ccruncher::Aggregator::append(int **rpaths, int m, long n, vector<Client *> *clients) throw(Exception)
{
  for(long i=0;i<numsegments;i++)
  {
    saggregators[i].append(rpaths, m, n, clients);
    saggregators[i].next();
  }
}

//***************************************************************************
// print
//***************************************************************************
void ccruncher::Aggregator::flush(bool finalize) throw(Exception)
{
  for(long i=0;i<numsegments;i++)
  {
    saggregators[i].flush(finalize);
  }
}

//***************************************************************************
// touch
//***************************************************************************
void ccruncher::Aggregator::touch() throw(Exception)
{
  for(long i=0;i<numsegments;i++)
  {
    saggregators[i].touch();
  }
}

//***************************************************************************
// setOutputProperties
//***************************************************************************
void ccruncher::Aggregator::setOutputProperties(string path, bool force, int buffersize) throw(Exception)
{
  for(long i=0;i<numsegments;i++)
  {
    saggregators[i].setOutputProperties(path, force, buffersize);
  }
}
