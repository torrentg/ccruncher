
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

#ifndef _MonteCarlo_
#define _MonteCarlo_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <pthread.h>
#include "kernel/IData.hpp"
#include "kernel/Aggregator.hpp"
#include "kernel/SimulatedData.hpp"
#include "kernel/SimulationThread.hpp"
#include "survival/Survival.hpp"
#include "math/Copula.hpp"
#include "utils/Date.hpp"
#include "utils/Timer.hpp"
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

    // list of simulated borrowers
    vector<SimulatedBorrower> borrowers;
    // list of simulated assets
    vector<SimulatedAsset> assets;
    // list of aggregators
    vector<Aggregator *> aggregators;
    // list of segmentations
    Segmentations *segmentations;
    // maximum number of iterations
    int maxiterations;
    // maximum execution time
    int maxseconds;
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
    // rng seed
    long seed;
    // hash (0=non show hashes) (default=0)
    int hash;
    // directory for output files
    string fpath;
    // force file overwriting flag
    bool bforce;
    // time account
    Timer timer;
    // ellapsed time writting data to disk
    Timer timer4;
    // simulation threads
    vector<SimulationThread*> threads;
    // number of iterations done
    int numiterations;
    // number of finished threads
    int nfthreads;
    // ensures data consistence
    pthread_mutex_t mutex;
    // trace simulated copulas+defaults flag
    bool btrace;
    // file where copula values are stored (if btrace is set)
    ofstream fcopulas;
    // file where default times are stored (if btrace is set)
    ofstream fdefaults;

  private:
  
    // initialize params
    void initParams(IData &) throw(Exception);
    // initialize borrowers
    void initBorrowers(IData &) throw(Exception);
    // initialize assets
    void initAssets(IData &) throw(Exception);
    // initialize survival functions
    void initSurvival(IData &) throw(Exception);
    // initialize copula
    void initCopula(IData &idata, long) throw(Exception);
    // initialize aggregators
    void initAggregators(IData &) throw(Exception);
    // initialize trace files
    void initTrace() throw(Exception);
    // print trace
    void printTrace(vector<SimulatedBorrower> &) throw(Exception);
    // append simulation result
    bool append(vector<vector<double> > &, vector<SimulatedBorrower> &);

  public:

    // constructor
    MonteCarlo();
    // destructor
    ~MonteCarlo();
    // set path for output files
    void setFilePath(string path, bool force);
    // set hash value (mark every num values)
    void setHash(int num);
    // initiliaze this class
    void initialize(IData &) throw(Exception);
    // execute Monte Carlo
    int execute(int) throw(Exception);
    // trace copula values + trace default times
    void trace(bool);
    // deallocate memory
    void release();
    
  public:
  
    // friend class
    friend class SimulationThread;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
