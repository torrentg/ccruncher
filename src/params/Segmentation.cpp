
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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
#include <limits>
#include <cassert>
#include "params/Segmentation.hpp"
#include "utils/Exception.hpp"
#include "utils/Utils.hpp"

#define UNASSIGNED "unassigned"

using namespace std;

/**************************************************************************//**
 * @param[in] name Segmentation name.
 * @param[in] enabled Segmentation enabled flag.
 * @param[in] hasUnassigned Create implicit unassigned segment flag.
 * @throw Exception Invalid segmentations name.
 */
 ccruncher::Segmentation::Segmentation(const string &name, bool enabled, bool hasUnassigned) : mEnabled(enabled)
{
  setName(name);
  if (hasUnassigned) {
    addSegment(UNASSIGNED);
  }
}

/**************************************************************************//**
 * @param[in] sname Segment name.
 * @return The index of the given segment.
 * @throw Exception Segment name not found.
 */
unsigned short ccruncher::Segmentation::indexOfSegment(const std::string &sname) const
{
  return indexOfSegment(sname.c_str());
}

/**************************************************************************//**
 * @param[in] sname Segment name.
 * @return The index of the given segment.
 * @throw Exception Segment name not found.
 */
unsigned short ccruncher::Segmentation::indexOfSegment(const char *sname) const
{
  assert(sname != nullptr);
  for(size_t i=0; i<mSegments.size(); i++) {
    if (mSegments[i].compare(sname) == 0) {
      return static_cast<unsigned short>(i);
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

  // cheking the number of segments
  if (mSegments.size() >= numeric_limits<unsigned short>::max()) {
    throw Exception("number of segments bigger than " + to_string(numeric_limits<unsigned short>::max()));
  }

  // inserting value
  mSegments.push_back(sname);
}

/**************************************************************************//**
 * @details Returns the name of the i-th segment. If the segmentation has
 *          only one segment(implicit segment), then the segment equates
 *          to segmentation name.
 * @param[in] i Segment index.
 * @return Segment name.
 */
const std::string& ccruncher::Segmentation::getSegment(unsigned short i) const
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
  return Utils::realpath(path) + Utils::pathSeparator + mName + ".csv";
}

