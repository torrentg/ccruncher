
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// Montecarlo.hpp - MonteCarlo header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/03/25 - Gerard Torrent [gerard@mail.generacio.com]
//   . added logger
//
// 2005/05/21 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed aggregators class
//   . added new SegmentAggregator class
//
// 2005/05/27 - Gerard Torrent [gerard@mail.generacio.com]
//   . added simulation method time-to-default
//
// 2005/07/12 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed useMPI() method
//
// 2005/07/24 - Gerard Torrent [gerard@mail.generacio.com]
//   . class CopulaNormal renamed to GaussianCopula
//   . GaussianCopula replaced by BlockGaussianCopula
//
// 2005/07/27 - Gerard Torrent [gerard@mail.generacio.com]
//   . execute() method returns number of realized simulations
//
// 2005/09/21 - Gerard Torrent [gerard@mail.generacio.com]
//   . added method randomize()
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/10/23 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed some method signatures
//
// 2006/01/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . MonteCarlo refactoring
//   . generic copula array
//
// 2006/01/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed simule and method params
//
// 2007/07/31 - Gerard Torrent [gerard@mail.generacio.com]
//   . added method printPrecomputedLosses()
//   . Client class renamed to Borrower
//
// 2008/11/09 - Gerard Torrent [gerard@mail.generacio.com]
//   . modified output file name (portfolio-rest.out -> portfolio.out)
//   . added optional bulk of copula values to file
//   . added optional bulk of simulated default to file
//   . changed from SegmentAggregator to Aggregator
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

    Ratings *ratings;
    Sectors *sectors;
    vector<Aggregator *> aggregators;
    vector<Borrower *> *borrowers;

    /** maximum number of iterations */
    long MAXITERATIONS;
    /** maximum execution time */
    long MAXSECONDS;
    /** number of time steps */
    int STEPS;
    /** step length */
    int STEPLENGTH;
    /** number of borrowers (taking into account onlyActiveBorrowers flag) */
    long N;
    /** iterations counter */
    long CONT;
    /** initial date */
    Date begindate;
    /* antithetic technologie for simetric copulas */
    bool antithetic;

    /** transition matrix (size = 1) (used by rating-path method, canbe used by time-to-default method) */
    Survival *survival;
    /** copula used to simulate correlations */
    Copula *copula;
    /** date per time tranch (size = M) */
    vector<Date> dates;
    /** simulated time-to-default per borrower (size = N) */
    int *ittd;

    /* management flag for antithetic method (default=false) */
    bool reversed;
    /* hash (0=non show hashes) (default=0) */
    int hash;
    /* directory for output files */
    string fpath;
    /* force file overwriting flag */
    bool bforce;
    /* list precomputed asset losses flag */
    bool blplosses;
    /* list simulated copula values flag */
    bool blcopulas;
    /* list simulated default times */
    bool bldeftime;
    /** file where copula values are stored (if blcopulas flag is set) */
    ofstream fcopulas;
    /** file where default times are stored (if bldeftime flag is set) */
    ofstream fdeftime;

    // internal methods
    void reset();
    void alloc(int) throw(Exception);
    void release();

    // init methods
    void init(IData &) throw(Exception);
    void initParams(const IData &) throw(Exception);
    void initBorrowers(const IData &) throw(Exception);
    void initRatings(const IData &) throw(Exception);
    void initSectors(const IData &) throw(Exception);
    void initTimeToDefault(IData &) throw(Exception);
    void initCopula(const IData &idata, long) throw(Exception);
    void initTimeToDefaultArray(int) throw(Exception);
    void initAggregators(const IData &) throw(Exception);

    // Monte Carlo methods
    void randomize();
    void simulate();
    bool evalue() throw(Exception);
    int simTimeToDefault(int iborrower);
    double getRandom(int iborrower);
    long executeWorker() throw(Exception);
    long executeCollector() throw(Exception);

    // trace methods
    void initAdditionalOutput() throw(Exception);
    void printCopulaValues() throw(Exception);
    void printDefaultTimes(int *) throw(Exception);

    // auxiliary methods
    double** getBorrowerCorrelationMatrix(const IData &);


  public:

    MonteCarlo();
    ~MonteCarlo();

    void setFilePath(string path, bool force);
    void setHash(int num);
    void initialize(IData &) throw(Exception);
    long execute() throw(Exception);
    void setAdditionalOutput(bool losses, bool copulas, bool deftimes);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
