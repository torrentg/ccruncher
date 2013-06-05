
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

#ifndef _SimulatedData_
#define _SimulatedData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "portfolio/Obligor.hpp"
#include "portfolio/LGD.hpp"
#include "portfolio/DateValues.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class SimulatedAsset
{

  public:

    // minimum event date (restricted to simulation time horizon)
    Date mindate;
    // maximum event date (restricted to simulation time horizon)
    Date maxdate;
    // allocated datevalues
    DateValues *begin;
    // allocated datevalues
    DateValues *end;
    // segmentations indexes
    unsigned short segments;

};

//---------------------------------------------------------------------------

class SimulatedObligor
{

  public:

    // obligor's factor index
    unsigned char ifactor;
    // obligor's rating index
    unsigned char irating;
    // obligor's lgd
    LGD lgd;
    // number of assets
    unsigned short numassets;
    // pointer
    union
    {
      // obligor
      Obligor *obligor;
      // simulated assets
      void *assets;
    } ref;

  public:

    // constructor
    SimulatedObligor(Obligor *obligor=NULL);
    // less-than operator
    bool operator < (const SimulatedObligor &obj) const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
