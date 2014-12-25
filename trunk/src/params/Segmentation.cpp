
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

#include <cstring>
#include <cassert>
#include "params/Segmentation.hpp"
#include "utils/Exception.hpp"
#include "utils/Utils.hpp"

#define UNASSIGNED "unassigned"

using namespace std;

/**************************************************************************/
ccruncher::Segmentation::Segmentation(const string &name, Type type, bool enabled, bool hasUnassigned) :
  mName(name), mType(type), mEnabled(enabled)
{
  if (hasUnassigned) {
    addSegment(UNASSIGNED); // adding catcher segment
  }
}

/**************************************************************************//**
 * @param[in] sname Segment name.
 * @return The index of the given segment.
 * @throw Exception Segment name not found.
 */
ushort ccruncher::Segmentation::indexOf(const std::string &sname) const
{
  return indexOf(sname.c_str());
}

/**************************************************************************//**
 * @param[in] sname Segment name.
 * @return The index of the given segment.
 * @throw Exception Segment name not found.
 */
ushort ccruncher::Segmentation::indexOf(const char *sname) const
{
  assert(sname != nullptr);
  for(size_t i=0; i<mSegments.size(); i++) {
    if (mSegments[i].compare(sname) == 0) {
      return (ushort) i;
    }
  }
  throw Exception("segment '" + string(sname) + "' not found");
}

/**************************************************************************//**
 * @details Check if the string is a valid segmentation or segment
 *          identifier:<br/>
 *          Name pattern: [a-zA-Z0-9+-._ ]+<br/>
 *          We need to check the segmentation name because it is used
 *          to create the simulation output files. we need to check the
 *          segments name because they appear in the CSV column headers
 *          and can be used as variable identifiers.
 * @param[in] str Segmentation name.
 * @return true = valid name, false = otherwise.
 */
bool ccruncher::Segmentation::isValidName(const std::string &str) const
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
void ccruncher::Segmentation::setType(const Type &type)
{
  mType = type;
}

/**************************************************************************//**
 * @param[in] type Segmentation type ('asset' or 'obligor').
 * @throw Exception Type not recognized.
 */
void ccruncher::Segmentation::setType(const std::string &type)
{
  if (type == "asset") {
    mType = Type::asset;
  }
  else if (type == "obligor") {
    mType = Type::obligor;
  }
  else {
    throw Exception("unrecognized components: '" + type + "'");
  }
}

/**************************************************************************//**
 * @param[in] b Indicates if this segmentation is enabled.
 */
void ccruncher::Segmentation::setEnabled(bool b)
{
  mEnabled = b;
}

/**************************************************************************//**
 * @param[in] sname Segment name.
 * @throw Exception Invalid name or segment name repeated.
 */
void ccruncher::Segmentation::addSegment(const std::string &sname)
{
  if (!isValidName(sname)) {
    throw Exception("invalid segment name '" + sname + "'");
  }

  // checking coherence
  for(string &segment : mSegments) {
    if (segment == sname) {
      throw Exception("segment '" + sname + "' repeated");
    }
  }

  // inserting value
  try
  {
    mSegments.push_back(sname);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

/**************************************************************************//**
 * @details Returns the name of the i-th segment. If the segmentation has
 *          only one segment(implicit segment), then the segment equates
 *          to segmentation name.
 * @param[in] i Segment index.
 * @return Segment name.
 */
const std::string& ccruncher::Segmentation::getSegment(ushort i) const
{
  assert(i < mSegments.size());
  if (mSegments.size() == 1) {
    return mName;
  }
  else {
    return mSegments[i];
  }
}

/**************************************************************************//**
 * @details Create filename concatenating path/name.csv.
 * @param[in] path Dir path.
 * @return File path.
 */
string ccruncher::Segmentation::getFilename(const string &path) const
{
  return Utils::normalizePath(path) + mName + ".csv";
}

