
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

#ifndef _MonteCarlo_
#define _MonteCarlo_

#include <vector>
#include <fstream>
#include <streambuf>
#include <pthread.h>
#include "kernel/IData.hpp"
#include "kernel/Inverses.hpp"
#include "kernel/Aggregator.hpp"
#include "kernel/SimulatedData.hpp"
#include "kernel/SimulationThread.hpp"
#include "utils/Date.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

// forward declaration
class SimulationThread;

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
    Logger log;
    //! List of simulated obligors
    std::vector<SimulatedObligor> obligors;
    //! List of simulated assets
    std::vector<SimulatedAsset> assets;
    //! List of obligor-segments
    std::vector<unsigned short> segments;
    //! Number of segments for each segmentation
    std::vector<unsigned short> numSegmentsBySegmentation;
    //! Total number of segments (included in all segmentations)
    size_t numsegments;
    //! List of aggregators
    std::vector<Aggregator *> aggregators;
    //! Maximum number of iterations
    int maxiterations;
    //! Maximum execution time
    int maxseconds;
    //! Initial date
    Date time0;
    //! Ending date
    Date timeT;
    //! Degrees of freedom
    double ndf;
    //! Inverse functions
    Inverses inverses;
    //! Cholesky matrix (factor correlations)
    gsl_matrix *chol;
    //! Factor loadings (w_i)
    std::vector<double> floadings1;
    //! Factor loadings (sqrt(1-w_i^2))
    std::vector<double> floadings2;
    //! Antithetic method flag
    bool antithetic;
    //! Latin hypercube sample size
    unsigned short lhs_size;
    //! Block size
    unsigned short blocksize;
    //! RNG seed
    unsigned long seed;
    //! Hash (0=non show hashes) (default=0)
    size_t hash;
    //! Directory for output files
    std::string fpath;
    //! Output file mode (a=append, w=overwrite, c=create)
    char fmode;
    //! Time account
    Timer timer;
    //! Simulation threads
    std::vector<SimulationThread*> threads;
    //! Number of iterations done
    int numiterations;
    //! Number of finished threads
    int nfthreads;
    //! Ensures data consistence
    pthread_mutex_t mutex;
    //! Stop flag
    bool *stop;
    //! File indexes.csv
    std::ofstream findexes;

  private:
  
    //! Deallocate memory
    void release();
    //! Initialize params
    void initParams(IData &) throw(Exception);
    //! Initialize obligors
    void initObligors(IData &) throw(Exception);
    //! Initialize assets
    void initAssets(IData &) throw(Exception);
    //! Initialize simulation parameters
    void initModel(IData &) throw(Exception);
    //! Initialize aggregators
    void initAggregators(IData &) throw(Exception);
    //! Averaged exposures by segment
    std::vector<double> getExposures(int isegmentation, IData &) const;
    //! Append simulation result
    bool append(int ithread, const std::vector<short> &, const double *) noexcept;
    //! Non-copyable class
    MonteCarlo(const MonteCarlo &);
    //! Non-copyable class
    MonteCarlo & operator=(const MonteCarlo &);

  public:

    //! Constructor
    MonteCarlo(std::streambuf *s=nullptr);
    //! Destructor
    ~MonteCarlo();
    //! Set path for output files
    void setFilePath(const std::string &path, char mode, bool indexes=false) throw(std::exception);
    //! Initiliaze this class
    void setData(IData &) throw(Exception);
    //! Execute Monte Carlo
    void run(unsigned char numthreads, size_t nhash, bool *stop_=nullptr);
    //! Returns number of iterations done
    int getNumIterations() const;
    //! Returns maximum number of iterations to do
    int getMaxIterations() const;

  public:
  
    //! Friend class
    friend class SimulationThread;

};

} // namespace

#endif

