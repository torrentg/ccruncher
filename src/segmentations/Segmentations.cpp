
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

#include "segmentations/Segmentations.hpp"
#include "utils/Strings.hpp"
#include <cassert>

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
// size
//===========================================================================
int ccruncher::Segmentations::size() const
{
  return vsegmentations.size();
}

//===========================================================================
// [] operator
//===========================================================================
Segmentation& ccruncher::Segmentations::getSegmentation(int i)
{
  // assertions
  assert(i >= 0 && i < (int) vsegmentations.size());

  // return i-th segmentation
  return vsegmentations[i];
}

//===========================================================================
// return the index of the given segmentation
//===========================================================================
int ccruncher::Segmentations::indexOfSegmentation(const string &sname) throw(Exception)
{
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == sname)
    {
      return (int)i;
    }
  }

  throw Exception("segmentation " + sname + " not found");
}

//===========================================================================
// return the index of the given segmentation
//===========================================================================
int ccruncher::Segmentations::indexOfSegmentation(const char *sname) throw(Exception)
{
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name.compare(sname) == 0)
    {
      return (int)i;
    }
  }

  throw Exception("segmentation " + string(sname) + " not found");
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::Segmentations::validate() throw(Exception)
{
  if (vsegmentations.size() == 0)
  {
    throw Exception("no segmentations defined");
  }
}

//===========================================================================
// insert a new segmentation in list
//===========================================================================
int ccruncher::Segmentations::insertSegmentation(Segmentation &val) throw(Exception)
{
  // checking coherence
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == val.name)
    {
      throw Exception("segmentation name " + val.name + " repeated");
    }
  }

  // checking special segmentations
  if (val.name == "obligors" && val.components != obligor)
  {
    throw Exception("segmentation 'obligors' needs components of type obligor");
  }
  if (val.name == "assets" && val.components != asset)
  {
    throw Exception("segmentation 'assets' needs components of type asset");
  }

  try
  {
    vsegmentations.push_back(val);
    return vsegmentations.size()-1;
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
      throw Exception("attributes are not allowed in tag segmentations");
    }
  }
  else if (isEqual(name_,"segmentation")) {
    auxsegmentation.reset();
    eppush(eu, &auxsegmentation, name_, attributes);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentations::epend(ExpatUserData &eu, const char *name_)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name_,"segmentations")) {
    validate();
    auxsegmentation.reset();
  }
  else if (isEqual(name_,"segmentation")) {
    insertSegmentation(auxsegmentation);
  }
  else {
    throw Exception("unexpected end tag " + string(name_));
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Segmentations::getXML(int ilevel) const throw(Exception)
{
  string spc = Strings::blanks(ilevel);
  string ret = "";

  ret += spc + "<segmentations>\n";

  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    ret += vsegmentations[i].getXML(ilevel+2);
  }

  ret += spc + "</segmentations>\n";

  return ret;
}
