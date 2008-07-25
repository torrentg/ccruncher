
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
// Segmentations.cpp - Segmentations code
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
#include "segmentations/Segmentations.hpp"
#include "utils/Utils.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segmentations::Segmentations()
{
  // nothing to do
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Segmentations::~Segmentations()
{
  // nothing to do
}

//===========================================================================
// return segmentations list
//===========================================================================
vector<Segmentation> ccruncher::Segmentations::getSegmentations()
{
  return vsegmentations;
}

//===========================================================================
// return segmentation by name
//===========================================================================
int ccruncher::Segmentations::getSegmentation(string name)
{
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == name)
    {
      return i;
    }
  }

  return -1;
}

//===========================================================================
// return components of a segmentation by name
//===========================================================================
components_t ccruncher::Segmentations::getComponents(string name)
{
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == name)
    {
      return vsegmentations[i].components;
    }
  }

  return undefined;
}

//===========================================================================
// return components of a segmentation by name
//===========================================================================
components_t ccruncher::Segmentations::getComponents(int iseg)
{
  if (iseg < 0 || (unsigned int) iseg >= vsegmentations.size())
  {
    return undefined;
  }
  else
  {
    return vsegmentations[iseg].components;
  }
}

//===========================================================================
// return segment by name
//===========================================================================
int ccruncher::Segmentations::getSegment(string segmentation, string segment)
{
  int iconcept = getSegmentation(segmentation);

  if (iconcept >= 0)
  {
    return vsegmentations[iconcept].getSegment(segment);
  }
  else
  {
    return -1;
  }
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::Segmentations::validate() throw(Exception)
{
  if (vsegmentations.size() == 0)
  {
    throw Exception("Segmentations::validate(): no segmentations defined");
  }
}

//===========================================================================
// insert a new segmentation in list
//===========================================================================
void ccruncher::Segmentations::insertSegmentation(Segmentation &val) throw(Exception)
{
  // validem coherencia
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == val.name)
    {
      string msg = "Segmentations::insertSegmentation(): segmentation name ";
      msg += val.name;
      msg += " repeated";
      throw Exception(msg);
    }
  }

  try
  {
    vsegmentations.push_back(val);
  }
  catch(std::exception &e)
  {
     throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentations::epstart(ExpatUserData &eu, const char *name_, const char **attributes)
{
  if (isEqual(name_,"segmentations")) {
    if (getNumAttributes(attributes) != 0) {
      throw eperror(eu, "attributes are not allowed in tag segmentations");
    }
  }
  else if (isEqual(name_,"segmentation")) {
    auxsegmentation.reset();
    eppush(eu, &auxsegmentation, name_, attributes);
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentations::epend(ExpatUserData &eu, const char *name_)
{
  if (isEqual(name_,"segmentations")) {
    validate();
    auxsegmentation.reset();
  }
  else if (isEqual(name_,"segmentation")) {
    insertSegmentation(auxsegmentation);
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name_));
  }
}

//===========================================================================
// addSegment
//===========================================================================
void ccruncher::Segmentations::addSegment(string segmentation, string segment) throw(Exception)
{
  int isegmentation = getSegmentation(segmentation);

  if (isegmentation < 0)
  {
    throw Exception("Segmentations::addSegment(): segmentation " + segmentation + " not found");
  }
  else
  {
    vsegmentations[isegmentation].addSegment(segment);
  }
}

//===========================================================================
// getSegmentationName
//===========================================================================
string ccruncher::Segmentations::getSegmentationName(int isegmentation) throw(Exception)
{
  if (isegmentation < 0 || isegmentation >= (int) vsegmentations.size())
  {
    throw Exception("Segmentations::getSegmentationName(): index out of range");
  }
  else
  {
    return vsegmentations[isegmentation].name;
  }
}

//===========================================================================
// getSegmentName
//===========================================================================
string ccruncher::Segmentations::getSegmentName(int isegmentation, int isegment) throw(Exception)
{
  if (isegmentation < 0 || isegmentation >= (int) vsegmentations.size())
  {
    throw Exception("Segmentations::getSegmentationName(): index out of range");
  }
  else
  {
    return vsegmentations[isegmentation].getSegmentName(isegment);
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Segmentations::getXML(int ilevel) throw(Exception)
{
  string spc = Utils::blanks(ilevel);
  string ret = "";

  ret += spc + "<segmentations>\n";

  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    ret += vsegmentations[i].getXML(ilevel+2);
  }

  ret += spc + "</segmentations>\n";

  return ret;
}
