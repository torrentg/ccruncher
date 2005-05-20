
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
// Montecarlo.cpp - MonteCarlo code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added logger
//
// 2005/05/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed period time resolution (year->month)
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Arrays class
//
//===========================================================================

#include <cfloat>
#include <cassert>
#include "kernel/MonteCarlo.hpp"
#include "utils/Arrays.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"
#include "utils/Parser.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::MonteCarlo::MonteCarlo()
{
  // setting default values
  reset();
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::MonteCarlo::~MonteCarlo()
{
  release();
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::MonteCarlo::reset()
{
  MAXSECONDS = 0L;
  MAXITERATIONS = 0L;
  STEPS = 0;
  STEPLENGTH = 0;
  N = 0L;
  CONT = 0L;
  antithetic = false;
  reversed = false;

  usempi = false;
  hash = 0;
  fpath = "path not set";
  bforce = false;

  interests = NULL;
  ratings = NULL;
  sectors = NULL;
  segmentations = NULL;
  aggregators = NULL;
  clients = NULL;

  mtrans = NULL;
  cmatrix = NULL;
  copulas = NULL;
  dates = NULL;
  rpaths = NULL;
}

//===========================================================================
// release
//===========================================================================
void ccruncher::MonteCarlo::release()
{
  // deallocating transition matrix object
  if (mtrans != NULL) { delete mtrans; mtrans = NULL; }

  // client correlation matrix deallocated by copulas[0] object

  // deallocating copula vector
  if (copulas != NULL)
  {
    for (int i=0;i<=STEPS;i++)
    {
      if (copulas[i] != NULL) delete copulas[i];
      copulas[i] = NULL;
    }
    delete [] copulas;
    copulas = NULL;
  }

  // deallocating dates vector
  if (dates != NULL) { delete [] dates; dates = NULL; }

  // deallocating rpaths matrix
  if (rpaths != NULL)
  {
    for (int i=0;i<N;i++)
    {
      if (rpaths[i] != NULL) delete [] rpaths[i];
      rpaths[i] = NULL;
    }
    delete [] rpaths;
    rpaths = NULL;
  }
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::MonteCarlo::initialize(const IData *idata) throw(Exception)
{
  if (idata == NULL)
  {
    throw Exception("MonteCarlo::initialize(): NULL idata");
  }
  else if (MAXITERATIONS != 0L)
  {
    throw Exception("MonteCarlo::initialize(): reinitialization not allowed");
  }
  else if (fpath == "path not set")
  {
    throw Exception("MonteCarlo::initialize(): output files path not established");
  }

  try
  {
    init(idata);
  }
  catch(Exception &e)
  {
    release();
    throw Exception(e, "MonteCarlo::initialize()");
  }
}

//===========================================================================
// init
//===========================================================================
void ccruncher::MonteCarlo::init(const IData *idata) throw(Exception)
{
  Logger::addBlankLine();
  Logger::trace("initialing procedure", '*');
  Logger::newIndentLevel();
  
  // initializing parameters
  initParams(idata);

  // initializing clients
  initClients(idata, dates, STEPS);

  // initializing ratings and transition matrix
  initRatings(idata);

  // initializing sectors
  initSectors(idata);
  
  // initializing transition matrix
  initMTrans(idata);
  
  // initializing correlation matrix
  cmatrix = initCorrelationMatrix(idata->correlations->getMatrix(), clients, N);

  // initializing copulas
  copulas = initCopulas(cmatrix, N, STEPS, idata->params->copula_seed);

  // ratings paths allocation
  rpaths = initRatingsPaths(N, STEPS, clients);

  // initializing data output (segmentations + aggregators)
  initDataOutput(idata);
  
  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initParams
//===========================================================================
void ccruncher::MonteCarlo::initParams(const IData *idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting parameters", '-');
  Logger::newIndentLevel();

  // information trace
  Logger::trace("resolution method", string("rating-path"));

  // fixing variance reduction method
  antithetic = idata->params->antithetic;
  Logger::trace("antithetic mode", Parser::bool2string(antithetic));

  // max number of seconds
  MAXSECONDS = idata->params->maxseconds;
  Logger::trace("maximum execution time (in seconds)", Parser::long2string(MAXSECONDS));
  
  // max number of iterations
  MAXITERATIONS = idata->params->maxiterations;
  Logger::trace("maximum number of iterations", Parser::long2string(MAXITERATIONS));

  // fixing step number
  STEPS = idata->params->steps;
  Logger::trace("number of time steps", Parser::int2string(STEPS));

  // fixing steplength
  STEPLENGTH = idata->params->steplength;
  Logger::trace("length of each time step (in months)", Parser::int2string(STEPLENGTH));

  // fixing time-tranches
  begindate = idata->params->begindate;
  dates = idata->params->getDates();
  
  // tracing dates
  for (int i=0;i<=STEPS;i++)
  Logger::trace("date[" + Parser::int2string(i)+"]", Parser::date2string(dates[i]));
  
  // initializing internal variables
  CONT = 0L;
  reversed = false;

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initClients
//===========================================================================
void ccruncher::MonteCarlo::initClients(const IData *idata, Date *idates, int isteps) throw(Exception)
{
  // setting logger header
  Logger::trace("fixing clients to simulate", '-');
  Logger::newIndentLevel();
  
  // setting logger info
  Logger::trace("simulate only active clients", Parser::bool2string(idata->params->onlyactive));
  Logger::trace("number of initial clients", Parser::long2string(idata->portfolio->getClients()->size()));
  
  // fixing number of clients
  if (idata->params->onlyactive)
  {
    idata->portfolio->sortClients(idates[0], idates[isteps]);
    N = idata->portfolio->getNumActiveClients(idates[0], idates[isteps]);
  } 
  else 
  {
    N = idata->portfolio->getClients()->size();
  }
  
  Logger::trace("number of simulated clients", Parser::long2string(N));
  
  // checking that exist clients to simulate
  if (N == 0) 
  {
    throw Exception("MonteCarlo::init(): 0 clients to simulate");
  }

  // setting client object
  clients = idata->portfolio->getClients();

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initRatings
//===========================================================================
void ccruncher::MonteCarlo::initRatings(const IData *idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting ratings", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of ratings", Parser::int2string(idata->ratings->getRatings()->size()));
  
  // fixing ratings
  ratings = idata->ratings;
  
  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initSectors
//===========================================================================
void ccruncher::MonteCarlo::initSectors(const IData *idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting sectors", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of sectors", Parser::int2string(idata->sectors->getSectors()->size()));

  // fixing ratings
  sectors = idata->sectors;
    
  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initMTrans
//===========================================================================
void ccruncher::MonteCarlo::initMTrans(const IData *idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting transition matrix", '-');
  Logger::newIndentLevel();

  // setting logger info
  string sval = Parser::int2string(idata->transitions->size());
  Logger::trace("matrix dimension", sval + "x" + sval);
  Logger::trace("initial period (in months)", Parser::int2string(idata->transitions->period));
  Logger::trace("scaling matrix to step length (in months)", Parser::int2string(STEPLENGTH));
  
  // trobem la matriu de transicio usada
  mtrans = translate(idata->transitions, STEPLENGTH);

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// client correlation matrix construction
//===========================================================================
double ** ccruncher::MonteCarlo::initCorrelationMatrix(double **sectorcorrels,
                               vector<Client *> *vclients, int n) throw(Exception)
{
  // setting logger header
  Logger::trace("computing client correlation matrix", '-');
  Logger::newIndentLevel();
  
  // setting logger info
  string sval = Parser::int2string(n);
  Logger::trace("matrix dimension", sval+"x"+sval);
  
  // allocating space
  double **ret = Arrays<double>::allocMatrix(n, n);

  // definim la matriu de correlacions entre clients
  for (int i=0;i<n;i++)
  {
    int isector1 = (*vclients)[i]->isector;

    for (int j=0;j<n;j++)
    {
      int isector2 = (*vclients)[j]->isector;

      if (i == j)
      {
        ret[i][j] = 1.0;
      }
      else
      {
        ret[i][j] = sectorcorrels[isector1][isector2];
      }
    }
  }

  // exit function
  Logger::previousIndentLevel();
  return ret;
}

//===========================================================================
// copula construction at every time tranch
//===========================================================================
CopulaNormal** ccruncher::MonteCarlo::initCopulas(double **ccm, long n, int k, long seed) throw(Exception)
{
  CopulaNormal **ret = NULL;
  
  // setting logger header
  Logger::trace("initializing copulas", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("copula dimension", Parser::long2string(n));
  Logger::trace("number of copulas (one per time-tranch)", Parser::int2string(k+1));
  Logger::trace("seed used to initialize randomizer (0=none)", Parser::long2string(seed));

  // allocating space
  try
  {
    ret = new CopulaNormal*[k+1];
    for (int i=0;i<=k;i++) ret[i] = NULL;

    // creem l'objecte per la generacio de copules normals
    ret[0] = new CopulaNormal(n, ccm);

    // el repliquem per tots els talls temporals
    for(int i=1;i<=k;i++)
    {
      ret[i] = new CopulaNormal(*(ret[0]));
    }
  }
  catch(std::exception &e)
  {
    if (ret != NULL)
    {
      for (int i=0;i<=k;i++)
      {
        if (ret[i] != NULL) delete ret[i];
        ret[i] = NULL;
      }
      delete [] ret;
    }    
    throw Exception(e, "MonteCarlo::initCopulas()");
  }

  // seeding random number generators
  // if no seed is given /dev/urandom or time() will be used
  if (seed != 0L)
  {
    for(int i=0;i<=k;i++)
    {
      ret[i]->setSeed(seed+(long)i);
    }
  }

  // exit function
  Logger::previousIndentLevel();
  return ret;
}

//===========================================================================
// initRatingsPaths
//===========================================================================
int** ccruncher::MonteCarlo::initRatingsPaths(int n, int k, vector<Client *> *vclients) throw(Exception)
{
  int **ret = NULL;

  // setting logger header
  Logger::trace("initializing rating paths buffer", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("buffer dimension", Parser::long2string(n)+"x"+Parser::int2string(k+1));

  // allocating space
  try
  {
    ret = new int*[n];
    for (int i=0;i<n;i++) ret[i] = NULL;
    
    for (int i=0;i<n;i++)
    {
      // allocating memory
      ret[i] = new int[k+1];

      // setting initial client rating
      ret[i][0] = (*vclients)[i]->irating;

      // setting a default rating
      for (int j=1;j<=k;j++)
      {
        ret[i][j] = 0;
      }
    }
  }
  catch(std::exception &e)
  {
    if (ret != NULL)
    {
      for (int i=0;i<n;i++)
      {
        if (ret[i] != NULL) delete [] ret[i];
        ret[i] = NULL;
      }
      delete [] ret;
    }
    throw Exception(e, "MonteCarlo::initRatingsPaths()");
  }

  // exit function
  Logger::previousIndentLevel();
  return ret;
}

//===========================================================================
// initDataOutput
//===========================================================================
void ccruncher::MonteCarlo::initDataOutput(const IData *idata) throw(Exception)
{
  // init only if spath is set
  if (fpath == "") 
  {
    return;
  }

  // setting logger header
  Logger::trace("initializing data output", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("output data directory", File::normalizePath(fpath));
  Logger::trace("number of segmentations defined", Parser::int2string(idata->segmentations->getSegmentations().size()));
  Logger::trace("number of aggregators defined", Parser::int2string(idata->aggregators->getAggregators()->size()));
  Logger::trace("number of segments to aggregate", Parser::int2string(idata->aggregators->getNumSegments()));

  // setting objects  
  segmentations = idata->segmentations;
  aggregators = idata->aggregators;

  // initializing aggregators
  Logger::trace("initializing aggregators (elapsed time)", true);
  int indexdefault = mtrans->getIndexDefault();
  aggregators->initialize(dates, STEPS+1, clients, N, indexdefault, ratings, interests);
  aggregators->setOutputProperties(fpath, bforce, 0);

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// execute
//===========================================================================
void ccruncher::MonteCarlo::execute() throw(Exception)
{
  bool moreiterations = true;
  Timer sw1, sw2;

  // assertions
  assert(fpath != "");

  // setting logger header
  Logger::addBlankLine();
  Logger::trace("running Monte Carlo" + (hash==0?"": " [" + Parser::int2string(hash) + " simulations per hash]"), '*');
  Logger::newIndentLevel();

  try
  {
    // touching output
    aggregators->touch();

    while(moreiterations)
    {
      // generem les copules
      sw1.resume();
      generateRatingsPaths();
      sw1.stop();

      // evaluem la cartera
      sw2.resume();
      evalueAggregators();
      sw2.stop();

      // increment contadors
      CONT++;

      // printing hashes
      if (hash > 0 && CONT%hash == 0)
      {
        Logger::append(".");
      }

      // checking stop criteria
      if (CONT >= MAXITERATIONS)
      {
        moreiterations = false;
      }

      // checking stop criteria
      if (MAXSECONDS > 0L && (sw1.read()+sw2.read())> MAXSECONDS)
      {
        moreiterations = false;
      }
    }

    // closing aggregators
    aggregators->flush(true);

    // sortim
    Logger::trace("elapsed time generatings ratings paths", Timer::format(sw1.read()));
    Logger::trace("elapsed time aggregating data", Timer::format(sw2.read()));
    Logger::trace("total simulation time", Timer::format(sw1.read()+sw2.read()));
    Logger::addBlankLine();
  }
  catch(Exception &e)
  {
    throw Exception(e, "MonteCarlo::execute()");
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// copula generation (one for each time tranch)
// @exception en cas d'error
//===========================================================================
void ccruncher::MonteCarlo::generateRatingsPaths()
{
  // without antithetic method
  if (!antithetic)
  {
    for(int i=1;i<=STEPS;i++)
    {
      copulas[i]->next();
    }
  }
  // with antithetic method (antithetic == true)
  else
  {
    if (!reversed)
    {
      for(int i=1;i<=STEPS;i++)
      {
        copulas[i]->next();
      }
      reversed = true;
    }
    else
    {
      reversed = false;
    }
  }

  // for each client we compute ratings path
  for (int i=0;i<N;i++)
  {
    generateRatingsPath(i);
  }
}

//===========================================================================
// fix rating evolution of client for each time tranch. 
// put result in rpaths[iclient]
//===========================================================================
void ccruncher::MonteCarlo::generateRatingsPath(int iclient)
{
  for (int t=1;t<=STEPS;t++)
  {
    if (antithetic)
    {
      if (reversed)
      {
        rpaths[iclient][t] = mtrans->evalue(rpaths[iclient][t-1], copulas[t]->get(iclient));
      }
      else
      {
        rpaths[iclient][t] = mtrans->evalue(rpaths[iclient][t-1], 1.0-copulas[t]->get(iclient));
      }
    }
    else
    {
      rpaths[iclient][t] = mtrans->evalue(rpaths[iclient][t-1], copulas[t]->get(iclient));
    }
  }
}

//===========================================================================
// portfolio evaluation using current copula
// @exception if error
//===========================================================================
void ccruncher::MonteCarlo::evalueAggregators() throw(Exception)
{
  // adding one simulation
  aggregators->append(rpaths, STEPS+1, N, clients);
}

//===========================================================================
// useMPI
//===========================================================================
void ccruncher::MonteCarlo::useMPI(bool val)
{
  usempi = val;
}

//===========================================================================
// setHash
//===========================================================================
void ccruncher::MonteCarlo::setHash(int num)
{
  assert(num >= 0);
  hash = max(0, num);
}

//===========================================================================
// setFilePath
//===========================================================================
void ccruncher::MonteCarlo::setFilePath(string path, bool force)
{
  fpath = path;
  bforce = force;
}
