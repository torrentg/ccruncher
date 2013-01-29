
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

#ifndef _SimulationThread_
#define _SimulationThread_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "kernel/Inverses.hpp"
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

    // used by random_shuffle function
    struct frand
    {
      const gsl_rng *rng;
      frand(const gsl_rng *r) : rng(r) {}
      int operator()(int limit) const { return gsl_rng_uniform_int(rng, limit); }
    };

  private:

    // thread identifier
    int id;
    // Monte Carlo parent
    MonteCarlo &montecarlo;
    // list of simulated obligors
    const vector<SimulatedObligor> &obligors;
    // list of simulated assets
    const vector<SimulatedAsset> &assets;
    // segmentations indexes per asset
    const vector<unsigned short> &segments;
    // number of factors
    size_t numfactors;
    // degrees of freedom
    double ndf;
    // random number generator
    gsl_rng *rng;
    // cholesky matrix
    const gsl_matrix *chol;
    // factor loadings [w_i]
    const vector<double> &floadings1;
    // factor loadings [sqrt(1-w_i^2)]
    const vector<double> &floadings2;
    // inverse functions
    const Inverses &inverses;
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
    // lhs sample size
    size_t lhs_size;
    // lhs current trial
    size_t lhs_pos;
    // number of lhs samples
    size_t lhs_num;
    // lh sample multivariate normal (factors)
    vector<double> lhs_values_z;
    // lh sample chi square
    vector<double> lhs_values_s;
    // auxiliar vector
    vector<pair<double,size_t> > lhs_aux;
    // current simulated multivariate distribution
    vector<double> xvalues;

  private:

    // comparator used to obtain ranks
    static bool pcomparator(const pair<double,size_t> &o1, const pair<double,size_t> &o2);
    // generate random numbers
    void randomize() throw();
    // simule obligor
    void simule(int) throw();
    // returns the simulated value
    double getRandom(int iobligor) throw();
    // simulate a multivariate distribution (normal or t-student)
    void rmvdist();
    // chi-square random generation
    void rchisq();
    // factors random generation
    void rfactors();
    // non-copyable class
    SimulationThread(const SimulationThread &);
    // non-copyable class
    SimulationThread & operator=(const SimulationThread &);

  public:

    // constructor
    SimulationThread(int, MonteCarlo &, unsigned long seed);
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
