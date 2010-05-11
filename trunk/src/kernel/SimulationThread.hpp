
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

#ifndef _SimulationThread_
#define _SimulationThread_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include "survival/Survival.hpp"
#include "kernel/MonteCarlo.hpp"
#include "kernel/SimulatedData.hpp"
#include "math/Copula.hpp"
#include "utils/Date.hpp"
#include "utils/Thread.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class SimulationThread : public Thread
{

  private:

    // Monte Carlo parent
    MonteCarlo &montecarlo;
    // list of simulated borrowers
    vector<SimulatedBorrower> borrowers;
    // list of simulated assets
    vector<SimulatedAsset> assets;
    // initial date
    Date time0;
    // date where risk is computed
    Date timeT;
    // survival functions
    Survival survival;
    // copula used to simulate correlations
    Copula *copula;
    // antithetic method flag
    bool antithetic;
    // management flag for antithetic method (default=false)
    bool reversed;
    // number of segmentations
    int numsegmentations;
    // asset segment indexes by segmentation
    vector<vector<int> > isegments;
    // asset loss values by segmentation
    vector<vector<double> > losses;

  private:
  
    // generate random numbers
    void randomize();
    // simulate default times
    void simulate();
    // evalue portfolio
    void evalue();
    // aggregate value
    void aggregate();
    // transfer data
    bool notify();
    // simulate default time
    Date simTimeToDefault(int iborrower);
    // returns the copula value
    double getRandom(int iborrower);

  public:

    // constructor
    SimulationThread(MonteCarlo &, long);
    // destructor
    ~SimulationThread();
    // thread main function
    void run();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
