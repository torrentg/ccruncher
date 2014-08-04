
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
#include "kernel/SimulatedData.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] obligor Obligor to use.
 */
ccruncher::SimulatedObligor::SimulatedObligor(Obligor *obligor)
{
  if (obligor != nullptr) {
    irating = static_cast<unsigned char>(obligor->irating);
    ifactor = static_cast<unsigned char>(obligor->ifactor);
    numassets = 0;
    ref.obligor = obligor;
  }
  else {
    irating = 0;
    ifactor = 0;
    numassets = 0;
    ref.obligor = nullptr;
  }
}

/**************************************************************************//**
 * @details Compare by factor and rating (in this order). This method can
 *          be used to sort SimulatedObligor.
 * @param[in] c Object to compare.
 * @return true = less than c, false = otherwise
 */
bool ccruncher::SimulatedObligor::operator<(const SimulatedObligor &c) const
{
  if (ifactor < c.ifactor)
  {
    return true;
  }
  else if (ifactor == c.ifactor)
  {
    if (irating < c.irating)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

/**************************************************************************/
ccruncher::SimulatedAsset::SimulatedAsset(Asset *asset) : begin(nullptr), end(nullptr)
{
  assign(asset);
}

/**************************************************************************/
ccruncher::SimulatedAsset::SimulatedAsset(const SimulatedAsset &o)
{
  *this = o;
}

/**************************************************************************/
ccruncher::SimulatedAsset::~SimulatedAsset()
{
  free();
}

/**************************************************************************/
ccruncher::SimulatedAsset& ccruncher::SimulatedAsset::operator=(const SimulatedAsset &o)
{
  mindate = o.mindate;
  maxdate = o.maxdate;
  if (o.begin == nullptr) {
    begin = nullptr;
    end = nullptr;
  }
  else {
    size_t len = o.end - o.begin;
    begin = new DateValues[len];
    end = begin + len;
    memcpy(begin, o.begin, len*sizeof(DateValues));
  }
  return *this;
}

/**************************************************************************//**
 * @details Initialize object content using asset data.
 * @param[in] asset Asset to use.
 */
void ccruncher::SimulatedAsset::assign(Asset *asset)
{
  if (begin != nullptr) {
    free();
  }

  if (asset == nullptr)
  {
    mindate = NAD;
    maxdate = NAD;
    begin = nullptr;
    end = nullptr;
  }
  else
  {
    mindate = asset->getMinDate();
    maxdate = asset->getMaxDate();

    size_t len = asset->getData().size();
    if (len > 0)
    {
      begin = new DateValues[len];
      end = begin + len;
      memcpy(begin, &(asset->getData()[0]), len*sizeof(DateValues));
    }
    else
    {
      begin = nullptr;
      end = nullptr;
    }
  }
}

/**************************************************************************//**
 * @details Deallocates memory pointed by begin.
 */
void ccruncher::SimulatedAsset::free()
{
  if (begin != nullptr)
  {
    assert(end != nullptr);
    delete [] begin;
    begin = nullptr;
    end = nullptr;
  }
}

