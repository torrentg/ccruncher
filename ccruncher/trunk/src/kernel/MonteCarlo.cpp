// 
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
// Montecarlo.cpp - MonteCarlo code - $Rev$
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
// 2005/07/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added mpi support
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added class Format (previously format function included in Parser)
//
// 2005/07/24 - Gerard Torrent [gerard@fobos.generacio.com]
//   . class CopulaNormal renamed to GaussianCopula
//   . GaussianCopula replaced by BlockGaussianCopula
//
// 2005/07/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . execute() method returns number of realized simulations
//
// 2005/07/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . moved <cassert> include at last position
//
// 2005/07/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added new check over survival defined user
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented MPI support
//
// 2005/09/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added param montecarlo.simule
//
// 2005/09/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed date list output
//
// 2005/09/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added onlyactive argument to sortClients() method
//
// 2005/09/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added method randomize()
//   . reindexed copulas vector
//
// 2005/09/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added timer at random number generation step
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/10/23 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed some method signatures
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Sectors refactoring
//   . Ratings refactoring
//   . Segmentations refactoring
//   . Survival refactoring
//   . TransitionMatrix refactoring
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Portfolio refactoring
//   . IData refactoring
//   . SegmentAggregator refactoring
//   . MonteCarlo refactoring
//   . generic copula array
//
// 2006/01/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed simule and method params
//
// 2006/12/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . solved bug in mpi version (SegmentAggregator::touch() 
//     called by all ranks, not just master)
//
//===========================================================================

#include <cfloat>
#include <MersenneTwister.h>
#include "kernel/MonteCarlo.hpp"
#include "math/BlockGaussianCopula.hpp"
#include "math/GaussianCopula.hpp"
#include "utils/Utils.hpp"
#include "utils/Arrays.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include <cassert>

#ifdef USE_MPI
  #include <mpi.h>
  #define MPI_VAL_WORK 1025   // work tag (see task variable)
  #define MPI_VAL_STOP 1026   // stop tag (see task variable)
  #define MPI_TAG_DATA 1027   // data tag (used to send results)
  #define MPI_TAG_INFO 1028   // info tag (used to send results)
  #define MPI_TAG_TASK 1029   // task tag (used to send results)
  #define MPI_TAG_EXIT 1030   // task tag (used to send results)
