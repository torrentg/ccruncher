
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

#include "params/Segmentation.hpp"
#include "utils/File.hpp"
#include <cassert>

#define UNASSIGNED "unassigned"

using namespace std;

//===========================================================================
// constructor
//===========================================================================
ccruncher::Segmentation::Segmentation()
{
  // default values
  reset();
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Segmentation::size() const
{
  return segments.size();
}

//===========================================================================
// returns i-th segment
//===========================================================================
const string& ccruncher::Segmentation::getSegment(int i) const
{
  // assertions
  assert(i >= 0 && i < (int) segments.size());

  // return i-th segment
  return segments[i];
}

//===========================================================================
// return the index of the given segment
//===========================================================================
int ccruncher::Segmentation::indexOfSegment(const string &sname) const throw(Exception)
{
  for (unsigned int i=0; i<segments.size(); i++)
  {
    if (segments[i] == sname)
    {
      return (int)i;
    }
  }

  throw Exception("segment " + sname + " not found");
}

//===========================================================================
// return the index of the given segment
//===========================================================================
int ccruncher::Segmentation::indexOfSegment(const char *sname) const throw(Exception)
{
  assert(sname != NULL);

  for (unsigned int i=0; i<segments.size(); i++)
  {
    if (segments[i].compare(sname) == 0)
    {
      return (int)i;
    }
  }

  throw Exception("segment " + string(sname) + " not found");
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::Segmentation::reset()
{
  segments.clear();
  numcomponents.clear();
  recode_map.clear();
  enabled = true;
  insertSegment(UNASSIGNED); // adding catcher segment
  name = "";
  components = obligor;
}

//===========================================================================
// isValidIdentifier
//===========================================================================
bool ccruncher::Segmentation::isValidName(const string &str)
{
  if (str.length() == 0) return false;
  for(size_t i=0; i<str.length(); i++) {
    if (!isalnum(str[i]) && strchr("+-._",str[i]) == NULL) {
      return false;
    }
  }
  return true;
}

//===========================================================================
// insertSegment
//===========================================================================
int ccruncher::Segmentation::insertSegment(const string &sname) throw(Exception)
{
  if (sname == "")
  {
    throw Exception("trying to insert a segment with invalid name (void name)");
  }

  // checking coherence
  for (unsigned int i=0; i<segments.size(); i++)
  {
    if (segments[i] == sname)
    {
      throw Exception("segment " + segments[i] + " repeated");
    }
  }

  // inserting value
  try
  {
    segments.push_back(sname);
    numcomponents.push_back(0);
    return segments.size()-1;
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentation::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"segmentation")) {
    if (getNumAttributes(attributes) < 2 || 3 < getNumAttributes(attributes)) {
      throw Exception("incorrect number of attributes in tag segmentation");
    }
    name = getStringAttribute(attributes, "name");
    if (!isValidName(name)) {
      throw Exception("segmentation name '" + name + "' is not valid");
    }
    enabled = getBooleanAttribute(attributes, "enabled", true);
    string strcomp = getStringAttribute(attributes, "components");
    // filling components variable
    if (strcomp == "asset") {
      components = asset;
    }
    else if (strcomp == "obligor") {
      components = obligor;
    }
    else {
      throw Exception("tag <segmentation> with invalid components attribute");
    }
  }
  else if (isEqual(name_,"segment")) {
    string sname = getStringAttribute(attributes, "name");
    insertSegment(sname);
  }
  else {
    throw Exception("unexpected tag " + string(name_));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Segmentation::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"segmentation")) {
    if (segments.size() == 1) {
      segments[0] = name;
    }
  }
}

//===========================================================================
// returns enabled flag
//===========================================================================
bool ccruncher::Segmentation::isEnabled() const
{
  return enabled;
}

//===========================================================================
// return filename
//===========================================================================
string ccruncher::Segmentation::getFilename(const string &path) const
{
  return File::normalizePath(path) + name + ".csv";
}

//===========================================================================
// add components to segmentation stats
//===========================================================================
void ccruncher::Segmentation::addComponent(int isegment)
{
  assert(0 <= isegment && isegment < (int)numcomponents.size());
  if (0 <= isegment && isegment < (int)numcomponents.size()) {
    numcomponents[isegment]++;
  }
}

//===========================================================================
// remove unused segments
//===========================================================================
void ccruncher::Segmentation::removeUnusedSegments()
{
  assert(segments.size() == numcomponents.size());

  int pos = 0;
  recode_map.clear();
  for(size_t i=0; i<segments.size(); i++)
  {
    if (numcomponents[i] == 0) {
      recode_map.push_back(-1);
      segments.erase(segments.begin()+i);
      numcomponents.erase(numcomponents.begin()+i);
      i--;
    }
    else {
      recode_map.push_back(pos);
      pos++;
    }
  }

  // moving unassigned segment at the end
  if (segments.size() > 1 && segments[0] == UNASSIGNED)
  {
    segments.push_back(UNASSIGNED);
    segments.erase(segments.begin());
    numcomponents.push_back(numcomponents[0]);
    numcomponents.erase(numcomponents.begin());
    recode_map[0] = pos;
    for(size_t i=0; i<recode_map.size(); i++) {
      if (recode_map[i] > 0) recode_map[i]--;
    }
  }
}

//===========================================================================
// recode segments removing unused segments
//===========================================================================
int ccruncher::Segmentation::recode(int isegment) const
{
  assert(0 <= isegment && isegment < (int)recode_map.size());
  return recode_map[isegment];
}

