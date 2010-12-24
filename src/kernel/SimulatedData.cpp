
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#include <algorithm>
#include "kernel/SimulatedData.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulatedObligor::SimulatedObligor(Obligor *obligor)
{
  assert(obligor != NULL);
  irating = obligor->irating;
  ref = obligor;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulatedAsset::SimulatedAsset(Asset *asset, int iobligor_)
{
  assert(asset != NULL);
  iobligor = iobligor_;
  mindate = asset->getMinDate();
  maxdate = asset->getMaxDate();
  ref = asset;
}

//===========================================================================
// less-than operator
// sort SimulatedObligor by sector and rating
//===========================================================================
bool ccruncher::SimulatedObligor::operator < (const SimulatedObligor &c) const
{
  if (ref->isector < c.ref->isector)
  {
    return true;
  }
  else if (ref->isector == c.ref->isector)
  {
    if (ref->irating < c.ref->irating)
    {
      return true;
    }
    else if (ref->irating == c.ref->irating)
    {
      return false;
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
