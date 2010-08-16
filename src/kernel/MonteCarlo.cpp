
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

#include <cmath>
#include <cfloat>
#include <algorithm>
#include "kernel/MonteCarlo.hpp"
#include "segmentations/Segmentations.hpp"
#include "math/BlockGaussianCopula.hpp"
#include "math/BlockTStudentCopula.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/File.hpp"
#include <cassert>

#define MAX_NUM_THREADS 16

//===========================================================================
// constructor
//===========================================================================
ccruncher::MonteCarlo::MonteCarlo() : borrowers(), assets(), aggregators(), threads(0)
{
  maxseconds = 0;
  numiterations = 0;
  maxiterations = 0;
  antithetic = false;
  reversed = false;
  seed = 0L;
  hash = 0;
  fpath = "path not set";
  bforce = false;
  copula = NULL;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::MonteCarlo::~MonteCarlo()
{
  release();
}

//===========================================================================
// release
//===========================================================================
void ccruncher::MonteCarlo::release()
{
  // removing threads
  for(unsigned int i=0; i<threads.size(); i++) {
    if (threads[i] != NULL) {
      threads[i]->cancel();
      delete threads[i];
      threads[i] = NULL;
    }
  }
  threads.clear();
  pthread_mutex_destroy(&mutex);

  // deallocating copula
  if (copula != NULL) { 
    delete copula; 
    copula = NULL; 
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
  if (maxiterations != 0)
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
    Logger::trace("initializing procedure", '*');
    Logger::newIndentLevel();

    // initializing parameters
    initParams(idata);

    // initializing borrowers
    initBorrowers(idata);

    // initializing assets
    initAssets(idata);

    // initializing survival functions
    initSurvival(idata);

    // initializing copula
    initCopula(idata, idata.getParams().copula_seed);

    // initializing aggregators
    initAggregators(idata);

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
void ccruncher::MonteCarlo::initParams(IData &idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting parameters", '-');
  Logger::newIndentLevel();

  // max number of seconds
  maxseconds = idata.getParams().maxseconds;
  Logger::trace("maximum execution time (in seconds)", Format::toString(maxseconds));

  // max number of iterations
  maxiterations = idata.getParams().maxiterations;
  Logger::trace("maximum number of iterations", Format::toString(maxiterations));

  // printing initial date
  time0 = idata.getParams().time0;
  Logger::trace("initial date", Format::toString(time0));
  // printing end date
  timeT = idata.getParams().timeT;
  Logger::trace("end date", Format::toString(timeT));

  // fixing variance reduction method
  antithetic = idata.getParams().antithetic;
  Logger::trace("antithetic mode", Format::toString(antithetic));
  reversed = false;

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initBorrowers
//===========================================================================
void ccruncher::MonteCarlo::initBorrowers(IData &idata) throw(Exception)
{
  // doing assertions
  assert(borrowers.size() == 0);

  // setting logger header
  Logger::trace("setting borrowers to simulate", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("simulate only active borrowers", Format::toString(idata.getParams().onlyactive));
  Logger::trace("number of initial borrowers", Format::toString(idata.getPortfolio().getBorrowers().size()));

  // determining the borrowers to simulate
  bool onlyactive = idata.getParams().onlyactive;
  vector<Borrower *> &vborrowers = idata.getPortfolio().getBorrowers();
  borrowers.reserve(vborrowers.size());
  for(unsigned int i=0; i<vborrowers.size(); i++)
  {
    if (onlyactive)
    {
      if (vborrowers[i]->isActive(time0, timeT)) 
      {
        borrowers.push_back(SimulatedBorrower(vborrowers[i]));
      }
    }
    else
    {
      borrowers.push_back(SimulatedBorrower(vborrowers[i]));
    }
  }
  
  // important: sorting borrowers list by sector and rating
  sort(borrowers.begin(), borrowers.end());
  Logger::trace("number of simulated borrowers", Format::toString(borrowers.size()));

  // checking that exist borrowers to simulate
  if (borrowers.size() == 0)
  {
    throw Exception("error initializing borrowers: 0 borrowers to simulate");
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initAssets
//===========================================================================
void ccruncher::MonteCarlo::initAssets(IData &idata) throw(Exception)
{
  // doing assertions
  assert(assets.size() == 0);

  // setting logger header
  Logger::trace("setting assets to simulate", '-');
  Logger::newIndentLevel();

  // note: this is the place where it must have the asset losses precomputation.
  // Asset losses has been moved to Borrower:insertAsset() with the purpose of 
  // being able flush memory on asset events just after precomputation because 
  // in massive portfolios memory can be exhausted

  // determining the assets to simulate
  int numassets = 0;
  assets.reserve(4*borrowers.size());
  for(unsigned int i=0; i<borrowers.size(); i++)
  {
    vector<Asset*> &vassets = borrowers[i].ref->getAssets();
    for(unsigned int j=0; j<vassets.size(); j++)
    {
      numassets++;
      if (vassets[j]->isActive(time0, timeT)) 
      {
        assets.push_back(SimulatedAsset(vassets[j], i));
      }
    }
  }
  Logger::trace("number of initial assets", Format::toString(numassets));
  Logger::trace("number of simulated assets", Format::toString(assets.size()));

  // checking that exist assets to simulate
  if (assets.size() == 0)
  {
    throw Exception("error initializing assets: 0 assets to simulate");
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initSurvival
//===========================================================================
void ccruncher::MonteCarlo::initSurvival(IData &idata) throw(Exception)
{
  // doing assertions
  assert(survival.size() == 0);

  // setting logger header
  Logger::trace("setting survival function", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of ratings", Format::toString(idata.getRatings().size()));

  if (idata.hasSurvival())
  {
    // setting survival function
    survival = idata.getSurvival();
    Logger::trace("survival function", string("user defined"));

    // checking that survival function is defined for t <= timeT
    int months = idata.getSurvival().getMinCommonTime();
    Date aux = addMonths(time0, months);
    if (aux < timeT)
    {
      throw Exception("survival function not defined at t=" + Format::toString(timeT));
    }
  }
  else
  {
    // setting logger info
    string sval = Format::toString(idata.getTransitionMatrix().size());
    Logger::trace("transition matrix dimension", sval + "x" + sval);
    Logger::trace("transition matrix period (in months)", Format::toString(idata.getTransitionMatrix().getPeriod()));

    // computing survival function using transition matrix
    int months = (int) ceil(time0.getMonthsTo(timeT));
    survival = idata.getTransitionMatrix().getSurvival(1, months+1);
    Logger::trace("transition matrix -> survival function", string("computed"));
    TransitionMatrix tm1 = idata.getTransitionMatrix().scale(1);
    double rerror = tm1.getRegularizationError();
    Logger::trace("regularization error", Format::toString(rerror));
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// copula construction
//===========================================================================
void ccruncher::MonteCarlo::initCopula(IData &idata, long seed_) throw(Exception)
{
  timer.start();

  // doing assertions
  assert(copula == NULL);

  // setting logger header
  Logger::trace("initializing copula", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of sectors", Format::toString(idata.getSectors().size()));

  // setting logger info
  Logger::trace("copula type", idata.getParams().copula_type);
  Logger::trace("copula dimension", Format::toString(borrowers.size()));
  Logger::trace("seed used to initialize randomizer (0=none)", Format::toString(seed_));

  try
  {
    vector<int> tmp(idata.getCorrelationMatrix().size(),0);

    // computing the number of borrowers in each sector
    for(unsigned int i=0; i<borrowers.size(); i++)
    {
      tmp[borrowers[i].ref->isector]++;
    }

    // creating the copula object
    if (idata.getParams().getCopulaType() == "gaussian")
    {
      copula = new BlockGaussianCopula(idata.getCorrelationMatrix().getMatrix(), &tmp[0], idata.getCorrelationMatrix().size());
      double cnum = ((BlockGaussianCopula*)copula)->getConditionNumber();
      Logger::trace("cholesky condition number", Format::toString(cnum));
    }
    else if (idata.getParams().getCopulaType() == "t")
    {
      double ndf = idata.getParams().getCopulaParam();
      copula = new BlockTStudentCopula(idata.getCorrelationMatrix().getMatrix(), &tmp[0], idata.getCorrelationMatrix().size(), ndf);
      double cnum = ((BlockTStudentCopula*)copula)->getConditionNumber();
      Logger::trace("cholesky condition number", Format::toString(cnum));
    }
    else 
    {
      throw Exception("invalid copula type");
    }
  }
  catch(std::exception &e)
  {
    // copula deallocated by release method
    throw Exception(e, "error ocurred while initializing copula");
  }

  // if no seed is given /dev/urandom or time() will be used
  seed = seed_;
  if (seed == 0L)
  {
    // use a seed based on clock
    seed = Utils::trand();
  }

  // seeding random number generators
  // see SimulationThread constructor
  copula->setSeed(seed);

  // exit function
  Logger::trace("elapsed time initializing copula", timer);
  Logger::previousIndentLevel();
}

//===========================================================================
// initAggregators
//===========================================================================
void ccruncher::MonteCarlo::initAggregators(IData &idata) throw(Exception)
{
  timer.start();
  segmentations = &(idata.getSegmentations());

  // assertions
  assert(fpath != "" && fpath != "path not set"); 
  assert(aggregators.size() == 0); 

  // setting logger header
  Logger::trace("initializing aggregators", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("output data directory", fpath);
  Logger::trace("number of segmentations", Format::toString(segmentations->size()));

  // allocating and initializing aggregators
  aggregators.clear();
  for(int i=0; i<segmentations->size(); i++)
  {
    string filename = File::normalizePath(fpath) + segmentations->getSegmentation(i).name + ".csv";
    Aggregator *aggregator = new Aggregator(assets, i, segmentations->getSegmentation(i), filename, bforce);
    aggregators.push_back(aggregator);
  }

  // exit function
  Logger::trace("elapsed time initializing aggregators", timer);
  Logger::previousIndentLevel();
}

//===========================================================================
// execute
//===========================================================================
int ccruncher::MonteCarlo::execute(int numthreads) throw(Exception)
{
  // assertions
  assert(1 <= numthreads && numthreads <= MAX_NUM_THREADS); 
  assert(fpath != "" && fpath != "path not set"); 

  if (numthreads <= 0 || MAX_NUM_THREADS < numthreads)
  {
    throw Exception("invalid number of threads");
  }

  // setting logger header
  Logger::addBlankLine();
  Logger::trace("running Monte Carlo" + (hash==0?"": " [" + Format::toString(hash) + " simulations per hash]"), '*');
  Logger::newIndentLevel();

  // creating and launching simulation threads
  timer.start();
  numiterations = 0;
  pthread_mutex_init(&mutex, NULL);
  threads.assign(numthreads, NULL);
  for(int i=0; i<numthreads; i++)
  {
    threads[i] = new SimulationThread(*this, seed+i);
    threads[i]->start(); // to avoid threads use run()
  }

  // awaiting threads
  for(int i=0; i<numthreads; i++)
  {
    threads[i]->wait();
  }

  // flushing aggregators
  for(unsigned int i=0; i<aggregators.size(); i++) 
  {
    aggregators[i]->flush();
  }

  // exit function
  Logger::trace("total simulation time", Timer::format(timer.read()));
  Logger::addBlankLine();
  Logger::previousIndentLevel();
  return numiterations;
}

//===========================================================================
// append a simulation result
//===========================================================================
bool ccruncher::MonteCarlo::append(vector<vector<double> > &losses)
{
  assert(losses.size() == aggregators.size());
  pthread_mutex_lock(&mutex);
  bool more = true;

  try
  {
    // aggregating simulation result
    for(unsigned int i=0; i<aggregators.size(); i++) 
    {
      aggregators[i]->append(losses[i]);
    }

    // counter increment
    numiterations++;

    // printing hashes
    if (hash > 0 && numiterations%hash == 0)
    {
      Logger::append(".");
    }

    // checking stop criteria
    if (maxiterations > 0 && numiterations >= maxiterations)
    {
      more = false;
    }

    // checking stop criteria
    if (maxseconds > 0 && timer.read() >  maxseconds)
    {
      more = false;
    }
  }
  catch(Exception &e)
  {
    pthread_mutex_unlock(&mutex);
    throw Exception(e, "error ocurred while executing Monte Carlo");
  }

  // exit function
  pthread_mutex_unlock(&mutex);
  return(more);
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

