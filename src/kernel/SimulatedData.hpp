
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

#include <vector>
#include "utils/config.h"
#include "portfolio/Obligor.hpp"
#include "portfolio/Asset.hpp"
#include "portfolio/DateValues.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class SimulatedAsset
{

  public:

    // minimum event date (restricted to simulation time horizon)
    Date mindate;
    // maximum event date (restricted to simulation time horizon)
    Date maxdate;
    // reference to asset data
    vector<DateValues>::const_iterator begin;
    // reference to asset data
    vector<DateValues>::const_iterator end;
    // segmentacions indexes
    int segments;

};

//---------------------------------------------------------------------------

class SimulatedObligor
{

  public:

    // obligor's rating index
    int irating;
    // reference to obligor
    Obligor *ref;
    // number of assets
    int numassets;
    // pointer to simulated assets
    SimulatedAsset *assets;

  public: 
    
    // constructor
    SimulatedObligor(Obligor *);
    // less-than operator
    bool operator < (const SimulatedObligor &obj) const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
