
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

    // Monte Carlo parent
    MonteCarlo &montecarlo;
    // list of simulated obligors
    const std::vector<SimulatedObligor> &obligors;
    // number of segments for each segmentation
    const std::vector<unsigned short> &numSegmentsBySegmentation;
    // cholesky matrix
    const gsl_matrix *chol;
    // factor loadings [w_i]
    const std::vector<double> &floadings1;
    // factor loadings [sqrt(1-w_i^2)]
    const std::vector<double> &floadings2;
    // inverse functions
    const Inverses &inverses;

    // thread identifier
    int id;
    // random number generator
    gsl_rng *rng;
    // number of factors
    size_t numfactors;
    // degrees of freedom
    double ndf;
    // initial date
    Date time0;
    // date where risk is computed
    Date timeT;
    // antithetic method flag
    bool antithetic;
    // total number of segments
    size_t numsegments;
    // asset size
    size_t assetsize;
    // block size
    unsigned short blocksize;
    // asset loss values
    std::vector<double> losses;
    // factors values
    std::vector<double> z;
    // chi square factors
    std::vector<double> s;
    // multivariate values
    std::vector<double> x;
    // indexes
    std::vector<short> indexes;

    // lhs sample size
    size_t lhs_size;
    // lhs current trial
    size_t lhs_pos;
    // number of lhs samples
    size_t lhs_num;
    // lhs sample multivariate normal (factors)
    std::vector<double> lhs_values_z;
    // lhs sample chi square
    std::vector<double> lhs_values_s;
    // auxiliar vector
    std::vector<std::pair<double,size_t> > lhs_aux;

    // gaussian random values pool
    std::vector<double> rngpool;
    // current position in rnorm pool
    size_t rngpool_pos;

    // elapsed time creating random numbers
    Timer timer1;
    // elapsed time simulating obligors & segmentations
    Timer timer2;
    // elapsed time writing to disk
    Timer timer3;

  private:

    // comparator used to obtain ranks
    static bool pcomparator(const std::pair<double,size_t> &o1, const std::pair<double,size_t> &o2);
    // simule latent variables
    void simuleLatentVars();
    // simule obligor
    void simuleObligorLoss(const SimulatedObligor &obligor, Date, double *) const throw();
    // chi-square random generation
    void rchisq();
    // factors random generation
    void rmvnorm();
    // returns a uniform gaussian variate
    double rnorm();
    // fills gaussian variates pool
    void fillGaussianPool();
    // non-copyable class
    SimulationThread(const SimulationThread &);
    // non-copyable class
    SimulationThread & operator=(const SimulationThread &);

  public:

    // constructor
    SimulationThread(int, MonteCarlo &, unsigned long seed, unsigned short);
    // destructor
    ~SimulationThread();
    // thread main function
    void run();
    // returns elapsed time creating random numbers
    double getElapsedTime1();
    // returns elapsed time simulating default times
    double getElapsedTime2();
    // returns elapsed time writting to disk
    double getElapsedTime3();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
