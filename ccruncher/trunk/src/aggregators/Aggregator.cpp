
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
// Aggregator.cpp - Aggregator code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Strings class
//
//===========================================================================

#include <cmath>
#include <cassert>
#include <algorithm>
#include "aggregators/Aggregator.hpp"
#include "utils/Strings.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// reset
//===========================================================================
void ccruncher::Aggregator::reset(Segmentations *segs)
{
  numsegments = 0L;
  saggregators = NULL;
  name = "";
  isegmentation = -1;
  components = asset;
  bvalues = true;
  bfull = false;
  segmentations = segs;
}

//===========================================================================
// constructor (don't use)
//===========================================================================
ccruncher::Aggregator::Aggregator()
{
  // reseting default values
  reset(NULL);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Aggregator::Aggregator(Segmentations *segs)
{
  // reseting default values
  reset(segs);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Aggregator::~Aggregator()
{
  // deallocating saggregators
  if (saggregators != NULL)
  {
    delete [] saggregators;
    saggregators = NULL;
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Aggregator::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"aggregator")) {
    if (getNumAttributes(attributes) != 4) {
      throw eperror(eu, "incorrect number of attributes in tag aggregator");
    }
    else {
    
      string sseg = getStringAttribute(attributes, "segmentation", "");
      string stype = getStringAttribute(attributes, "type", "");
      name = getStringAttribute(attributes, "name", "");
      bfull = getBooleanAttribute(attributes, "full", false);

      // setting name value
      if (name == "") {
        throw eperror(eu, "invalid name attribute at <aggregator>");
      }
      
      // setting isegmentation value
      isegmentation = segmentations->getSegmentation(sseg);
      
      // setting components value
      components = segmentations->getComponents(sseg);
      if (isegmentation < 0 || (components != asset && components != client))
      {
        throw eperror(eu, "segmentation " + sseg + " not defined");
      }
      
      // setting type value
      if (stype == "values") {
        bvalues = true;
      }
      else if (stype == "ratings") {
        bvalues = false;
      }
      else {
        throw eperror(eu, "type " + stype + " not allowed (try values or ratings)");
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
void ccruncher::Aggregator::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"aggregator")) {
    validate();
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// validacions de la llista de items recollida
//===========================================================================
void ccruncher::Aggregator::validate() throw(Exception)
{
  // validating type segmentation
  if (segmentations->getComponents(isegmentation) == asset && bvalues == false)
  {
    throw Exception("Aggregator::validate(): trying to aggregate ratings in a asset segmentation");
  }
}

//===========================================================================
// getName
//===========================================================================
string ccruncher::Aggregator::getName() const
{
  return name;
}

//===========================================================================
// getISegmentation
//===========================================================================
int ccruncher::Aggregator::getISegmentation() const
{
  return isegmentation;
}

//===========================================================================
// getBValues
//===========================================================================
bool ccruncher::Aggregator::getBValues() const
{
  return bvalues;
}

//===========================================================================
// getBFull
//===========================================================================
bool ccruncher::Aggregator::getBFull() const
{
  return bfull;
}

//===========================================================================
// getNumSegments
//===========================================================================
long ccruncher::Aggregator::getNumSegments() throw(Exception)
{
  assert(segmentations != NULL);
  assert(isegmentation >= 0);

  try
  {
    return (segmentations->getSegmentations())[isegmentation].getSegments().size();
  }
  catch(std::exception &e)
  {
    throw Exception(e, "Aggregator::getNumSegments()");
  }
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::Aggregator::initialize(Date *dates, int m, vector<Client *> *clients,
     long n, int indexdefault, Ratings *ratings, Interests *interests) throw(Exception)
{
  try
  {
    // allocating SegmentAggregator's
    numsegments = getNumSegments();
    saggregators = new SegmentAggregator[numsegments];

    // initializing SegmentAggregator's
    for(long i=0;i<numsegments;i++)
    {
      saggregators[i].define(isegmentation, i, components, bvalues, bfull);
      saggregators[i].setNames(name, segmentations->getSegmentationName(isegmentation),
                                segmentations->getSegmentName(isegmentation, i));
      saggregators[i].initialize(dates, m, clients, n, indexdefault, ratings, interests);
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, "Aggregator::initialize(): not enougth space");
  }
}

//===========================================================================
// comparation operator
//===========================================================================
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

//===========================================================================
// append
//===========================================================================
void ccruncher::Aggregator::append(int **rpaths, int m, long n, vector<Client *> *clients) throw(Exception)
{
  for(long i=0;i<numsegments;i++)
  {
    saggregators[i].append(rpaths, m, n, clients);
    saggregators[i].next();
  }
}

//===========================================================================
// print
//===========================================================================
void ccruncher::Aggregator::flush(bool finalize) throw(Exception)
{
  for(long i=0;i<numsegments;i++)
  {
    saggregators[i].flush(finalize);
  }
}

//===========================================================================
// touch
//===========================================================================
void ccruncher::Aggregator::touch() throw(Exception)
{
  for(long i=0;i<numsegments;i++)
  {
    saggregators[i].touch();
  }
}

//===========================================================================
// setOutputProperties
//===========================================================================
void ccruncher::Aggregator::setOutputProperties(string path, bool force, int buffersize) throw(Exception)
{
  for(long i=0;i<numsegments;i++)
  {
    saggregators[i].setOutputProperties(path, force, buffersize);
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Aggregator::getXML(int ilevel) throw(Exception)
{
  string ret = Strings::blanks(ilevel);

  ret += "<aggregator ";
  ret += "name='" + name + "' ";
  ret += "segmentation='"+ segmentations->getSegmentationName(isegmentation);
  ret += "' type='";
  ret += (bvalues==true?"values":"ratings");
  ret += "' full='" + Parser::bool2string(bfull) + "'";
  ret += "/>\n";

  return ret;
}
