
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#ifndef _MonteCarlo_
#define _MonteCarlo_

#include <mutex>
#include <string>
#include <vector>
#include <streambuf>
#include "kernel/IData.hpp"
#include "kernel/Inverse.hpp"
#include "params/CDF.hpp"
#include "params/Segmentation.hpp"
#include "params/Transitions.hpp"
#include "portfolio/Obligor.hpp"
#include "utils/Date.hpp"
#include "utils/Timer.hpp"
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
 *          IData object used to initialize this class can be removed just
 *          after this class initialization.
 *          Caution: assets is the list of simulated asset. The size of these
 *          objects depends on the number of segmentations and its size is
 *          fixed at execution time. We do it this way for performance
 *          reasons (to avoid a dereference).
 *
 * @see SimulationThread
 */
class MonteCarlo
{

  private:

    //! Logger
    Logger logger;
    //! List of simulated obligors
    std::vector<Obligor> obligors;
    //! Number of segments for each segmentation
    std::vector<ushort> numSegmentsBySegmentation;
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
    ushort blocksize;
    //! RNG seed
    ulong seed;
    //! Hash (0=non show hashes) (default=0)
    size_t mHash;
    //! Simulation elapsed time
    Timer timer;
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

  private:
  
    //! Deallocate memory
    void freeMemory();
    //! Averaged exposures by segment
    std::vector<double> getSegmentationExposures(ushort isegmentation, ushort numSegments) const;
    //! Append simulation result
    bool append(const double *) noexcept;
    //! Non-copyable class
    MonteCarlo(const MonteCarlo &);
    //! Non-copyable class
    MonteCarlo & operator=(const MonteCarlo &);

  public:

    //! Constructor
    MonteCarlo(std::streambuf *s=nullptr);
    //! Destructor
    ~MonteCarlo();

    //! Set simulation parameters
    void setParams(const std::map<std::string,std::string> &parameters);
    //! Set default probabilities (using default probabilities)
    void setDefaultProbabilities(const std::vector<CDF> &dprobs);
    //! Set default probabilities (using transition matrix)
    double setDefaultProbabilities(const Transitions &transitions);
    //! Set factor loadings
    void setFactorLoadings(const std::vector<double> &loadings);
    //! Set correlation matrix
    void setCorrelations(const std::vector<std::vector<double>> &correlations);
    //! Set obligors' portfolio
    void setObligors(std::vector<Obligor> &obligors);
    //! Set segmentations
    void setSegmentations(const std::vector<Segmentation> &segmentations, const std::string &path, char mode);

    //! Initiliaze this class
    void setData(IData &data, const std::string &path, char mode);
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

