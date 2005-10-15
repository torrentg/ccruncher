
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
//===========================================================================

#ifndef _MonteCarlo_
#define _MonteCarlo_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "kernel/IData.hpp"
#include "kernel/SegmentAggregator.hpp"
#include "interests/Interest.hpp"
#include "ratings/Ratings.hpp"
#include "transitions/TransitionMatrix.hpp"
#include "sectors/Sectors.hpp"
#include "correlations/CorrelationMatrix.hpp"
#include "math/BlockGaussianCopula.hpp"
#include "portfolio/Portfolio.hpp"
#include "portfolio/Client.hpp"
#include "segmentations/Segmentations.hpp"
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

    Interests *interests;
    Ratings *ratings;
    Sectors *sectors;
    Segmentations *segmentations;
    vector<SegmentAggregator *> aggregators;
    vector<Client *> *clients;

    /** numero maxim de iteracions */
    long MAXITERATIONS;
    /** numero maxim de segons */
    long MAXSECONDS;
    /** numero de talls temporals */
    int STEPS;
    /** amplada de cada pas temporal */
    int STEPLENGTH;
    /** numero de clients */
    long N;
    /** contador de iteracions */
    long CONT;
    /** data de inici de la simulacio */
    Date begindate;
    /* antithetic technologie for simetric copulas */
    bool antithetic;
    /* simulation method flag (true=time-to-default, false=rating-path) */
    bool ttdmethod;

    /** transition matrix (size = 1) (used by rating-path method, canbe used by time-to-default method) */
    Survival *survival;
    /** survival function (can be used by time-to-default method) */
    TransitionMatrix *mtrans;
    /** arrays of pointers to copulas (size = M) */
    BlockGaussianCopula **copulas;
    /** date per time tranch (size = M) */
    Date *dates;
    /** simulated time-to-default per client (size = N) */
    int *ittd;

    /* number of copulas used (time-to-default->1, rating-path->STEPS+1) */
    int numcopulas;
    /* management flag for antithetic method (default=false) */
    bool reversed;
    /* hash (0=non show hashes) (default=0) */
    int hash;
    /* directory for output files */
    string fpath;
    /* force file overwriting flag */
    bool bforce;

    void reset();
    void alloc(int) throw(Exception);
    void release();
    double ** allocMatrix(int n) throw(Exception);
    void init(IData *) throw(Exception);
    void initParams(const IData *) throw(Exception);
    void initClients(const IData *, Date *, int) throw(Exception);
    void initRatings(const IData *) throw(Exception);
    void initSectors(const IData *) throw(Exception);
    void initRatingPath(const IData *) throw(Exception);
    void initTimeToDefault(IData *) throw(Exception);
    BlockGaussianCopula** initCopulas(const IData *idata, long, int, long) throw(Exception);
    int* initTimeToDefaultArray(int) throw(Exception);
    void initAggregators(const IData *) throw(Exception);
    bool evalueAggregators() throw(Exception);
    void randomize();
    void simulate();
    int simRatingPath(int iclient);
    int simTimeToDefault(int iclient);
    double getRandom(int itime, int iclient);
    long executeWorker() throw(Exception);
    long executeCollector() throw(Exception);


  public:

    MonteCarlo();
    ~MonteCarlo();

    void setFilePath(string path, bool force);
    void setHash(int num);
    void initialize(IData *) throw(Exception);
    long execute() throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
