
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
//   . removed aggregators class
//   . added new SegmentAggregator class
//
// 2005/05/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added simulation method time-to-default
//
// 2005/06/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added interests support
//
// 2005/07/12 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed useMPI() method
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
  ttdmethod = true;
  numcopulas = 0;

  hash = 0;
  fpath = "path not set";
  bforce = false;

  interests = NULL;
  ratings = NULL;
  sectors = NULL;
  segmentations = NULL;
  clients = NULL;

  survival = NULL;
  mtrans = NULL;
  cmatrix = NULL;
  copulas = NULL;
  dates = NULL;
  ittd = NULL;
}

//===========================================================================
// release
//===========================================================================
void ccruncher::MonteCarlo::release()
{
  // deallocating transition matrix object
  if (mtrans != NULL) { delete mtrans; mtrans = NULL; }

  // survival fuction object deallocated by IData (his container)
  // client correlation matrix deallocated by copulas[0] object

  // deallocating copula vector
  if (copulas != NULL)
  {
    for (int i=0;i<numcopulas;i++)
    {
      if (copulas[i] != NULL) delete copulas[i];
      copulas[i] = NULL;
    }
    delete [] copulas;
    copulas = NULL;
  }

  // deallocating dates vector
  if (dates != NULL) {
    Arrays<Date>::deallocVector(dates);
    dates = NULL;
  }

  // deallocating time-to-default arrays
  if (ittd != NULL) {
    Arrays<int>::deallocVector(ittd);
    ittd = NULL;
  }

  // dropping aggregators elements
  for(unsigned int i=0;i<aggregators.size();i++) {
    if (aggregators[i] != NULL) {
      delete aggregators[i];
      aggregators[i] = NULL;
    }
  }
  aggregators.clear();
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::MonteCarlo::initialize(IData *idata) throw(Exception)
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
void ccruncher::MonteCarlo::init(IData *idata) throw(Exception)
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

  if (ttdmethod) {
    // initializing transition matrix (rating-path method)
    initTimeToDefault(idata);
  }
  else {
    // initialize survival function (time-to-default method)
    initRatingPath(idata);
  }

  // initializing correlation matrix
  cmatrix = initCorrelationMatrix(idata->correlations->getMatrix(), clients, N);

  // initializing copulas
  copulas = initCopulas(cmatrix, N, numcopulas, idata->params->copula_seed);

  // ratings paths allocation
  ittd = initTimeToDefaultArray(N);

  // initializing aggregators
  initAggregators(idata);

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
  for (int i=0;i<=STEPS;i++) {
    Logger::trace("date[" + Parser::int2string(i)+"]", Parser::date2string(dates[i]));
  }

  // fixing simulation method
  if (idata->params->smethod == "time-to-default") {
    ttdmethod = true;
    numcopulas = 1;
    Logger::trace("resolution method", string("time-to-default"));
  }
  else if (idata->params->smethod == "rating-path") {
    ttdmethod = false;
    numcopulas = STEPS+1;
    Logger::trace("resolution method", string("rating-path"));
  }
  else {
    throw Exception("MonteCarlo::initParams(): unknow smethod");
  }

  // fixing variance reduction method
  antithetic = idata->params->antithetic;
  Logger::trace("antithetic mode", Parser::bool2string(antithetic));

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
// initRatingPath
//===========================================================================
void ccruncher::MonteCarlo::initRatingPath(const IData *idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting transition matrix", '-');
  Logger::newIndentLevel();

  // setting logger info
  string sval = Parser::int2string(idata->transitions->size());
  Logger::trace("matrix dimension", sval + "x" + sval);
  Logger::trace("initial period (in months)", Parser::int2string(idata->transitions->period));
  Logger::trace("scaling matrix to step length (in months)", Parser::int2string(STEPLENGTH));

  // finding transition matrix for steplength time
  mtrans = translate(idata->transitions, STEPLENGTH);

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initTimeToDefault
//===========================================================================
void ccruncher::MonteCarlo::initTimeToDefault(IData *idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting survival function", '-');
  Logger::newIndentLevel();

  if (idata->survival != NULL)
  {
    survival = idata->survival;
    Logger::trace("survival function", string("user defined"));
  }
  else
  {
    // setting logger info
    string sval = Parser::int2string(idata->transitions->size());
    Logger::trace("transition matrix dimension", sval + "x" + sval);
    Logger::trace("initial period (in months)", Parser::int2string(idata->transitions->period));

    // computing survival function using transition matrix
    double **aux = Arrays<double>::allocMatrix(idata->transitions->n, STEPS+1);
    ccruncher::survival(idata->transitions, STEPLENGTH, STEPS+1, aux);
    int *itime = Arrays<int>::allocVector(STEPS+1);
    for (int i=0;i<=STEPS;i++) {
      itime[i] = i*STEPLENGTH;
    }

    // creating survival function object
    survival = new Survival(idata->ratings, STEPS+1, itime, aux, 2*(STEPS+1)*STEPLENGTH);
    Arrays<double>::deallocMatrix(aux, idata->transitions->n);
    Arrays<int>::deallocVector(itime);
    Logger::trace("transition matrix -> survival function", string("computed"));

    // appending survival object to idata (will dealloc survival object)
    idata->survival = survival;
  }

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
// copula construction
//===========================================================================
CopulaNormal** ccruncher::MonteCarlo::initCopulas(double **ccm, long n, int k, long seed) throw(Exception)
{
  CopulaNormal **ret = NULL;

  // setting logger header
  Logger::trace("initializing copulas", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("copula dimension", Parser::long2string(n));
  Logger::trace("number of copulas", Parser::int2string(k));
  Logger::trace("seed used to initialize randomizer (0=none)", Parser::long2string(seed));

  // allocating space
  try
  {
    ret = new CopulaNormal*[k];
    for (int i=0;i<k;i++) ret[i] = NULL;

    // creem l'objecte per la generacio de copules normals
    ret[0] = new CopulaNormal(n, ccm);

    // el repliquem per tots els talls temporals
    for(int i=1;i<k;i++)
    {
      ret[i] = new CopulaNormal(*(ret[0]));
    }
  }
  catch(std::exception &e)
  {
    if (ret != NULL)
    {
      for (int i=0;i<k;i++)
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
    for(int i=0;i<k;i++)
    {
      ret[i]->setSeed(seed+(long)i);
    }
  }

  // exit function
  Logger::previousIndentLevel();
  return ret;
}

//===========================================================================
// initTimeToDefaultArray
//===========================================================================
int* ccruncher::MonteCarlo::initTimeToDefaultArray(int n) throw(Exception)
{
  int *ret = NULL;

  // setting logger header
  Logger::trace("initializing time-to-default workspace", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("workspace dimension (= number of clients)", Parser::long2string(n));

  // allocating space
  ret = Arrays<int>::allocVector(n);

  // exit function
  Logger::previousIndentLevel();
  return ret;
}

//===========================================================================
// initAggregators
//===========================================================================
void ccruncher::MonteCarlo::initAggregators(const IData *idata) throw(Exception)
{
  long numsegments = 0;

  // init only if spath is set
  if (fpath == "")
  {
    return;
  }

  // setting logger header
  Logger::trace("initializing aggregators", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("output data directory", File::normalizePath(fpath));
  Logger::trace("number of segmentations defined", Parser::int2string(idata->segmentations->getSegmentations().size()));
  Logger::trace("elapsed time initializing aggregators", true);

  // setting objects
  interests = idata->interests;
  segmentations = idata->segmentations;
  aggregators.clear();

  // allocating and initializing aggregators
  for(int i=0;i<segmentations->getNumSegmentations();i++)
  {
    for(int j=0;j<segmentations->getNumSegments(i);j++)
    {
      // allocating SegmentAggregator
      SegmentAggregator *tmp = new SegmentAggregator();

      // asigning a filename
      string filename = segmentations->getSegmentationName(i) + "-" + segmentations->getSegmentName(i, j) + ".out";

      // initializing SegmentAggregator
      tmp->define(i, j, segmentations->getComponents(i));
      tmp->setOutputProperties(fpath, filename, bforce, 0);
      tmp->initialize(dates, STEPS+1, clients, N, interests);

      // adding aggregator to list (only if have elements)
      numsegments++;
      if (tmp->getNumElements() > 0) {
        // creating output file
        if (fpath != "") tmp->touch();
        // add aggregator to list
        aggregators.push_back(tmp);
      } else {
        delete tmp;
      }
    }
  }

  // setting logger info
  Logger::trace("number of segments", Parser::long2string(numsegments));
  Logger::trace("number of segments with elements", Parser::long2string(aggregators.size()));

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
    while(moreiterations)
    {
      // generating random numbers + simulating time-to-default for each client
      sw1.resume();
      simulate();
      sw1.stop();

      // portfolio evaluation
      sw2.resume();
      evalueAggregators();
      sw2.stop();

      // counter increment
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

    // flushing aggregators
    for(unsigned int i=0;i<aggregators.size();i++) {
      aggregators[i]->flush();
    }

    // printing traces
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
// generating random numbers + simulating time-to-default for each client
// put result in rpaths[iclient]
// @exception en cas d'error
//===========================================================================
void ccruncher::MonteCarlo::simulate()
{
  // generate a new realization for copulas
  if (!antithetic)
  {
    for(int i=(ttdmethod?0:1);i<numcopulas;i++)
    {
      copulas[i]->next();
    }
  }
  else // antithetic == true
  {
    if (!reversed)
    {
      for(int i=(ttdmethod?0:1);i<numcopulas;i++)
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

  // for each client we simule the time where defaults
  if (ttdmethod) {
    // using time-to-default algorithm ...
    for (int i=0;i<N;i++) {
      ittd[i] = simTimeToDefault(i);
    }
  }
  else {
    // using rating-path algorithm ...
    for (int i=0;i<N;i++) {
      ittd[i] = simRatingPath(i);
    }
  }
}

//===========================================================================
// getRandom. Returns requested copula value
// encapsules antithetic management
//===========================================================================
double ccruncher::MonteCarlo::getRandom(int icopula, int iclient)
{
  if (antithetic)
  {
    if (reversed)
    {
      return copulas[icopula]->get(iclient);
    }
    else
    {
      return 1.0 - copulas[icopula]->get(iclient);
    }
  }
  else
  {
    return copulas[icopula]->get(iclient);
  }
}

//===========================================================================
// given a client, simule using rating-path algorithm
//===========================================================================
int ccruncher::MonteCarlo::simRatingPath(int iclient)
{
  int indexdefault = mtrans->getIndexDefault();
  int r1, r2;
  double u;

  // rating at t0 is initial rating
  r1 = (*clients)[iclient]->irating;

  // simulate rating at each time-tranch
  for (int t=1;t<=STEPS;t++)
  {
    // getting random number U[0,1] (correlated with rest of clients...)
    u = getRandom(t, iclient);

    // compute next rating
    r2 = mtrans->evalue(r1, u);

    // check if client has defaulted
    if (r2 == indexdefault)
    {
      return t;
    }
    else
    {
      r1 = r2;
    }
  }

  // if non defaults, return a number bigger than STEPS
  return STEPS+1;
}

//===========================================================================
// given a client, simule using time-to-default algorithm
//===========================================================================
int ccruncher::MonteCarlo::simTimeToDefault(int iclient)
{
  // rating at t0 is initial rating
  int r1 = (*clients)[iclient]->irating;

  // getting random number U[0,1] (correlated with rest of clients...)
  double u = getRandom(0, iclient);

  // simulate month where this client defaults
  int month = survival->inverse(r1, u);

  // return index time where defaults (always bigger than 0)
  return max(1, int(ceil(double(month)/double(STEPLENGTH))));
}

//===========================================================================
// portfolio evaluation using current copula
// @exception if error
//===========================================================================
void ccruncher::MonteCarlo::evalueAggregators() throw(Exception)
{
  // adding one simulation
  for(unsigned int i=0;i<aggregators.size();i++)
  {
    aggregators[i]->append(ittd);
  }
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
