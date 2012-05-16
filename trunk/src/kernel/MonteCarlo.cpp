
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
#include "params/Segmentations.hpp"
#include "math/BlockGaussianCopula.hpp"
#include "math/BlockTStudentCopula.hpp"
#include "math/CopulaCalibration.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/File.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::MonteCarlo::MonteCarlo() : obligors(), assets(NULL), aggregators(), threads(0)
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
  btrace = false;
  bcalib = false;
  copula = NULL;
  running = false;
  assetsize = 0;
  numassets = 0;
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
  // deallocating assets
  if (assets != NULL)
  {
    delete [] assets;
    assets = NULL;
  }

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
  
  // closing files
  if (btrace) 
  {
    fcopulas.close();
  }
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::MonteCarlo::initialize(IData &idata) throw(Exception)
{
  if (numiterations != 0)
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

    // initializing obligors
    initObligors(idata);

    // initializing assets
    initAssets(idata);

    // initializing survival functions
    initSurvival(idata);

    // calibrate copula params (correlations+ndf)
    calibrateCopula(idata);

    // initializing copula
    initCopula(idata, idata.getParams().copula_seed);

    // initializing aggregators
    initAggregators(idata);

    // initialize trace
    initTrace();

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
// initObligors
//===========================================================================
void ccruncher::MonteCarlo::initObligors(IData &idata) throw(Exception)
{
  // doing assertions
  assert(obligors.size() == 0);

  // setting logger header
  Logger::trace("setting obligors to simulate", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("simulate only active obligors", Format::toString(idata.getParams().onlyactive));
  Logger::trace("number of initial obligors", Format::toString(idata.getPortfolio().getObligors().size()));

  // determining the obligors to simulate
  bool onlyactive = idata.getParams().onlyactive;
  vector<Obligor *> &vobligors = idata.getPortfolio().getObligors();
  obligors.reserve(vobligors.size());
  for(unsigned int i=0; i<vobligors.size(); i++)
  {
    if (onlyactive)
    {
      if (vobligors[i]->isActive(time0, timeT)) 
      {
        obligors.push_back(SimulatedObligor(vobligors[i]));
      }
    }
    else
    {
      obligors.push_back(SimulatedObligor(vobligors[i]));
    }
  }
  
  // important: sorting obligors list by sector and rating
  sort(obligors.begin(), obligors.end());
  Logger::trace("number of simulated obligors", Format::toString(obligors.size()));

  // checking that exist obligors to simulate
  if (obligors.size() == 0)
  {
    throw Exception("error initializing obligors: 0 obligors to simulate");
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initAssets
//===========================================================================
void ccruncher::MonteCarlo::initAssets(IData &idata) throw(Exception)
{
  UNUSED(idata);
  
  // doing assertions
  assert(assets == NULL);

  // setting logger header
  Logger::trace("setting assets to simulate", '-');
  Logger::newIndentLevel();

  // note: this is the place where it must have the asset losses precomputation.
  // Asset losses has been moved to Obligor:insertAsset() with the purpose of 
  // being able flush memory on asset events just after precomputation because 
  // in massive portfolios memory can be exhausted

  // determining the assets to simulate
  int cont = 0;
  numassets = 0;
  for(unsigned int i=0; i<obligors.size(); i++)
  {
    vector<Asset*> &vassets = obligors[i].ref->getAssets();
    for(unsigned int j=0; j<vassets.size(); j++)
    {
      cont++;
      if (vassets[j]->isActive(time0, timeT)) 
      {
        numassets++;
      }
    }
  }
  Logger::trace("number of initial assets", Format::toString(cont));
  Logger::trace("number of simulated assets", Format::toString(numassets));

  // checking that exist assets to simulate
  if (numassets == 0)
  {
    throw Exception("error initializing assets: 0 assets to simulate");
  }

  // creating the simulated assets array
  assetsize = sizeof(SimulatedAsset) + sizeof(int)*(idata.getSegmentations().size()-1);
  assets = new char[assetsize*numassets];
  
  numassets = 0;
  for(unsigned int i=0; i<obligors.size(); i++)
  {
    vector<Asset*> &vassets = obligors[i].ref->getAssets();
    for(unsigned int j=0; j<vassets.size(); j++)
    {
      if (vassets[j]->isActive(time0, timeT)) 
      {
        SimulatedAsset *p = (SimulatedAsset *) &(assets[numassets*assetsize]);
        p->mindate = vassets[j]->getMinDate();
        p->maxdate = vassets[j]->getMaxDate();
        p->begin = vassets[j]->getData().begin();
        p->end = vassets[j]->getData().end();
        int *segments = &(p->segments);
        for(int k=0; k<idata.getSegmentations().size(); k++)
        {
          segments[k] = vassets[j]->getSegment(k);
        }
        if (obligors[i].numassets <= 0)
        {
          obligors[i].assets = p;
        }
        obligors[i].numassets++;
        numassets++;
      }
    }
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
    Date aux = add(time0, months, 'M');
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
    int months = (int) ceil(diff(time0, timeT, 'M'));
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
// calibrate copula params
//===========================================================================
void ccruncher::MonteCarlo::calibrateCopula(IData &idata) throw(Exception)
{
  if (!bcalib) return;

  timer.start();

  // setting logger header
  Logger::trace("calibrating copula", '-');
  Logger::newIndentLevel();

  int k = idata.getSectors().size();
  vector<int> n(idata.getSectors().size(),0);
  const vector<vector<hdata> > &h = idata.getDefaults().getData();
  vector<double> p(idata.getSectors().size(),0.03);

  // computing the number of obligors in each sector
  for(unsigned int i=0; i<obligors.size(); i++)
  {
    n[obligors[i].ref->isector]++;
  }

  CopulaCalibration mle;
  mle.setParams(k, &(n[0]), h, &(p[0]));
  mle.run();
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
  Logger::trace("copula dimension", Format::toString(obligors.size()));
  Logger::trace("seed used to initialize randomizer (0=none)", Format::toString(seed_));

  try
  {
    vector<int> tmp(idata.getCorrelationMatrix().size(),0);

    // computing the number of obligors in each sector
    for(unsigned int i=0; i<obligors.size(); i++)
    {
      tmp[obligors[i].ref->isector]++;
    }

    // creating the copula object
    if (idata.getParams().getCopulaType() == "gaussian")
    {
      copula = new BlockGaussianCopula(idata.getCorrelationMatrix().getMatrix(), &tmp[0], idata.getCorrelationMatrix().size());
      double cnum = ((BlockGaussianCopula*)copula)->getConditionNumber();
      Logger::trace("cholesky condition number", Format::toString(cnum));
      bool coerced = ((BlockGaussianCopula*)copula)->isCoerced();
      Logger::trace("correlations coerced", Format::toString(coerced));
    }
    else if (idata.getParams().getCopulaType() == "t")
    {
      double ndf = idata.getParams().getCopulaParam();
      copula = new BlockTStudentCopula(idata.getCorrelationMatrix().getMatrix(), &tmp[0], idata.getCorrelationMatrix().size(), ndf);
      double cnum = ((BlockTStudentCopula*)copula)->getConditionNumber();
      Logger::trace("cholesky condition number", Format::toString(cnum));
      bool coerced = ((BlockTStudentCopula*)copula)->isCoerced();
      Logger::trace("correlations coerced", Format::toString(coerced));
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
    Aggregator *aggregator = new Aggregator(assets, numassets, assetsize, i, segmentations->getSegmentation(i), filename, bforce);
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
  double etime1=0.0; // ellapsed time generating random numbers
  double etime2=0.0; // ellapsed time simulating obligors & segmentations

  // assertions
  assert(1 <= numthreads); 
  assert(fpath != "" && fpath != "path not set");
  
  // check that number of threads is lower than number of iterations
  if (maxiterations > 0 && numthreads > maxiterations) 
  {
    numthreads = maxiterations;
  }

  // setting logger header
  Logger::addBlankLine();
  Logger::trace("running Monte Carlo" + (hash==0?"": " [" + Format::toString(hash) + " simulations per hash]"), '*');
  Logger::newIndentLevel();

  // creating and launching simulation threads
  running = true;
  timer.start();
  nfthreads = numthreads;
  vector<Copula*> copulas(numthreads, (Copula*)NULL);
  numiterations = 0;
  timer3.reset();
  pthread_mutex_init(&mutex, NULL);
  threads.assign(numthreads, (SimulationThread*)NULL);
  for(int i=0; i<numthreads; i++)
  {
    if (i == 0) copulas[i] = copula;
    else copulas[i] = copula->clone(false);
    copulas[i]->setSeed(seed+i);
    threads[i] = new SimulationThread(*this, copulas[i]);
    threads[i]->start(); // to work without threads use run() instead of start()
  }

  // awaiting threads
  for(int i=0; i<numthreads; i++)
  {
    threads[i]->wait();
    etime1 += threads[i]->getEllapsedTime1();
    etime2 += threads[i]->getEllapsedTime2();
  }

  // flushing aggregators
  for(unsigned int i=0; i<aggregators.size(); i++) 
  {
    timer3.resume();
    aggregators[i]->flush();
    timer3.stop();
  }

  // destroying copulas (except main copula)
  for(int i=1; i<numthreads; i++)
  {
    delete copulas[i];
    copulas[i] = NULL;
  }

  // exit function
  Logger::trace("elapsed time creating random numbers", Timer::format(etime1/numthreads));
  Logger::trace("elapsed time simulating obligors", Timer::format(etime2/numthreads));
  Logger::trace("elapsed time writing data to disk", timer3);
  Logger::trace("total simulation time", Timer::format(timer.read()));
  Logger::addBlankLine();
  Logger::previousIndentLevel();
  running = false;
  return numiterations;
}

//===========================================================================
// append a simulation result
//===========================================================================
bool ccruncher::MonteCarlo::append(vector<vector<double> > &losses, const double *u) throw()
{
  assert(losses.size() == aggregators.size());
  pthread_mutex_lock(&mutex);
  timer3.resume();
  bool more = true;

  try
  {
    // aggregating simulation result
    for(unsigned int i=0; i<aggregators.size(); i++) 
    {
      aggregators[i]->append(losses[i]);
    }
    
    // trace values
    if (btrace)
    {
      printTrace(u);
    }

    // counter increment
    numiterations++;

    // printing hashes
    if (hash > 0 && numiterations%hash == 0)
    {
      Logger::append(".");
    }

    // checking stop criteria
    if (maxiterations > 0 && numiterations > maxiterations-nfthreads)
    {
      more = false;
      nfthreads--;
    }
    // checking stop criteria
    else if (maxseconds > 0 && timer.read() >  maxseconds)
    {
      more = false;
      nfthreads--;
    }
  }
  catch(Exception &e)
  {
    cerr << "error: " << e << endl;
    more = false;
  }

  // exit function
  timer3.stop();
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

//===========================================================================
// setTrace
//===========================================================================
void ccruncher::MonteCarlo::setTrace(bool val)
{
  btrace = val;
}

//===========================================================================
// setCalib
//===========================================================================
void ccruncher::MonteCarlo::setCalib(bool val)
{
  bcalib = val;
}

//===========================================================================
// initAdditionalOutput
//===========================================================================
void ccruncher::MonteCarlo::initTrace() throw(Exception)
{
  assert(fpath != "" && fpath != "path not set"); 
  string dirpath = File::normalizePath(fpath);
  
  if (btrace)
  {
    // simulated copula values file
    string filename = dirpath + "copula.csv";
    try
    {
      fcopulas.open(filename.c_str(), ios::out|ios::trunc);
      for(unsigned int i=0; i<obligors.size(); i++)
      {
        fcopulas << "\"" << obligors[i].ref->id << "\"" << (i!=obligors.size()-1?", ":"");
      }
      fcopulas << endl;
    }
    catch(...)
    {
      throw Exception("error writing file " + filename);
    }
  }
}

//===========================================================================
// printTrace
//===========================================================================
void ccruncher::MonteCarlo::printTrace(const double *u) throw(Exception)
{
  for(unsigned int i=0; i<obligors.size(); i++) 
  {
    fcopulas << u[i] << (i!=obligors.size()-1?", ":"");
  }
  fcopulas << "\n";
}

//===========================================================================
// indicates if is doing simulations
//===========================================================================
bool ccruncher::MonteCarlo::isRunning() const
{
  return running;
}

//===========================================================================
// abort
// return 1 if running, 0 otherwise
//===========================================================================
void ccruncher::MonteCarlo::abort()
{
  if (running)
  {
    maxiterations = 1;
  }
}

