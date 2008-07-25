
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
// Segmentations.cpp - Segmentations code - $Rev$
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
// 2005/05/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added methods getNum*
//
// 2005/07/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . moved <cassert> include at last position
//
// 2005/08/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . tag concept renamed to segmentation
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . class refactoring
//
// 2006/02/11 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed method ExpatHandlers::eperror()
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
Segmentation& ccruncher::Segmentations::operator []  (int i)
{
  // assertions
  assert(i >= 0 && i < (int) vsegmentations.size());

  // return i-th segmentation
  return vsegmentations[i];
}

//===========================================================================
// [] operator. returns element by name
//===========================================================================
Segmentation& ccruncher::Segmentations::operator []  (const string &name) throw(Exception)
{
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == name)
    {
      return vsegmentations[i];
    }
  }

  throw Exception("segmentation " + name + " not found");
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
void ccruncher::Segmentations::insertSegmentation(Segmentation &val) throw(Exception)
{
  // checking coherence
  for (unsigned int i=0;i<vsegmentations.size();i++)
  {
    if (vsegmentations[i].name == val.name)
    {
      throw Exception("segmentation name " + val.name + " repeated");
    }
  }

  try
  {
    val.order = vsegmentations.size();
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
// addSegment
//===========================================================================
void ccruncher::Segmentations::addSegment(const string segmentation, const string segment) throw(Exception)
{
  (*this)[segmentation].addSegment(segment);
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
