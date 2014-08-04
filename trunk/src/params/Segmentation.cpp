
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#include <cassert>
#include "params/Segmentation.hpp"
#include "utils/File.hpp"

#define UNASSIGNED "unassigned"

using namespace std;

/**************************************************************************/
ccruncher::Segmentation::Segmentation(const string &name, ComponentsType type, bool enabled) :
  mType(type), mEnabled(enabled)
{
  mName = name; // bypassing name validation (eg. allows void name '')
  add(UNASSIGNED); // adding catcher segment
}

/**************************************************************************//**
 * @details Set default values that are valids. Defines the UNASSIGNED
 *          segment.
 */
void ccruncher::Segmentation::reset()
{
  mName = "";
  mType = obligor;
  mEnabled = true;
  mSegments.clear();
  mNumElements.clear();
  mRecodeMap.clear();
  add(UNASSIGNED); // adding catcher segment
}

/**************************************************************************//**
 * @param[in] sname Segment name.
 * @return The index of the given segment.
 * @throw Exception Segment name not found.
 */
size_t ccruncher::Segmentation::indexOf(const std::string &sname) const
{
  return indexOf(sname.c_str());
}

/**************************************************************************//**
 * @param[in] sname Segment name.
 * @return The index of the given segment.
 * @throw Exception Segment name not found.
 */
size_t ccruncher::Segmentation::indexOf(const char *sname) const
{
  assert(sname != nullptr);

  for(size_t i=0; i<mSegments.size(); i++)
  {
    if (mSegments[i].compare(sname) == 0)
    {
      return i;
    }
  }

  throw Exception("segment '" + string(sname) + "' not found");
}

/**************************************************************************//**
 * @details Check if the string is a valid segmentation or segment
 *          identifier:<br/>
 *          Name pattern: [a-zA-Z0-9+-._]+<br/>
 *          We need to check the segmentation name because it is used
 *          to create the simulation output files. we need to check the
 *          segments name because they appear in the CSV column headers
 *          and can be used as variable identifiers.
 * @param[in] str Segmentation name.
 * @return true = valid name, false = otherwise.
 */
bool ccruncher::Segmentation::isValidName(const std::string &str)
{
  if (str.length() == 0) return false;
  for(size_t i=0; i<str.length(); i++) {
    if (!isalnum(str[i]) && strchr("+-._ ",str[i]) == nullptr) {
      return false;
    }
  }
  return true;
}

/**************************************************************************//**
 * @param[in] name Segmentation name.
 */
void ccruncher::Segmentation::setName(const std::string &name)
{
  if (!isValidName(name)) {
    throw Exception("invalid segmentation name '" + name + "'");
  }
  mName = name;
}

/**************************************************************************//**
 * @param[in] type Segmentation type.
 */
void ccruncher::Segmentation::setType(const ComponentsType &type)
{
  mType = type;
}

/**************************************************************************//**
 * @param[in] sname Segment name.
 * @throw Exception Invalid name or segment name repeated.
 */
void ccruncher::Segmentation::add(const std::string &sname)
{
  if (!isValidName(sname)) {
    throw Exception("invalid segment name '" + sname + "'");
  }

  // checking coherence
  for(string &segment : mSegments)
  {
    if (segment == sname)
    {
      throw Exception("segment '" + sname + "' repeated");
    }
  }

  // inserting value
  try
  {
    mSegments.push_back(sname);
    mNumElements.push_back(0);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Segmentation::epstart(ExpatUserData &, const char *tag, const char **attributes)
{
  if (isEqual(tag,"segmentation")) {
    if (getNumAttributes(attributes) < 2 || 3 < getNumAttributes(attributes)) {
      throw Exception("incorrect number of attributes in tag segmentation");
    }
    mName = getStringAttribute(attributes, "name");
    if (!isValidName(mName)) {
      throw Exception("segmentation name '" + mName + "' is not valid");
    }
    mEnabled = getBooleanAttribute(attributes, "enabled", true);
    string strcomp = getStringAttribute(attributes, "components");
    // filling components variable
    if (strcomp == "asset") {
      mType = asset;
    }
    else if (strcomp == "obligor") {
      mType = obligor;
    }
    else {
      throw Exception("unrecognized components attribute: '" + strcomp + "'");
    }
  }
  else if (isEqual(tag,"segment")) {
    string sname = getStringAttribute(attributes, "name");
    add(sname);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 */
void ccruncher::Segmentation::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"segmentation")) {
    if (mSegments.size() == 1) {
      mSegments[0] = mName;
    }
  }
}

/**************************************************************************//**
 * @details Create filename concatenating path/name.csv.
 * @param[in] path Dir path.
 * @return File path.
 */
string ccruncher::Segmentation::getFilename(const string &path) const
{
  return File::normalizePath(path) + mName + ".csv";
}

/**************************************************************************//**
 * @details Adds a component to the i-th segment. Thus reports that segment
 *          is used by someone and don't be removed by method
 *          Segmentation::removeUnusedSegments().
 * @param[in] isegment Segment index.
 */
void ccruncher::Segmentation::addComponent(size_t isegment)
{
  assert(isegment < mNumElements.size());
  if (isegment < mNumElements.size()) {
    mNumElements[isegment]++;
  }
}

/**************************************************************************//**
 * @details Removes unused segments (recoding indexes when necessary) in
 *          order to minimize the memory allocation/access in the
 *          simulation stage. Before call this method you need to inform
 *          about segment usage calling method
 *          Segmentation::addComponent(int isegment) for each component.
 */
void ccruncher::Segmentation::recode()
{
  assert(mSegments.size() == mNumElements.size());

  int pos = 0;
  mRecodeMap.clear();

  // removing unused segments
  for(size_t i=0; i<mSegments.size(); i++)
  {
    if (mNumElements[i] == 0) {
      mRecodeMap.push_back(SIZE_MAX);
      mSegments.erase(mSegments.begin()+i);
      mNumElements.erase(mNumElements.begin()+i);
      i--;
    }
    else {
      mRecodeMap.push_back(pos);
      pos++;
    }
  }

  // moving unassigned segment to the end
  if (mSegments.size() > 1 && mSegments[0] == UNASSIGNED)
  {
    mSegments.push_back(UNASSIGNED);
    mSegments.erase(mSegments.begin());
    mNumElements.push_back(mNumElements[0]);
    mNumElements.erase(mNumElements.begin());
    mRecodeMap[0] = pos;
    for(size_t i=0; i<mRecodeMap.size(); i++) {
      if (mRecodeMap[i] != 0 && mRecodeMap[i]!= SIZE_MAX) mRecodeMap[i]--;
    }
  }
}

/**************************************************************************//**
 * @details This method is used to recode assets' segments.
 * @param[in] isegment Segment index.
 * @return New segment index.
 * @exception Trying to recode an unused segment.
 */
size_t ccruncher::Segmentation::recode(size_t isegment) const
{
  assert(isegment < mRecodeMap.size());
  if (isegment >= mRecodeMap.size() || mRecodeMap[isegment] == SIZE_MAX) {
    throw Exception("trying to recode an unused segment");
  }
  return mRecodeMap[isegment];
}

