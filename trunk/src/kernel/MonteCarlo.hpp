
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
#include "kernel/IData.hpp"
#include "kernel/Aggregator.hpp"
#include "ratings/Ratings.hpp"
#include "transitions/TransitionMatrix.hpp"
#include "sectors/Sectors.hpp"
#include "math/Copula.hpp"
#include "portfolio/Borrower.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class MonteCarlo
{

  private:

    // list of ratings
    const Ratings *ratings;
    // list of sectors
    const Sectors *sectors;
    // list of borrowers
    vector<Borrower *> *borrowers;
    // list of aggregators
    vector<Aggregator *> aggregators;
    // maximum number of iterations
    long MAXITERATIONS;
    // maximum execution time
    long MAXSECONDS;
    // number of borrowers (taking into account onlyActiveBorrowers flag)
    long N;
    // iterations counter
    long CONT;
    // initial date
    Date time0;
    // date where risk is computed
    Date timeT;
    // antithetic method flag
    bool antithetic;
    // survival functions
    Survival survival;
    // copula used to simulate correlations
    Copula *copula;
    // simulated time-to-default per borrower (size = N)
    Date *ttd;
    // management flag for antithetic method (default=false)
    bool reversed;
    // hash (0=non show hashes) (default=0)
    int hash;
    // directory for output files
    string fpath;
    // force file overwriting flag
    bool bforce;
    // list simulated copula values flag
    bool blcopulas;
    // list simulated default times
    bool bldeftime;
    // file where copula values are stored (if blcopulas flag is set)
    ofstream fcopulas;
    // file where default times are stored (if bldeftime flag is set)
    ofstream fdeftime;
    // maximum number of segments in a aggregator * sizeof(double)
    long ccmaxbufsize;

  private:
  
    // internal members
    void reset();
    // alloc memory
    void alloc(int) throw(Exception);
    // initialize params
    void initParams(IData &) throw(Exception);
    // initialize borrowers
    void initBorrowers(IData &) throw(Exception);
    // initialize ratings
    void initRatings(IData &) throw(Exception);
    // initialize sectors
    void initSectors(IData &) throw(Exception);
    // initialize survival functions
    void initSurvival(IData &) throw(Exception);
    // initialize copula
    void initCopula(IData &idata, long) throw(Exception);
    // initialize buffers
    void initTimeToDefaultArray(int) throw(Exception);
    // initialize aggregators
    void initAggregators(IData &) throw(Exception);
    // generate random numbers (Monte Carlo)
    void randomize();
    // simulate default times
    void simulate();
    // evalue portfolio
    bool evalue() throw(Exception);
    // simulate default time
    Date simTimeToDefault(int iborrower);
    // returns the copula value
    double getRandom(int iborrower);
    // runs Monte Carlo as slave
    long executeWorker() throw(Exception);
    // runs Monte Carlo as master
    long executeCollector() throw(Exception);
    // initialize traces outputs
    void initAdditionalOutput() throw(Exception);
    // prints current copula values
    void printCopulaValues() throw(Exception);
    // prints current default times
    void printDefaultTimes(Date *) throw(Exception);

  public:

    // default constructor
    MonteCarlo();
    // destructor
    ~MonteCarlo();
    // set path for output files
    void setFilePath(string path, bool force);
    // set hash value (mark every num values)
    void setHash(int num);
    // initiliaze this class
    void initialize(IData &, bool only_validation=false) throw(Exception);
    // execute Monte Carlo
    long execute() throw(Exception);
    // enables trace outputs
    void setAdditionalOutput(bool copulas, bool deftimes);
    // deallocate memory
    void release();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
