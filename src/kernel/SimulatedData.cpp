
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
ccruncher::SimulatedBorrower::SimulatedBorrower(Borrower *borrower)
{
  assert(borrower != NULL);
  irating = borrower->irating;
  dtime = Date();
  ref = borrower;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulatedAsset::SimulatedAsset(Asset *asset, int iborrower_)
{
  assert(asset != NULL);
  iborrower = iborrower_;
  mindate = asset->getMinDate();
  maxdate = asset->getMaxDate();
  loss = 0.0;
  ref = asset;
}

//===========================================================================
// less-than operator
// sort SimulatedBorrower by sector and rating
//===========================================================================
bool ccruncher::SimulatedBorrower::operator < (const SimulatedBorrower &c) const
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
