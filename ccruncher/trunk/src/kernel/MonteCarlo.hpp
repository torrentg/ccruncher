
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
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added logger
//
// 2005/05/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed aggregators class
//   . added new SegmentAggregator class
//
// 2005/05/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added simulation method time-to-default
//
// 2005/07/12 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed useMPI() method
//
// 2005/07/24 - Gerard Torrent [gerard@fobos.generacio.com]
//   . class CopulaNormal renamed to GaussianCopula
//   . GaussianCopula replaced by BlockGaussianCopula
//
// 2005/07/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . execute() method returns number of realized simulations
//
// 2005/09/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added method randomize()
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/10/23 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed some method signatures
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . MonteCarlo refactoring
//   . generic copula array
//
// 2006/01/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed simule and method params
//
// 2007/07/31 - Gerard Torrent [gerard@mail.generacio.com]
//   . added method printPrecomputedLosses()
//
//===========================================================================

#ifndef _MonteCarlo_
#define _MonteCarlo_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "kernel/IData.hpp"
#include "kernel/SegmentAggregator.hpp"
#include "ratings/Ratings.hpp"
#include "transitions/TransitionMatrix.hpp"
#include "sectors/Sectors.hpp"
#include "math/Copula.hpp"
#include "portfolio/Client.hpp"
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
    vector<SegmentAggregator *> aggregators;
    vector<Client *> *clients;

    /** maximum number of iterations */
    long MAXITERATIONS;
    /** maximum execution time */
    long MAXSECONDS;
    /** number of time steps */
    int STEPS;
    /** step length */
    int STEPLENGTH;
    /** number of borrowers (taking into account onlyActiveClients flag) */
    long N;
    /** iterations counter */
    long CONT;
    /** initial date */
    Date begindate;
    /* antithetic technologie for simetric copulas */
    bool antithetic;

    /** transition matrix (size = 1) (used by rating-path method, canbe used by time-to-default method) */
    Survival *survival;
    /** survival function (can be used by time-to-default method) */
    TransitionMatrix *mtrans;
    /** copula used to simulate correlations */
    Copula *copula;
    /** date per time tranch (size = M) */
    Date *dates;
    /** simulated time-to-default per client (size = N) */
    int *ittd;

    /* management flag for antithetic method (default=false) */
    bool reversed;
    /* hash (0=non show hashes) (default=0) */
    int hash;
    /* directory for output files */
    string fpath;
    /* force file overwriting flag */
    bool bforce;

    // internal methods
    void reset();
    void alloc(int) throw(Exception);
    void release();

    // init methods
    void init(IData &) throw(Exception);
    void initParams(const IData &) throw(Exception);
    void initClients(const IData &, Date *, int) throw(Exception);
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
    int simTimeToDefault(int iclient);
    double getRandom(int iclient);
    long executeWorker() throw(Exception);
    long executeCollector() throw(Exception);

    // auxiliary methods
    double** getClientCorrelationMatrix(const IData &);


  public:

    MonteCarlo();
    ~MonteCarlo();

    void setFilePath(string path, bool force);
    void setHash(int num);
    void initialize(IData &) throw(Exception);
    long execute() throw(Exception);
    void printPrecomputedLosses();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
