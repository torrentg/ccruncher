
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#ifndef _MonteCarlo_
#define _MonteCarlo_

#include <mutex>
#include <string>
#include <vector>
#include <streambuf>
#include <gsl/gsl_matrix.h>
#include "kernel/Input.hpp"
#include "kernel/Inverse.hpp"
#include "params/CDF.hpp"
#include "params/Segmentation.hpp"
#include "portfolio/Obligor.hpp"
#include "utils/Date.hpp"
#include "utils/Logger.hpp"

namespace ccruncher {

// forward declarations
class SimulationThread;
class Aggregator;

/**************************************************************************//**
 * @brief Monte Carlo simulation.
 *
 * @details This object manages the Monte Carlo simulation. Efective
 *          simulation steps are done in the SimulationThread objects.
 *          Input object used to initialize this class can be removed just
 *          after this class initialization.
 *
 * @see SimulationThread
 */
class MonteCarlo
{

  private:

    //! Status types
    enum class status {
      fresh=0,       //!< Object not initialized
      initialized=1, //!< Object initializated
      running=2,     //!< Running simulation
      error=3,       //!< Error found
      stopped=4,     //!< Simulation stopped
      finished=5     //!< Simulation finished
    };

  private:

    //! Logger
    Logger logger;
    //! List of simulated obligors
    std::vector<Obligor> obligors;
    //! Number of segments for each segmentation
    std::vector<unsigned short> numSegmentsBySegmentation;
    //! Total number of segments (included in all segmentations)
    size_t numsegments;
    //! List of aggregators
    std::vector<Aggregator *> aggregators;
    //! Maximum number of iterations
    size_t maxiterations;
    //! Maximum execution time
    size_t maxseconds;
    //! Initial date
    Date time0;
    //! Ending date
    Date timeT;
    //! Degrees of freedom
    double ndf;
    //! Probability of default functions
    std::vector<CDF> dprobs;
    //! Inverse functions
    std::vector<Inverse> inverses;
    //! Cholesky matrix (factor correlations)
    gsl_matrix *chol;
    //! Factor loadings (w_i)
    std::vector<double> floadings1;
    //! Factor loadings (sqrt(1-w_i^2))
    std::vector<double> floadings2;
    //! Antithetic method flag
    bool antithetic;
    //! Block size
    unsigned short blocksize;
    //! RNG seed
    unsigned long seed;
    //! Hash (0=non show hashes) (default=0)
    size_t mHash;
    //! Simulation starting time
    std::chrono::steady_clock::time_point t1;
    //! Simulation threads
    std::vector<SimulationThread*> threads;
    //! Number of iterations done
    size_t numiterations;
    //! Number of finished threads
    size_t nfthreads;
    //! Ensures data consistence
    std::mutex mMutex;
    //! Stop flag
    bool *mStop;
    //! Object status
    status mStatus;

  private:
  
    //! Deallocate memory
    void freeMemory();
    //! Set simulation parameters
    void setParams(const Params &params);
    //! Set default probabilities (using default probabilities)
    void setDefaultProbabilities(const std::vector<CDF> &cdfs);
    //! Set factor loadings
    void setFactorLoadings(const std::vector<double> &loadings);
    //! Set correlation matrix
    void setCorrelations(const std::vector<std::vector<double>> &correlations);
    //! Set obligors' portfolio
    void setObligors(std::vector<Obligor> &obligors, const std::vector<Segmentation> &segmentations);
    //! Set segmentations
    void setSegmentations(const std::vector<Segmentation> &segmentations, const std::string &path, char mode);
    //! Create Finv(t(x)) spline functions
    void setInverses();
    //! Append simulation result
    bool append(const std::vector<std::vector<double>> &losses) noexcept;
    //! Computes the Cholesky matrix
    gsl_matrix* cholesky(const std::vector<std::vector<double>> &M);
    //! Averaged exposures by segment
    std::vector<double> getExposures(unsigned short isegmentation);

  public:

    //! Constructor
    MonteCarlo(std::streambuf *s=nullptr);
    //! Non-copyable class
    MonteCarlo(const MonteCarlo &) = delete;
    //! Non-copyable class
    MonteCarlo & operator=(const MonteCarlo &) = delete;
    //! Destructor
    ~MonteCarlo();

    //! Initiliaze this class
    void init(Input &data, const std::string &path, char mode);
    //! Execute Monte Carlo
    void run(unsigned char numthreads, size_t nhash=0, bool *stop=nullptr);

    //! Returns number of iterations done
    size_t getNumIterations() const;
    //! Returns maximum number of iterations to do
    size_t getMaxIterations() const;

  public:
  
    //! Friend class
    friend class SimulationThread;

};

} // namespace

#endif

