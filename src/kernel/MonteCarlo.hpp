
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

#ifndef _MonteCarlo_
#define _MonteCarlo_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <streambuf>
#include <pthread.h>
#include "kernel/IData.hpp"
#include "kernel/Inverses.hpp"
#include "kernel/Aggregator.hpp"
#include "kernel/SimulatedData.hpp"
#include "kernel/SimulationThread.hpp"
#include "portfolio/DateValues.hpp"
#include "utils/Date.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

// forward declaration
class SimulationThread;

//---------------------------------------------------------------------------

class MonteCarlo
{

  private:

    // logger
    Logger log;
    // list of simulated obligors
    vector<SimulatedObligor> obligors;
    // list of simulated assets
    char *assets;
    // simulated asset size
    int assetsize;
    // number of simulated assets
    int numassets;
    // datevalues list
    vector<DateValues> datevalues;
    // list of aggregators
    vector<Aggregator *> aggregators;
    // maximum number of iterations
    int maxiterations;
    // maximum execution time
    int maxseconds;
    // initial date
    Date time0;
    // date where risk is computed
    Date timeT;
    // degrees of freedom
    double ndf;
    // inverse functions
    Inverses inverses;
    // factors cholesky matrix
    gsl_matrix *chol;
    // factor loadings
    vector<double> floadings;
    // antithetic method flag
    bool antithetic;
    // rng seed
    unsigned long seed;
    // hash (0=non show hashes) (default=0)
    size_t hash;
    // directory for output files
    string fpath;
    // force file overwriting flag
    bool bforce;
    // time account
    Timer timer;
    // ellapsed time writting data to disk
    Timer timer3;
    // simulation threads
    vector<SimulationThread*> threads;
    // number of iterations done
    int numiterations;
    // number of finished threads
    int nfthreads;
    // ensures data consistence
    pthread_mutex_t mutex;
    // stop flag
    bool *stop;

  private:
  
    // deallocate memory
    void release();
    // initialize params
    void initParams(IData &) throw(Exception);
    // initialize obligors
    void initObligors(IData &) throw(Exception);
    // initialize assets
    void initAssets(IData &) throw(Exception);
    // initialize survivals functions
    void initModel(IData &) throw(Exception);
    // initialize aggregators
    void initAggregators(IData &) throw(Exception);
    // append simulation result
    bool append(vector<vector<double> > &) throw();
    // non-copyable class
    MonteCarlo(const MonteCarlo &);
    // non-copyable class
    MonteCarlo & operator=(const MonteCarlo &);

  public:

    // constructor
    MonteCarlo(streambuf *s=NULL);
    // destructor
    ~MonteCarlo();
    // set path for output files
    void setFilePath(const string &path, bool force);
    // initiliaze this class
    void setData(IData &) throw(Exception);
    // execute Monte Carlo
    void run(unsigned char numthreads, size_t nhash, bool *stop_=NULL);
    // returns iterations done
    int getNumIterations() const;
    // returns iterations done
    int getMaxIterations() const;

  public:
  
    // friend class
    friend class SimulationThread;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
