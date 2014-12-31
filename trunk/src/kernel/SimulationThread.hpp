
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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
#include "kernel/Inverse.hpp"
#include "kernel/MonteCarlo.hpp"
#include "utils/Date.hpp"
#include "utils/Thread.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

// forward declaration
class MonteCarlo;

/**************************************************************************//**
 * @brief Monte Carlo simulation
 *
 * @details This class does the following tasks:
 *          - Blocksize (simultaneous simulations, performance reasons)
 *          - Antithetic management
 *          - Simulate obligors default times
 *          - Simulate asset losses
 *          - Losses aggregation (by segmentation)
 *          Simulation parameters are constants and shared with other
 *          threads.
 *
 * @see MonteCarlo
 */
class SimulationThread : public Thread
{

  private:

    //! Monte Carlo parent
    MonteCarlo &montecarlo;
    //! List of simulated obligors
    const std::vector<Obligor> &obligors;
    //! Number of segments for each segmentation
    const std::vector<ushort> &numSegmentsBySegmentation;
    //! Cholesky matrix (see MonteCarlo::initModel())
    const gsl_matrix *chol;
    //! Factor loadings [w_i]
    const std::vector<double> &floadings1;
    //! Factor loadings [sqrt(1-w_i^2)]
    const std::vector<double> &floadings2;
    //! Inverse functions
    const std::vector<Inverse> &inverses;
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
    const ushort &blocksize;

    //! Random number generator
    gsl_rng *rng;

  private:

    //! Returns the j-th component of x taking into account the antithetic mode
    double getValue(const std::vector<double> &x, size_t j);
    //! Simule obligor
    void simuleObligorLoss(const Obligor &obligor, Date dtime, std::vector<double> &losses) const noexcept;
    //! Chi-square random generation
    void rchisq(std::vector<double> &s);
    //! Factors random generation
    void rmvnorm(std::vector<std::vector<double>> &z);
    //! Non-copyable class
    SimulationThread(const SimulationThread &);
    //! Non-copyable class
    SimulationThread & operator=(const SimulationThread &);

  public:

    //! Constructor
    SimulationThread(MonteCarlo &, ulong seed);
    //! Destructor
    virtual ~SimulationThread() override;
    //! Thread main function
    virtual void run() override;

};

} // namespace

#endif

