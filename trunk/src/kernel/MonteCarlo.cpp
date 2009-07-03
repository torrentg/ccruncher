
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

#include <cmath>
#include <cfloat>
#include "kernel/MonteCarlo.hpp"
#include "segmentations/Segmentations.hpp"
#include "math/BlockGaussianCopula.hpp"
#include "math/BlockTStudentCopula.hpp"
#include "utils/Utils.hpp"
#include "utils/Arrays.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/File.hpp"
#include "utils/ccmpi.h"
#include <cassert>

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
  N = 0L;
  CONT = 0L;
  antithetic = false;
  reversed = false;

  hash = 0;
  fpath = "path not set";
  bforce = false;

  ratings = NULL;
  sectors = NULL;
  borrowers = NULL;

  copula = NULL;
  survival = NULL;
  ttd = NULL;

  blcopulas = false;
  bldeftime = false;

  ccmaxbufsize = CCMAXBUFSIZE;
}

//===========================================================================
// release
//===========================================================================
void ccruncher::MonteCarlo::release()
{
  // deallocating copula
  if (copula != NULL) { delete copula; copula = NULL; }

  // survival fuction object deallocated by IData (its container)

  // deallocating workspace array
  if (ttd != NULL) {
    Arrays<Date>::deallocVector(ttd);
    ttd = NULL;
  }

  // dropping aggregators elements
  for(unsigned int i=0;i<aggregators.size();i++) {
    if (aggregators[i] != NULL) {
      delete aggregators[i];
      aggregators[i] = NULL;
    }
  }
  aggregators.clear();

  // closing files
  if (blcopulas) fcopulas.close();
  if (bldeftime) fdeftime.close();
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::MonteCarlo::initialize(IData &idata, bool only_validation) throw(Exception)
{
  if (MAXITERATIONS != 0L)
  {
    throw Exception("Monte Carlo reinitialization not allowed");
  }
  else if (fpath == "path not set")
  {
    throw Exception("error initializing Monte Carlo: output files path not established");
  }

  try
  {
    Logger::addBlankLine();
    Logger::trace("initialing procedure", '*');
    Logger::newIndentLevel();

    // initializing parameters
    initParams(idata);

    // initializing borrowers
    initBorrowers(idata);

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

    if (!only_validation)
    {
      // initializing aggregators
      initAggregators(idata);

      // initializes debug bulk files 
      initAdditionalOutput();
    }

    // exit function
    Logger::previousIndentLevel();
  }
  catch(Exception &e)
  {
    release();
    throw Exception(e, "error initializing Monte Carlo");
  }
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
  time0 = idata.getParams().time0;
  Logger::trace("initial date", Format::date2string(time0));
  // printing end date
  timeT = idata.getParams().timeT;
  Logger::trace("end date", Format::date2string(timeT));

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
// initBorrowers
//===========================================================================
void ccruncher::MonteCarlo::initBorrowers(const IData &idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting borrowers to simulate", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("simulate only active borrowers", Format::bool2string(idata.getParams().onlyactive));
  Logger::trace("number of initial borrowers", Format::long2string(idata.getPortfolio().getBorrowers().size()));

  // sorting borrowers by sector and rating
  bool onlyactive = idata.getParams().onlyactive;
  idata.getPortfolio().sortBorrowers(time0, timeT, onlyactive);

  // fixing number of borrowers
  if (idata.getParams().onlyactive)
  {
    N = idata.getPortfolio().getNumActiveBorrowers(time0, timeT);
  }
  else
  {
    N = idata.getPortfolio().getBorrowers().size();
  }

  Logger::trace("number of simulated borrowers", Format::long2string(N));

  // checking that exist borrowers to simulate
  if (N == 0)
  {
    throw Exception("error initializing borrowers: 0 borrowers to simulate");
  }

  // setting borrowers object
  borrowers = &(idata.getPortfolio().getBorrowers());

  // note: this is the place where it must have the asset losses precomputation.
  // Asset losses has been moved to Borrower:insertAsset() with the purpose of 
  // being able flush memory on asset events just after precomputation because 
  // in massive portfolios memory can be exhausted

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
    // checking that survival function is defined for t <= timeT
    int months = idata.getSurvival().getMinCommonTime();
    Date aux = addMonths(time0, months);
    if (aux < timeT)
    {
      throw Exception("survival function not defined at t=" + Format::date2string(timeT));
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
    Logger::trace("transition matrix period (in months)", Format::int2string(idata.getTransitionMatrix().getPeriod()));

    // computing survival function using transition matrix
    int months = (int) ceil(time0.getMonthsTo(timeT));
    double **aux = Arrays<double>::allocMatrix(idata.getTransitionMatrix().size(), months+1);
    ccruncher::survival(idata.getTransitionMatrix(), 1, months+1, aux);
    int *itime = Arrays<int>::allocVector(months+1);
    for (int i=0;i<=months;i++) {
      itime[i] = i;
    }

    // creating survival function object
    survival = new Survival(idata.getRatings(), months+1, itime, aux);
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
// getBorrowerCorrelationMatrix
// method not used because can exhaust memory with large portfolios (eg. N=50000)
//===========================================================================
double ** ccruncher::MonteCarlo::getBorrowerCorrelationMatrix(const IData &idata)
{
  double **ret = Arrays<double>::allocMatrix(N, N, 0.0);
  double **scorrels = idata.getCorrelationMatrix().getMatrix();

  for (long i=0;i<N;i++)
  {
    int sector1 = (*borrowers)[i]->isector;
    for(int j=0;j<N;j++)
    {
      int sector2 = (*borrowers)[j]->isector;
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
  Logger::trace("copula type", idata.getParams().copula_type);
  Logger::trace("copula dimension", Format::long2string(N));
  Logger::trace("seed used to initialize randomizer (0=none)", Format::long2string(seed));
  Logger::trace("elapsed time initializing copula", true);

  try
  {
    // allocating temporal memory
    tmp = Arrays<int>::allocVector(idata.getCorrelationMatrix().size(),0);

    // computing the number of borrowers in each sector
    for(long i=0; i<N; i++)
    {
      tmp[(*borrowers)[i]->isector]++;
    }

    // creating the copula object
    if (idata.getParams().getCopulaType() == "gaussian")
    {
      copula = new BlockGaussianCopula(idata.getCorrelationMatrix().getMatrix(), tmp, idata.getCorrelationMatrix().size());
    }
    else if (idata.getParams().getCopulaType() == "t")
    {
      double ndf = idata.getParams().getCopulaParam();
      copula = new BlockTStudentCopula(idata.getCorrelationMatrix().getMatrix(), tmp, idata.getCorrelationMatrix().size(), ndf);
    }
    else {
      throw Exception("invalid copula type");
    }

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
    throw Exception(e, "error ocurred while initializing copula");
  }

  // if no seed is given /dev/urandom or time() will be used
  if (seed == 0L)
  {
    if (Utils::isMaster())
    {
      // use a seed based on clock
      seed = Utils::trand();
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
  assert(ttd == NULL);

  // allocating space
  ttd = Arrays<Date>::allocVector(n);
}

//===========================================================================
// initAggregators
//===========================================================================
void ccruncher::MonteCarlo::initAggregators(const IData &idata) throw(Exception)
{
  Segmentations &segmentations = idata.getSegmentations();

  // assertions
  assert(fpath != "" && fpath != "path not set"); 

  // setting logger header
  Logger::trace("initializing aggregators", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("output data directory", fpath);
  Logger::trace("number of segmentations", Format::int2string(segmentations.size()));
  Logger::trace("elapsed time initializing aggregators", true);

  // allocating and initializing aggregators
  aggregators.clear();
  for(int i=0;i<segmentations.size();i++)
  {
    string filename = File::normalizePath(fpath) + segmentations[i].name + ".csv";
    Aggregator *aggregator = new Aggregator(aggregators.size(), segmentations[i], *borrowers, N);
    aggregator->setOutputProperties(filename, bforce);
    aggregators.push_back(aggregator);
  }

#ifdef USE_MPI
  ccmaxbufsize = CCMAXBUFSIZE;
  for(unsigned int i=0; i<aggregators.size(); i++) {
    long numbytes = aggregators[i]->getBufferSize();
    if (numbytes > ccmaxbufsize) {
      ccmaxbufsize = numbytes;
    }
  }
#endif

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
  assert(fpath != "" && fpath != "path not set"); 

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

      // simulating default time for each borrower
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
    Logger::trace("elapsed time simulating default times", Timer::format(timer2.read()));
    Logger::trace("elapsed time aggregating data", Timer::format(timer3.read()));
    Logger::trace("total simulation time", Timer::format(timer1.read()+timer2.read()+timer3.read()));
  }
  catch(Exception &e)
  {
    throw Exception(e, "error ocurred while executing Monte Carlo");
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
  double data[ccmaxbufsize];
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
          MPI::COMM_WORLD.Recv(&data, datalength, MPI::DOUBLE, rankid, MPI_TAG_DATA);
          // aggregating data
          long numsims = aggregators[aggregatorid]->appendRawData(data, datalength);

          // updating simulations counter
          if (aggregatorid == 0)
          {
            for(int i=0;i<numsims;i++)
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
        throw Exception("unexpected MPI tag");
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

  // trace copula values
  if (blcopulas) 
  {
    printCopulaValues();
  }
}

//===========================================================================
// simulate time-to-default for each borrower
// put result in rpaths[iborrower]
//===========================================================================
void ccruncher::MonteCarlo::simulate()
{
  // simulates default times
  for (long i=0; i<N; i++) 
  {
    ttd[i] = simTimeToDefault(i);
  }

  // trace default times
  if (bldeftime) 
  {
    printDefaultTimes(ttd);
  }
}

//===========================================================================
// getRandom. Returns requested copula value
// encapsules antithetic management
//===========================================================================
double ccruncher::MonteCarlo::getRandom(int iborrower)
{
  if (antithetic)
  {
    if (reversed)
    {
      return copula->get(iborrower);
    }
    else
    {
      return 1.0 - copula->get(iborrower);
    }
  }
  else
  {
    return copula->get(iborrower);
  }
}

//===========================================================================
// given a borrower, simule time to default
//===========================================================================
Date ccruncher::MonteCarlo::simTimeToDefault(int iborrower)
{
  // rating at t0 is initial rating
  int r1 = (*borrowers)[iborrower]->irating;

  // getting random number U[0,1] (correlated with rest of borrowers...)
  double u = getRandom(iborrower);

  // simulate time where this borrower defaults (in months)
  double t = survival->inverse(r1, u);

  // return simulated default date
  // TODO: assumed no leap years (this can be improved)
  return time0 + (long)(t*365.0/12.0);
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
    if (aggregators[i]->append(ttd) == false)
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

//===========================================================================
// setAdditionalOutput
//===========================================================================
void ccruncher::MonteCarlo::setAdditionalOutput(bool copulas, bool deftimes)
{
  blcopulas = copulas;
  bldeftime = deftimes;
}

//===========================================================================
// initAdditionalOutput
//===========================================================================
void ccruncher::MonteCarlo::initAdditionalOutput() throw(Exception)
{
#ifndef USE_MPI
  assert(fpath != "" && fpath != "path not set"); 
  string dirpath = File::normalizePath(fpath);
  
  // initializes copula values file
  if (blcopulas)
  {
    string filename = dirpath + "copula.csv";
    try
    {
      fcopulas.open(filename.c_str(), ios::out|ios::trunc);
      for(long i=0; i<N; i++)
      {
        fcopulas << "\"" << (*borrowers)[i]->name << "\"" << (i!=N-1?", ":"");
      }
      fcopulas << endl;
    }
    catch(...)
    {
      throw Exception("error writing file " + filename);
    }
  }

  // initializes simulated default times file
  if (bldeftime)
  {
    string filename = dirpath + "deftimes.csv";
    try
    {
      fdeftime.open(filename.c_str(), ios::out|ios::trunc);
      for(long i=0; i<N; i++)
      {
        fdeftime << "\"" << (*borrowers)[i]->name << "\"" << (i!=N-1?", ":"");
      }
      fdeftime << endl;
    }
    catch(...)
    {
      throw Exception("error writing file " + filename);
    }
  }
#endif
}

//===========================================================================
// printCopulaValues
//===========================================================================
void ccruncher::MonteCarlo::printCopulaValues() throw(Exception)
{
#ifndef USE_MPI
  for(long i=0; i<N; i++) 
  {
    fcopulas << getRandom(i) << (i!=N-1?", ":"");
  }
  fcopulas << "\n";
#endif
}

//===========================================================================
// printDefaultTimes
//===========================================================================
void ccruncher::MonteCarlo::printDefaultTimes(Date *values) throw(Exception)
{
#ifndef USE_MPI
  for(long i=0; i<N; i++) 
  {
    fdeftime << values[i] << (i!=N-1?", ":"");
  }
  fdeftime << "\n";
#endif
}

