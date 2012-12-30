
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include "kernel/SimulatedData.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulatedObligor::SimulatedObligor(Obligor *obligor)
{
  if (obligor != NULL) {
    irating = static_cast<unsigned char>(obligor->irating);
    ifactor = static_cast<unsigned char>(obligor->ifactor);
    recovery = obligor->recovery;
  }
  else {
    irating = 0;
    ifactor = 0;
    //recovery = Recovery();
  }
  numassets = 0;
  ref.obligor = obligor;
}

//===========================================================================
// less-than operator
// sort SimulatedObligor by factor and rating
// caution: ref contains obligor, not assets
//===========================================================================
bool ccruncher::SimulatedObligor::operator < (const SimulatedObligor &c) const
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

