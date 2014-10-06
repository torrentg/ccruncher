
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

#include <climits>
#include <cassert>
#include "params/Segmentations.hpp"
#include "utils/Format.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] sname Segmentation name.
 * @return Index of the segmentation.
 * @throw Exception Segmentation not found.
 */
ushort ccruncher::Segmentations::indexOf(const std::string &sname) const
{
  return indexOf(sname.c_str());
}

/**************************************************************************//**
 * @param[in] sname Segmentation name.
 * @return Index of the segmentation.
 * @throw Exception Segmentation not found.
 */
ushort ccruncher::Segmentations::indexOf(const char *sname) const
{
  for(size_t i=0; i<this->size(); i++) {
    if ((*this)[i].getName().compare(sname) == 0) {
      return (ushort)i;
    }
  }
  throw Exception("segmentation '" + string(sname) + "' not found");
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Segmentations::epstart(ExpatUserData &, const char *tag, const char **attributes)
{
  if (isEqual(tag,"segmentations")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag segmentations");
    }
  }
  else if (isEqual(tag,"segmentation")) {
    if (getNumAttributes(attributes) < 2 || 3 < getNumAttributes(attributes)) {
      throw Exception("incorrect number of attributes in tag segmentation");
    }
    string name = getStringAttribute(attributes, "name");
    bool enabled = getBooleanAttribute(attributes, "enabled", true);
    string components = getStringAttribute(attributes, "components");
    Segmentation segmentation(name);
    segmentation.setEnabled(enabled);
    segmentation.setType(components);
    this->push_back(segmentation);
  }
  else if (isEqual(tag,"segment")) {
    if (this->empty()) {
      throw Exception("segment without segmentation");
    }
    string name = getStringAttribute(attributes, "name");
    this->back().addSegment(name);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 */
void ccruncher::Segmentations::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"segmentation")) {
    // nothing to do
  }
  else if (isEqual(tag,"segmentations")) {
    isValid(*this, true);
  }
}

/**************************************************************************//**
 * @details Performs the following checks:
 *          - non-empty
 *          - size less than 65535
 *          - non duplicated segmentation names
 *          - 0 or 1 basic (only 1 segment) segmentation.
 * @param[in] segmentations List of segmentations.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Validation error.
 */
bool ccruncher::Segmentations::isValid(const std::vector<Segmentation> &segmentations, bool throwException)
{
  try
  {
    if (segmentations.empty()) {
      throw Exception("segmentations not found");
    }
    if (segmentations.size() > USHRT_MAX) {
      throw Exception("number of segmentations bigger than " + Format::toString(USHRT_MAX));
    }

    // check for duplicated elements
    for(size_t i=0; i<segmentations.size(); i++) {
      for(size_t j=i+1; j<segmentations.size(); j++) {
        if (segmentations[i].getName() == segmentations[j].getName()) {
          throw Exception("segmentation name '" + segmentations[i].getName() + "' repeated");
        }
      }
    }

    // check that only exist 0 or 1 'portfolio' segmentation
    /*
    string str;
    int nbasic = 0;
    for(const Segmentation &segmentation : segmentations) {
      if (segmentation.size() == 1) {
        str += (nbasic>0?", ":"") + segmentation.getName();
        nbasic++;
      }
    }
    if (nbasic > 1) {
      throw Exception("found duplicated segmentations [" + str + "]");
    }
    */

    return true;
  }
  catch(Exception &)
  {
    if (throwException) throw;
    else return false;
  }
}

/**************************************************************************//**
 * @param[in] segmentations List of segmentations.
 */
//! Returns the number of enabled segmentations
ushort ccruncher::Segmentations::numEnabledSegmentations(const std::vector<Segmentation> &segmentations)
{
  ushort ret = 0;
  for(const Segmentation &segmentation : segmentations) {
    if (segmentation.isEnabled()) {
      ret ++;
    }
  }
  return ret;
}

