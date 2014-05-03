
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
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
#include "params/Segmentations.hpp"
#include "portfolio/Asset.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @return Number of enabled segmentations
 */
int ccruncher::Segmentations::size() const
{
  return enabled.size();
}

/**************************************************************************//**
 * @details Index with negative values (i<0) refers to disabled
 *          segmentations. Index with positive values (>=0) refers to
 *          enabled segmentations.
 * @param[in] i Index of the segmentation.
 * @return Requested segmentation.
 */
const Segmentation& ccruncher::Segmentations::getSegmentation(int i) const
{
  if (i >= 0) {
    assert(i < (int) enabled.size());
    return enabled[i];
  }
  else {
    assert(-1-i < (int) disabled.size());
    return disabled[-1-i];
  }
}

/**************************************************************************//**
 * @param[in] sname Segmentation name.
 * @return Index of the segmentation (negative if disabled, positive otherwise).
 * @throw Exception Segmentation not found.
 */
int ccruncher::Segmentations::indexOfSegmentation(const std::string &sname) const
{
  return indexOfSegmentation(sname.c_str());
}

/**************************************************************************//**
 * @param[in] sname Segmentation name.
 * @return Index of the segmentation (negative if disabled, positive otherwise).
 * @throw Exception Segmentation not found.
 */
int ccruncher::Segmentations::indexOfSegmentation(const char *sname) const
{
  for(size_t i=0; i<enabled.size(); i++)
  {
    if (enabled[i].name.compare(sname) == 0)
    {
      return (int)i;
    }
  }
  for(size_t i=0; i<disabled.size(); i++)
  {
    if (disabled[i].name.compare(sname) == 0)
    {
      return -(int)(i+1);
    }
  }
  throw Exception("segmentation '" + string(sname) + "' not found");
}

/**************************************************************************//**
 * @details Check that exist enabled segmentations. Also checks that at
 *          most there is only one basic segmentation (segmentation with
 *          only the unassigned segment). In this case results are the same
 *          regardless of the type of components.
 * @throw Exception Validation error.
 */
void ccruncher::Segmentations::validate()
{
  if (enabled.empty()) {
    throw Exception("active segmentations not found");
  }

  string str;
  int nbasic = 0;
  for(Segmentation &segmentation : enabled) {
    if (segmentation.size() == 1) {
      str += (nbasic>0?", ":"") + segmentation.name;
      nbasic++;
    }
  }
  for(Segmentation &segmentation : disabled) {
    if (segmentation.size() == 1) {
      str += (nbasic>0?", ":"") + segmentation.name;
      nbasic++;
    }
  }
  if (nbasic > 1)
  {
    throw Exception("found duplicated segmentations [" + str + "]");
  }
}

/**************************************************************************//**
 * @param[in] val Segmentation to insert.
 * @throw Exception Repeated segmentation name.
 */
int ccruncher::Segmentations::insertSegmentation(Segmentation &val)
{
  // checking coherence
  for(Segmentation &segmentation : enabled)
  {
    if (segmentation.name == val.name)
    {
      throw Exception("segmentation name '" + val.name + "' repeated");
    }
  }
  for(Segmentation &segmentation : disabled)
  {
    if (segmentation.name == val.name)
    {
      throw Exception("segmentation name '" + val.name + "' repeated");
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

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Segmentations::epstart(ExpatUserData &eu, const char *tag, const char **attributes)
{
  if (isEqual(tag,"segmentations")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes are not allowed in tag segmentations");
    }
  }
  else if (isEqual(tag,"segmentation")) {
    auxsegmentation.reset();
    eppush(eu, &auxsegmentation, tag, attributes);
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
    insertSegmentation(auxsegmentation);
  }
  else if (isEqual(tag,"segmentations")) {
    validate();
    auxsegmentation.reset();
  }
}

/**************************************************************************//**
 * @details Calls method Segmentation::addComponent() for each segment
 *          adscribed to the given asset.
 * @param[in] asset Asset.
 */
void ccruncher::Segmentations::addComponents(const Asset *asset)
{
  assert(asset != nullptr);
  for(size_t i=0; i<enabled.size(); i++)
  {
    enabled[i].addComponent(asset->getSegment(i));
  }
}

/**************************************************************************//**
 * @details Calls method Segmentation::removeUnusedSegments() for each
 *          enabled segmentation.
 */
void ccruncher::Segmentations::removeUnusedSegments()
{
  for(Segmentation &segmentation : enabled)
  {
    segmentation.removeUnusedSegments();
  }
}

/**************************************************************************//**
 * @details Calls method Segmentation::recodeSegments() for each segment
 *          adscribed to the given asset.
 */
void ccruncher::Segmentations::recodeSegments(Asset *asset)
{
  assert(asset != nullptr);
  for(size_t i=0; i<enabled.size(); i++)
  {
    int old = asset->getSegment(i);
    asset->setSegment(i, enabled[i].recode(old));
  }

}

