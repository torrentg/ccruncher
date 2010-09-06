
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

#ifndef _SimulatedData_
#define _SimulatedData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include "portfolio/Borrower.hpp"
#include "portfolio/Asset.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class SimulatedBorrower
{

  public:

    // borrower's rating index
    int irating;
    // current simulated copula value
    double rvalue;
    // current simulated default time
    Date dtime;
    // reference to borrower
    Borrower *ref;

  public: 
    
    // constructor
    SimulatedBorrower(Borrower *);
    // less-than operator
    bool operator < (const SimulatedBorrower &obj) const;

};

//---------------------------------------------------------------------------

class SimulatedAsset
{

  public:

    // minimum event date (restricted to simulation time horizon)
    Date mindate;
    // maximum event date (restricted to simulation time horizon)
    Date maxdate;
    // borrower index (see borrowers vector)
    int iborrower;
    // simulated loss
    double loss;
    // reference to asset
    Asset *ref;
    
  public:
  
    // constructor
    SimulatedAsset(Asset *, int);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
