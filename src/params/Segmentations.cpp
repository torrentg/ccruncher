
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#include "params/Segmentations.hpp"
#include "portfolio/Asset.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segmentations::Segmentations()
{
  // nothing to do
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Segmentations::size() const
{
  return enabled.size();
}

//===========================================================================
// [] operator
//===========================================================================
const Segmentation& ccruncher::Segmentations::getSegmentation(int i) const
{
  // assertions
  if (i >= 0) {
    assert(i < (int) enabled.size());
    return enabled[i];
  }
  else {
    assert(-1-i < (int) disabled.size());
    return disabled[-1-i];
  }
}

//===========================================================================
// return the index of the given segmentation
//===========================================================================
int ccruncher::Segmentations::indexOfSegmentation(const string &sname) const throw(Exception)
{
  for (size_t i=0; i<enabled.size(); i++)
  {
    if (enabled[i].name == sname)
    {
      return (int)i;
    }
  }
  for (size_t i=0; i<disabled.size(); i++)
  {
    if (disabled[i].name == sname)
    {
      return -i-1;
    }
  }
  throw Exception("segmentation " + sname + " not found");
}

//===========================================================================
// return the index of the given segmentation
//===========================================================================
int ccruncher::Segmentations::indexOfSegmentation(const char *sname) const throw(Exception)
{
  for (size_t i=0; i<enabled.size(); i++)
  {
    if (enabled[i].name.compare(sname) == 0)
    {
      return (int)i;
    }
  }
  for (size_t i=0; i<disabled.size(); i++)
  {
    if (disabled[i].name.compare(sname) == 0)
    {
      return -i-1;
    }
  }
  throw Exception("segmentation " + string(sname) + " not found");
}

//===========================================================================
// validate
//===========================================================================
void ccruncher::Segmentations::validate() throw(Exception)
{
  if (enabled.empty())
  {
    throw Exception("don't found active segmentations");
  }

  // checking duplicates
  string str;
  int nbasic = 0;
  for (size_t i=0; i<enabled.size(); i++) {
    if (enabled[i].size() == 1) {
      str += (nbasic>0?", ":"") + enabled[i].name;
      nbasic++;
    }
  }
  for (size_t i=0; i<disabled.size(); i++) {
    if (disabled[i].size() == 1) {
      str += (nbasic>0?", ":"") + disabled[i].name;
      nbasic++;
    }
  }
  if (nbasic > 1)
  {
    throw Exception("found duplicated segmentations [" + str + "]");
  }
}

//===========================================================================
// insert a new segmentation in list
//===========================================================================
int ccruncher::Segmentations::insertSegmentation(Segmentation &val) throw(Exception)
{
  // checking coherence
  for (size_t i=0; i<enabled.size(); i++)
  {
    if (enabled[i].name == val.name)
    {
      throw Exception("segmentation name " + val.name + " repeated");
    }
  }
  for (size_t i=0; i<disabled.size(); i++)
  {
    if (disabled[i].name == val.name)
    {
      throw Exception("segmentation name " + val.name + " repeated");
    }
  }

  // inserting segmentation
  if (val.isEnabled())
  {
    enabled.push_back(val);
    return enabled.size()-1;
  }
  else
  {
    disabled.push_back(val);
    return -disabled.size();
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
void ccruncher::Segmentations::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"segmentation")) {
    insertSegmentation(auxsegmentation);
  }
  else if (isEqual(name_,"segmentations")) {
    validate();
    auxsegmentation.reset();
  }
}

//===========================================================================
// add components to segmentations stats
//===========================================================================
void ccruncher::Segmentations::addComponents(const Asset *asset)
{
  assert(asset != NULL);
  for(size_t i=0; i<enabled.size(); i++)
  {
    enabled[i].addComponent(asset->getSegment(i));
  }
}

//===========================================================================
// remove unused segments
//===========================================================================
void ccruncher::Segmentations::removeUnusedSegments()
{
  for(size_t i=0; i<enabled.size(); i++)
  {
    enabled[i].removeUnusedSegments();
  }
}

//===========================================================================
// recode segments removing unused segments
//===========================================================================
void ccruncher::Segmentations::recodeSegments(Asset *asset)
{
  assert(asset != NULL);
  for(size_t i=0; i<enabled.size(); i++)
  {
    int old = asset->getSegment(i);
    asset->setSegment(i, enabled[i].recode(old));
  }

}

