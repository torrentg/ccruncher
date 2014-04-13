
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

namespace ccruncher {

// forward declaration
class MonteCarlo;

/**************************************************************************//**
 * @brief Monte Carlo simulation
 *
 * @details This class does the following tasks:
 *          - RNG management
 *          - RNG gaussian pool (performance reasons)
 *          - Blocksize (simultaneous simulations, performance reasons)
 *          - Latin Hypercube Sampling management (LHS)
 *          - Antithetic management
 *          - Simulate obligors default times
 *          - Simulate asset losses
 *          - Losses aggregation (by segmentation)
 *          Simulation parameters are constants and shared with other
 *          threads.
 *
 * @see MonteCarlo
 *
 * @note Note on LHS method
 *       We allow LHS variance reduction technique only on random variables
 *       S and Z. We don't do LHS in X (all the obligors) because:
 *       - we can exhaust memory when num_obligors and lhs_size  are high
 *       - lhs with dependence is ineficient (beta eval)
 *       - if we use quick version (0.5 instead of beta) gives discrete values
 *       Observe that lhs_size=1 means that LHS is disabled.
 *
 * @note Variance reduction papers
 *       - 'Monte Carlo methods for security pricing'
 *         by Phelim Boyle, Mark Broadie, Paul Glasserman
 *       - 'A user's guide to LHS: Sandia's Latin Hypercube Sampling Software'
 *         by Gregory D. Wyss, Kelly H. Jorgensen
 *       - 'Latin hypercube sampling with dependence and applications in finance'
 *         by Natalie Packham, Wolfgang Schmidt
 */
class SimulationThread : public Thread
{

  private:

    /**
     * @brief Internal struct (RNG functor)
     * @details Used by random_shuffle function
     */
    struct frand
    {
      //! RNG object
      const gsl_rng *rng;
      //! Constructor
      frand(const gsl_rng *r) : rng(r) {}
      //!  Return a non-negative value less than its argument
      int operator()(int limit) const { return gsl_rng_uniform_int(rng, limit); }
    };

  private:

    //! Monte Carlo parent
    MonteCarlo &montecarlo;
    //! List of simulated obligors
    const std::vector<SimulatedObligor> &obligors;
    //! List of simulated assets
    const std::vector<SimulatedAsset> &assets;
    //! List of obligor-segments
    const std::vector<unsigned short> &segments;
    //! Number of segments for each segmentation
    const std::vector<unsigned short> &numSegmentsBySegmentation;
    //! Cholesky matrix (see MonteCarlo::initModel())
    const gsl_matrix *chol;
    //! Factor loadings [w_i]
    const std::vector<double> &floadings1;
    //! Factor loadings [sqrt(1-w_i^2)]
    const std::vector<double> &floadings2;
    //! Inverse functions
    const Inverses &inverses;
    //! Number of factors
    const size_t &numfactors;
    //! Degrees of freedom
    const double &ndf;
    //! starting simulation date
    const Date &time0;
    //! ending simulation date
    const Date &timeT;
    //! Antithetic method flag
    const bool &antithetic;
    //! Total number of segments
    const size_t &numsegments;
    //! Block size
    const unsigned short &blocksize;
    //! Lhs sample size
    const unsigned short &lhs_size;

    //! Thread identifier
    int id;

    //! Random number generator
    gsl_rng *rng;
    //! Gaussian random values pool
    std::vector<double> rngpool;
    //! Current position in rngpool
    size_t rngpool_pos;

    //! Asset loss values
    std::vector<double> losses;
    //! Simulated factors
    std::vector<double> z;
    //! Simulated Chi-square values
    std::vector<double> s;
    //! Gaussian/T-Student values
    std::vector<double> x;
    //! Indexes
    std::vector<short> indexes;

    //! Lhs current trial
    size_t lhs_pos;
    //! Number of lhs samples
    size_t lhs_num;
    //! Lhs sample multivariate normal (factors)
    std::vector<double> lhs_values_z;
    //! Lhs sample chi square
    std::vector<double> lhs_values_s;
    //! Auxiliar vector
    std::vector<std::pair<double,size_t>> lhs_aux;

    //! Elapsed time creating random numbers
    Timer timer1;
    //! Elapsed time simulating obligors & segmentations
    Timer timer2;
    //! Elapsed time writing to disk
    Timer timer3;

  private:

    //! Comparator used to obtain ranks
    static bool pcomparator(const std::pair<double,size_t> &, const std::pair<double,size_t> &);

    //! Simule latent variables
    void simuleLatentVars();
    //! Simule obligor
    void simuleObligorLoss(const SimulatedObligor &, Date, const SimulatedAsset *,
                           const unsigned short *, double *) const throw();
    //! Chi-square random generation
    void rchisq();
    //! Factors random generation
    void rmvnorm();
    //! Returns a univariate gaussian from rngpool
    double rnorm();
    //! Fills gaussian variates pool
    void fillGaussianPool();
    //! Non-copyable class
    SimulationThread(const SimulationThread &);
    //! Non-copyable class
    SimulationThread & operator=(const SimulationThread &);

  public:

    //! Constructor
    SimulationThread(int, MonteCarlo &, unsigned long seed);
    //! Destructor
    ~SimulationThread();
    //! Thread main function
    virtual void run() override;
    //! Returns elapsed time creating random numbers
    double getElapsedTime1();
    //! Returns elapsed time simulating default times
    double getElapsedTime2();
    //! Returns elapsed time writting to disk
    double getElapsedTime3();

};

} // namespace

#endif

