
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
//===========================================================================

#ifndef _MonteCarlo_
#define _MonteCarlo_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "IData.hpp"
#include "interests/Interest.hpp"
#include "ratings/Ratings.hpp"
#include "transitions/TransitionMatrix.hpp"
#include "sectors/Sectors.hpp"
#include "correlations/CorrelationMatrix.hpp"
#include "math/CopulaNormal.hpp"
#include "portfolio/Portfolio.hpp"
#include "portfolio/Client.hpp"
#include "segmentations/Segmentations.hpp"
#include "aggregators/Aggregators.hpp"
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
    Aggregators *aggregators;
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

    /** matriu de transicio tall temporal */
    TransitionMatrix *mtrans;
    /** client correlation matrix */
    double **cmatrix;
    /** objecte per a generar les copules */
    CopulaNormal **copulas;
    /** dates dels nodes temporals */
    Date *dates;
    /** path ratings per client */
    int **rpaths;

    /* flag per gestionar antithetic (default=false) */
    bool reversed;
    /* verbosity level (0=quiet, 1=verbose) (default=0)*/
    int verbosity;
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
    double ** initCorrelationMatrix(double **, vector<Client *> *, int) throw(Exception);
    CopulaNormal** initCopulas(double **, int, int, long) throw(Exception);
    int** initRatingsPaths(int, int, vector<Client *> *);
    void initAggregators() throw(Exception);
    void evalueAggregators() throw(Exception);
    void generateRatingsPaths();
    void generateRatingsPath(int i);

  public:

    MonteCarlo();
    ~MonteCarlo();

    void setFilePath(string path, bool force);
    void setVerbosity(int level);
    void useMPI(bool);
    void setHash(int num);
    void initialize(const IData *) throw(Exception);
    void execute() throw(Exception);
    void showInfo();
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
