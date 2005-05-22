
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
// Montecarlo.hpp - MonteCarlo header
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
#include "math/CopulaNormal.hpp"
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

    /** transition matrix (size = 1) */
    TransitionMatrix *mtrans;
    /** client correlation matrix (size = N x N) */
    double **cmatrix;
    /** arrays of pointers to copulas (size = M) */
    CopulaNormal **copulas;
    /** date per time tranch (size = M) */
    Date *dates;
    /** simulated time-to-default per client (size = N) */
    int *ittd;

    /* management flag for antithetic method (default=false) */
    bool reversed;
    /* mpi usage flag (default=false) */
    bool usempi;
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
    void init(const IData *) throw(Exception);
    void initParams(const IData *) throw(Exception);
    void initClients(const IData *, Date *, int) throw(Exception);
    void initRatings(const IData *) throw(Exception);
    void initSectors(const IData *) throw(Exception);
    void initMTrans(const IData *) throw(Exception);
    double ** initCorrelationMatrix(double **, vector<Client *> *, int) throw(Exception);
    CopulaNormal** initCopulas(double **, long, int, long) throw(Exception);
    int* initTimeToDefaultArray(int) throw(Exception);
    void initAggregators(const IData *) throw(Exception);
    void evalueAggregators() throw(Exception);
    void simulate();
    int simulateTTD(int iclient);

  public:

    MonteCarlo();
    ~MonteCarlo();

    void setFilePath(string path, bool force);
    void useMPI(bool);
    void setHash(int num);
    void initialize(const IData *) throw(Exception);
    void execute() throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
