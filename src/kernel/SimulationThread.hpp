
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

#ifndef _SimulationThread_
#define _SimulationThread_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "kernel/Inverse.hpp"
#include "kernel/MonteCarlo.hpp"
#include "kernel/SimulatedData.hpp"
#include "utils/Date.hpp"
#include "utils/Timer.hpp"
#include "utils/Thread.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

// forward declaration
class MonteCarlo;

//---------------------------------------------------------------------------

class SimulationThread : public Thread
{

  private:

    // Monte Carlo parent
    MonteCarlo &montecarlo;
    // list of simulated obligors
    const vector<SimulatedObligor> &obligors;
    // list of simulated assets
    const char *assets;
    // asset size
    int assetsize;
    // random number generator
    gsl_rng *rng;
    // simulated factors
    gsl_vector *factors;
    // cholesky matrix
    const gsl_matrix *chol;
    // factor loadings
    const vector<double> &floadings;
    // inverse functions
    const Inverse &inverse;
    // initial date
    Date time0;
    // date where risk is computed
    Date timeT;
    // antithetic method flag
    bool antithetic;
    // management flag for antithetic method (default=false)
    bool reversed;
    // number of segmentations
    int numsegmentations;
    // asset loss values by segmentation
    vector<vector<double> > losses;
    // elapsed time creating random numbers
    Timer timer1;
    // ellapsed time simulating obligors & segmentations
    Timer timer2;
    // multivariate normal simulated values
    vector<double> uvalues;

  private:
  
    // generate random numbers
    void randomize() throw();
    // simule obligor
    void simule(int) throw();
    // returns the simulated value
    double getRandom(int ibobligor) throw();
    // simulate a multivariate normal
    void rmvnorm();
    // non-copyable class
    SimulationThread(const SimulationThread &);
    // non-copyable class
    SimulationThread & operator=(const SimulationThread &);

  public:

    // constructor
    SimulationThread(MonteCarlo &, unsigned long seed);
    // destructor
    ~SimulationThread();
    // thread main function
    void run();
    // returns ellapsed time creating random numbers
    double getEllapsedTime1();
    // returns ellapsed time simulating default times
    double getEllapsedTime2();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