#endif

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

  hash = 0;
  fpath = "path not set";
  bforce = false;

  ratings = NULL;
  sectors = NULL;
  clients = NULL;

  copula = NULL;
  survival = NULL;
  mtrans = NULL;
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

  // deallocating copula
  if (copula != NULL) { delete copula; copula = NULL; }

  // survival fuction object deallocated by IData (its container)

  // deallocating dates vector
  if (dates != NULL) {
    Arrays<Date>::deallocVector(dates);
    dates = NULL;
  }

  // deallocating workspace array
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
void ccruncher::MonteCarlo::initialize(IData &idata) throw(Exception)
{
  if (MAXITERATIONS != 0L)
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
void ccruncher::MonteCarlo::init(IData &idata) throw(Exception)
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

  // initializing survival function
  initTimeToDefault(idata);

  // initializing copula
  initCopula(idata, idata.getParams().copula_seed);

  // ratings paths allocation
  initTimeToDefaultArray(N);

  // initializing aggregators
  initAggregators(idata);

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initParams
//===========================================================================
void ccruncher::MonteCarlo::initParams(const IData &idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting parameters", '-');
  Logger::newIndentLevel();

  // max number of seconds
  MAXSECONDS = idata.getParams().maxseconds;
  Logger::trace("maximum execution time (in seconds)", Format::long2string(MAXSECONDS));

  // max number of iterations
  MAXITERATIONS = idata.getParams().maxiterations;
  Logger::trace("maximum number of iterations", Format::long2string(MAXITERATIONS));

  // printing initial date
  Logger::trace("initial date", Format::date2string(idata.getParams().begindate));
  // fixing step number
  STEPS = idata.getParams().steps;
  Logger::trace("number of time steps", Format::int2string(STEPS));

  // fixing steplength
  STEPLENGTH = idata.getParams().steplength;
  Logger::trace("length of each time step (in months)", Format::int2string(STEPLENGTH));

  // fixing time-tranches
  begindate = idata.getParams().begindate;
  dates = idata.getParams().getDates();

  // fixing variance reduction method
  antithetic = idata.getParams().antithetic;
  Logger::trace("antithetic mode", Format::bool2string(antithetic));

  // initializing internal variables
  CONT = 0L;
  reversed = false;

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initClients
//===========================================================================
void ccruncher::MonteCarlo::initClients(const IData &idata, Date *idates, int isteps) throw(Exception)
{
  // setting logger header
  Logger::trace("fixing clients to simulate", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("simulate only active clients", Format::bool2string(idata.getParams().onlyactive));
  Logger::trace("number of initial clients", Format::long2string(idata.getPortfolio().getClients().size()));

  // sorting clients by sector and rating
  idata.getPortfolio().sortClients(idates[0], idates[isteps], idata.getParams().onlyactive);

  // fixing number of clients
  if (idata.getParams().onlyactive)
  {
    N = idata.getPortfolio().getNumActiveClients(idates[0], idates[isteps]);
  }
  else
  {
    N = idata.getPortfolio().getClients().size();
  }

  Logger::trace("number of simulated clients", Format::long2string(N));

  // checking that exist clients to simulate
  if (N == 0)
  {
    throw Exception("MonteCarlo::init(): 0 clients to simulate");
  }

  // setting client object
  clients = &(idata.getPortfolio().getClients());

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initRatings
//===========================================================================
void ccruncher::MonteCarlo::initRatings(const IData &idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting ratings", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of ratings", Format::int2string(idata.getRatings().size()));

  // fixing ratings
  ratings = &(idata.getRatings());

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initSectors
//===========================================================================
void ccruncher::MonteCarlo::initSectors(const IData &idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting sectors", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of sectors", Format::int2string(idata.getSectors().size()));

  // fixing ratings
  sectors = &(idata.getSectors());

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initTimeToDefault
//===========================================================================
void ccruncher::MonteCarlo::initTimeToDefault(IData &idata) throw(Exception)
{
  // doing assertions
  assert(survival == NULL);

  // setting logger header
  Logger::trace("setting survival function", '-');
  Logger::newIndentLevel();

  if (idata.hasSurvival())
  {
    // checking that survival function is defined for t <= STEPS*STEPLENGTH
    if (idata.getSurvival().getMinCommonTime() < STEPS*STEPLENGTH)
    {
      throw Exception("survival function not defined at t=" + Format::int2string(STEPS*STEPLENGTH));
    }

    // setting survival function
    survival = &(idata.getSurvival());
    Logger::trace("survival function", string("user defined"));
  }
  else
  {
    // setting logger info
    string sval = Format::int2string(idata.getTransitionMatrix().size());
    Logger::trace("transition matrix dimension", sval + "x" + sval);
    Logger::trace("initial period (in months)", Format::int2string(idata.getTransitionMatrix().getPeriod()));

    // computing survival function using transition matrix
    double **aux = Arrays<double>::allocMatrix(idata.getTransitionMatrix().size(), STEPS+1);
    ccruncher::survival(idata.getTransitionMatrix(), STEPLENGTH, STEPS+1, aux);
    int *itime = Arrays<int>::allocVector(STEPS+1);
    for (int i=0;i<=STEPS;i++) {
      itime[i] = i*STEPLENGTH;
    }

    // creating survival function object
    survival = new Survival(idata.getRatings(), STEPS+1, itime, aux, 2*(STEPS+1)*STEPLENGTH);
    Arrays<double>::deallocMatrix(aux, idata.getTransitionMatrix().size());
    Arrays<int>::deallocVector(itime);
    Logger::trace("transition matrix -> survival function", string("computed"));

    // appending survival object to idata (will dealloc survival object)
    idata.setSurvival(*survival);
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// getClientCorrelationMatrix
//===========================================================================
double ** ccruncher::MonteCarlo::getClientCorrelationMatrix(const IData &idata)
{
  double **ret = Arrays<double>::allocMatrix(N, N, 0.0);
  double **scorrels = idata.getCorrelationMatrix().getMatrix();

  for (int i=0;i<N;i++)
  {
    int sector1 = (*clients)[i]->isector;
    for(int j=0;j<N;j++)
    {
      int sector2 = (*clients)[j]->isector;
      if (i == j) {
        ret[i][j] = 1.0;
      }
      else {
        ret[i][j] = scorrels[sector1][sector2];
      }
    }
  }

  return ret;
}

//===========================================================================
// copula construction
//===========================================================================
void ccruncher::MonteCarlo::initCopula(const IData &idata, long seed) throw(Exception)
{
  int *tmp = NULL;

  // doing assertions
  assert(copula == NULL);

  // setting logger header
  Logger::trace("initializing copula", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("copula dimension", Format::long2string(N));
  Logger::trace("seed used to initialize randomizer (0=none)", Format::long2string(seed));
  Logger::trace("elapsed time initializing copula", true);

  try
  {
    // allocating temporal memory
    tmp = Arrays<int>::allocVector(idata.getCorrelationMatrix().size(),0);

    // computing the number of clients in each sector
    for(long i=0;i<N;i++)
    {
      tmp[(*clients)[i]->isector]++;
    }

    // creating the copula object
    //copula = new GaussianCopula(N, getClientCorrelationMatrix(idata));
    copula = new BlockGaussianCopula(idata.getCorrelationMatrix().getMatrix(), tmp, idata.getCorrelationMatrix().size());

    // releasing temporal memory
    Arrays<int>::deallocVector(tmp);
    tmp = NULL;
  }
  catch(std::exception &e)
  {
    if (tmp != NULL)
    {
      Arrays<int>::deallocVector(tmp);
      tmp = NULL;
    }

    // copula deallocated by release method
    throw Exception(e, "MonteCarlo::initCopula()");
  }

  // if no seed is given /dev/urandom or time() will be used
  if (seed == 0L)
  {
    if (Utils::isMaster())
    {
      // use a seed based on clock
      MTRand mtrand;
      seed = mtrand.randInt();
    }
#ifdef USE_MPI
    // all nodes knows the same seed
    MPI::COMM_WORLD.Bcast(&seed, 1, MPI::LONG, 0);
#endif
  }

  // seeding random number generators
#ifdef USE_MPI
  // caution: cluster limited to 30000 nodes ;)
  copula->setSeed(seed+30001L*MPI::COMM_WORLD.Get_rank());
#else
  copula->setSeed(seed);
#endif

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initTimeToDefaultArray
//===========================================================================
void ccruncher::MonteCarlo::initTimeToDefaultArray(int n) throw(Exception)
{
  // doing assertions
  assert(ittd == NULL);

  // setting logger header
  Logger::trace("initializing workspace", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("workspace dimension (= number of clients)", Format::long2string(n));

  // allocating space
  ittd = Arrays<int>::allocVector(n);

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initAggregators
//===========================================================================
void ccruncher::MonteCarlo::initAggregators(const IData &idata) throw(Exception)
{
  long numsegments = 0;
  Segmentations &segmentations = idata.getSegmentations();

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
  Logger::trace("number of segmentations defined", Format::int2string(segmentations.size()));
  Logger::trace("elapsed time initializing aggregators", true);

  // setting objects
  aggregators.clear();

  // allocating and initializing aggregators
  for(int i=0;i<segmentations.size();i++)
  {
    for(int j=0;j<segmentations[i].size();j++)
    {
      // allocating SegmentAggregator
      SegmentAggregator *tmp = new SegmentAggregator();

      // asigning a filename
      string filename = segmentations[i].name + "-" + segmentations[i][j].name + ".out";

      // initializing SegmentAggregator
      tmp->define(aggregators.size(), i, j, segmentations[i].components);
      tmp->setOutputProperties(fpath, filename, bforce, 0);
      tmp->initialize(dates, STEPS+1, *clients, N, idata.getInterests());

      // adding aggregator to list (only if have elements)
      numsegments++;
      if (tmp->getNumElements() > 0) {
        // creating output file
        if (fpath != "" && Utils::isMaster()) tmp->touch();
        // add aggregator to list
        aggregators.push_back(tmp);
      } else {
        delete tmp;
      }
    }
  }

  // setting logger info
  Logger::trace("number of segments", Format::long2string(numsegments));
  Logger::trace("number of segments with elements", Format::long2string(aggregators.size()));

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// execute
//===========================================================================
long ccruncher::MonteCarlo::execute() throw(Exception)
{
  long ret;

  // assertions
  assert(fpath != "");

  // setting logger header
  Logger::addBlankLine();
  Logger::trace("running Monte Carlo" + (hash==0?"": " [" + Format::int2string(hash) + " simulations per hash]"), '*');
  Logger::newIndentLevel();

#ifdef USE_MPI
  if (Utils::isMaster())
  {
    ret =executeCollector();
  }
  else
  {
    ret = executeWorker();
  }
#else
  ret = executeWorker();
#endif

  // exit function
  Logger::addBlankLine();
  Logger::previousIndentLevel();
  return ret;
}

//===========================================================================
// executeWorker
//===========================================================================
long ccruncher::MonteCarlo::executeWorker() throw(Exception)
{
  bool aux=false, moreiterations=true;
  Timer timer1, timer2, timer3;

#ifdef USE_MPI
  if (!Utils::isMaster()) {
    // disabling max time criteria for slaves
    MAXSECONDS = 0L;
    // disabling max itaration criteria for slaves
    MAXITERATIONS = 0L;
  }
#endif

  try
  {
    while(moreiterations)
    {
      // generating random numbers
      timer1.resume();
      randomize();
      timer1.stop();

      // simulating default time for each client
      timer2.resume();
      simulate();
      timer2.stop();

      // portfolio evaluation
      timer3.resume();
      aux = evalue();
      timer3.stop();

      // counter increment
      CONT++;

      // printing hashes
      if (hash > 0 && CONT%hash == 0)
      {
        Logger::append(".");
      }

      // checking stop criteria
      if (aux == false)
      {
        moreiterations = false;
      }

      // checking stop criteria
      if (MAXITERATIONS > 0L && CONT >= MAXITERATIONS)
      {
        moreiterations = false;
      }

      // checking stop criteria
      if (MAXSECONDS > 0L && (timer1.read()+timer2.read())> MAXSECONDS)
      {
        moreiterations = false;
      }
    }

    // flushing aggregators
    for(unsigned int i=0;i<aggregators.size();i++) {
      aggregators[i]->flush();
    }

    // printing traces
    Logger::trace("elapsed time creating random numbers", Timer::format(timer1.read()));
    Logger::trace("elapsed time simulating clients", Timer::format(timer2.read()));
    Logger::trace("elapsed time aggregating data", Timer::format(timer3.read()));
    Logger::trace("total simulation time", Timer::format(timer1.read()+timer2.read()+timer3.read()));
  }
  catch(Exception &e)
  {
    throw Exception(e, "MonteCarlo::execute()");
  }

#ifdef USE_MPI
  // notify exit to master
  MPI::COMM_WORLD.Send(NULL, 0, MPI::INT, 0, MPI_TAG_EXIT);
#endif

  // exit function
  return(CONT);
}

//===========================================================================
// executeCollector
// only used in MPI mode. this method is executed by master node
// mpi message flow (data transfer):
//   worker send MPI_TAG_INFO msg
//   worker send MPI_TAG_DATA msg
//   master send MPI_TAG_TASK msg
// mpi message flow (exit notification):
//   worker send MPI_TAG_EXIT
//===========================================================================
long ccruncher::MonteCarlo::executeCollector() throw(Exception)
{
#ifdef USE_MPI
  bool more=true, moreiterations=true;
  double data[CCMAXBUFSIZE];
  int nranks=MPI::COMM_WORLD.Get_size();
  int aggregatorid=0, rankid=0, tagid=0, datalength=0, task=0, nexits=0;
  MPI::Status status;
  Timer timer;

  // initializing values
  timer.start();

  // main master loop
  while(more)
  {
    // awaiting for a slave request
    MPI::COMM_WORLD.Probe(MPI::ANY_SOURCE, MPI::ANY_TAG, status);
    // retrieving rank
    rankid = status.Get_source();
    // retrieving tag
    tagid = status.Get_tag();

    switch(tagid)
    {
      // message EXIT sequence
      case MPI_TAG_EXIT:
        {
          // receiving exit tag
          MPI::COMM_WORLD.Recv(NULL, 0, MPI::INT, rankid, MPI_TAG_EXIT);

          // updating counter
          nexits++;

          // cheking that all ranks have finished
          if (nexits == nranks-1) {
            more = false;
          }

          // finish sequence EXIT
          break;
        }

      // message DATA sequence
      case MPI_TAG_INFO:
        {
          // receiving data
          MPI::COMM_WORLD.Recv(&aggregatorid, 1, MPI::INT, rankid, MPI_TAG_INFO);
          // awaiting for data
          MPI::COMM_WORLD.Probe(rankid, MPI_TAG_DATA, status);
          // retrieving data size
          datalength = status.Get_count(MPI::DOUBLE);
          // receiving data
          MPI::COMM_WORLD.Recv(&data, CCMAXBUFSIZE, MPI::DOUBLE, rankid, MPI_TAG_DATA);

          // updating simulations counter
          if (aggregatorid == 0)
          {
            for(int i=0;i<datalength;i++)
            {
              // counter increment
              CONT++;
              // printing hashes
              if (hash > 0 && CONT%hash == 0)
              {
                Logger::append(".");
              }
            }
          }

          // aggregating data
          aggregators[aggregatorid]->appendRawData(data, datalength);

          // checking stop criteria
          if (MAXITERATIONS > 0L && CONT >= MAXITERATIONS) {
            moreiterations = false;
          }

          // checking stop criteria
          if (MAXSECONDS > 0L && timer.read() >  MAXSECONDS) {
            moreiterations = false;
          }

          // sending new task to slave
          task = (moreiterations?MPI_VAL_WORK:MPI_VAL_STOP);
          MPI::COMM_WORLD.Send(&task, 1, MPI::INT, rankid, MPI_TAG_TASK);

          // finish sequence DATA
          break;
       }

      // other messages cause errors
      default:
        throw Exception("executeCollector: unexpected MPI tag");
    }
  }

  // printing traces
  Logger::trace("elapsed time", Timer::format(timer.read()));

  // return the number of simulations
  return CONT;
#else
  return 0L;
#endif
}

//===========================================================================
// generate random numbers
//===========================================================================
void ccruncher::MonteCarlo::randomize()
{
  // generate a new realization for each copula
  if (!antithetic)
  {
    copula->next();
  }
  else // antithetic == true
  {
    if (!reversed)
    {
      copula->next();
      reversed = true;
    }
    else
    {
      reversed = false;
    }
  }
}

//===========================================================================
// simulate time-to-default for each client
// put result in rpaths[iclient]
//===========================================================================
void ccruncher::MonteCarlo::simulate()
{
  // for each client we simule the time where defaults
  for (int i=0;i<N;i++) {
    ittd[i] = simTimeToDefault(i);
  }
}

//===========================================================================
// getRandom. Returns requested copula value
// encapsules antithetic management
//===========================================================================
double ccruncher::MonteCarlo::getRandom(int iclient)
{
  if (antithetic)
  {
    if (reversed)
    {
      return copula->get(iclient);
    }
    else
    {
      return 1.0 - copula->get(iclient);
    }
  }
  else
  {
    return copula->get(iclient);
  }
}

//===========================================================================
// given a client, simule time to default
//===========================================================================
int ccruncher::MonteCarlo::simTimeToDefault(int iclient)
{
  // rating at t0 is initial rating
  int r1 = (*clients)[iclient]->irating;

  // getting random number U[0,1] (correlated with rest of clients...)
  double u = getRandom(iclient);

  // simulate month where this client defaults
  int month = survival->inverse(r1, u);

  // return index time where defaults (always bigger than 0)
  return std::max(1, int(ceil(double(month)/double(STEPLENGTH))));
}

//===========================================================================
// portfolio evaluation using current copula
// @exception if error
// return true=all ok, continue
// return false=stop montecarlo, someone order you (the master)
//===========================================================================
bool ccruncher::MonteCarlo::evalue() throw(Exception)
{
  bool ret=true;

  // adding current simulation to each aggregator
  for(unsigned int i=0;i<aggregators.size();i++)
  {
    if (aggregators[i]->append(ittd) == false)
    {
      ret = false;
    }
  }

  // exit function
  return ret;
}

//===========================================================================
// setHash
//===========================================================================
void ccruncher::MonteCarlo::setHash(int num)
{
  assert(num >= 0);
  hash = std::max(0, num);
}

//===========================================================================
// setFilePath
//===========================================================================
void ccruncher::MonteCarlo::setFilePath(string path, bool force)
{
  fpath = path;
  bforce = force;
}
