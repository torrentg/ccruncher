
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
#include "math/GMFCopula.hpp"
#include "math/TMFCopula.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/File.hpp"
#include <cassert>

#define MAX_NUM_THREADS 16

//===========================================================================
// constructor
//===========================================================================
ccruncher::MonteCarlo::MonteCarlo() : obligors(), assets(NULL), aggregators(), threads(0)
{
  pthread_mutex_init(&mutex, NULL);
  maxseconds = 0;
  numiterations = 0;
  maxiterations = 0;
  antithetic = false;
  seed = 0L;
  hash = 0;
  fpath = "path not set";
  bforce = false;
  btrace = false;
  copula = NULL;
  stop = NULL;
  assetsize = 0;
  numassets = 0;
  numthreads = 1;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::MonteCarlo::~MonteCarlo()
{
  release();
  pthread_mutex_destroy(&mutex);
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

  // deallocating copula
  if (copula != NULL) { 
    delete copula; 
    copula = NULL; 
  }

  // dropping aggregators elements
  for(unsigned int i=0; i<aggregators.size(); i++) {
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
void ccruncher::MonteCarlo::setData(IData &idata) throw(Exception)
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

    // initialize trace
    initTrace();

    // initializing copula
    initCopula(idata);

    // initializing assets
    initAssets(idata);

    // initializing survival functions
    initSurvivals(idata);

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
  Logger::trace("maximum execution time (seconds)", Format::toString(maxseconds));

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

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initObligors
//===========================================================================
void ccruncher::MonteCarlo::initObligors(IData &idata) throw(Exception)
{
  // doing assertions
  assert(obligors.empty());

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
  if (obligors.empty())
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
  int numdatevalues = 0;
  for(unsigned int i=0; i<obligors.size(); i++)
  {
    vector<Asset*> &vassets = obligors[i].ref.obligor->getAssets();
    for(unsigned int j=0; j<vassets.size(); j++)
    {
      cont++;
      if (vassets[j]->isActive(time0, timeT)) 
      {
        numassets++;
        numdatevalues += vassets[j]->getData().size();
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
  datevalues.resize(numdatevalues);
  assetsize = sizeof(SimulatedAsset) + sizeof(int)*(idata.getSegmentations().size()-1);
  assets = new char[assetsize*numassets];
  
  numassets = 0;
  numdatevalues = 0;
  for(unsigned int i=0; i<obligors.size(); i++)
  {
    Obligor *obligor = obligors[i].ref.obligor;
    obligors[i].ref.assets = NULL;
    vector<Asset*> &vassets = obligor->getAssets();
    for(unsigned int j=0; j<vassets.size(); j++)
    {
      if (vassets[j]->isActive(time0, timeT)) 
      {
        // creating asset
        SimulatedAsset *p = (SimulatedAsset *)(assets+numassets*assetsize);
        p->mindate = vassets[j]->getMinDate();
        p->maxdate = vassets[j]->getMaxDate();
        p->begin = datevalues.begin() + numdatevalues;

        // setting asset datevalues
        for(size_t k=0; k<vassets[j]->getData().size(); k++)
        {
          datevalues[numdatevalues] = vassets[j]->getData()[k];
          numdatevalues++;
        }
        p->end = datevalues.begin() + numdatevalues;

        // setting asset segments
        int *segments = &(p->segments);
        for(int k=0; k<idata.getSegmentations().size(); k++)
        {
          segments[k] = vassets[j]->getSegment(k);
        }

        // assigning asset to obligor
        if (obligors[i].ref.assets == NULL)
        {
          obligors[i].ref.assets = p;
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
// initSurvivals
//===========================================================================
void ccruncher::MonteCarlo::initSurvivals(IData &idata) throw(Exception)
{
  // doing assertions
  assert(survivals.size() == 0);

  // setting logger header
  Logger::trace("setting survival function", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of ratings", Format::toString(idata.getRatings().size()));

  if (idata.hasSurvivals())
  {
    // setting survival functions
    survivals = idata.getSurvivals();
    Logger::trace("survival functions", string("user defined"));

    // checking that survival function is defined for t <= timeT
    int months = idata.getSurvivals().getMinCommonTime();
    Date aux = add(time0, months, 'M');
    if (aux < timeT)
    {
      throw Exception("survival functions not defined at t=" + Format::toString(timeT));
    }
  }
  else
  {
    // setting logger info
    string sval = Format::toString(idata.getTransitions().size());
    Logger::trace("transition matrix dimension", sval + "x" + sval);
    Logger::trace("transition matrix period (months)", Format::toString(idata.getTransitions().getPeriod()));

    // computing survival functions using transition matrix
    int months = (int) ceil(diff(time0, timeT, 'M'));
    survivals = idata.getTransitions().getSurvivals(1, months+1);
    Logger::trace("transition matrix -> survival functions", string("computed"));
    Transitions tm1 = idata.getTransitions().scale(1);
    double rerror = tm1.getRegularizationError();
    Logger::trace("regularization error", Format::toString(rerror));
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// copula construction
//===========================================================================
void ccruncher::MonteCarlo::initCopula(IData &idata) throw(Exception)
{
  seed = idata.getParams().copula_seed;

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
  Logger::trace("seed used to initialize randomizer (0=none)", Format::toString(seed));

  try
  {
    // computing the number of obligors in each sector
    vector<unsigned int> noblig(idata.getCorrelations().size(),0);
    for(unsigned int i=0; i<obligors.size(); i++)
    {
      noblig[obligors[i].ref.obligor->isector]++;
    }

    // creating the copula object
    const vector<vector<double> > &C = idata.getCorrelations().getMatrix();

    if (idata.getParams().getCopulaType() == "gaussian")
    {
      copula = new GMFCopula(C, noblig);
    }
    else if (idata.getParams().getCopulaType() == "t")
    {
      double ndf = idata.getParams().getCopulaParam();
      copula = new TMFCopula(C, noblig, ndf);
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
  if (seed == 0L)
  {
    // use a seed based on clock
    seed = Utils::trand();
  }

  // seeding random number generators
  // see SimulationThread constructor
  copula->setSeed(seed);

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initAggregators
//===========================================================================
void ccruncher::MonteCarlo::initAggregators(IData &idata) throw(Exception)
{
  // assertions
  assert(fpath != "" && fpath != "path not set"); 
  assert(aggregators.empty()); 

  // setting logger header
  Logger::trace("initializing aggregators", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("output data directory", fpath);
  Logger::trace("number of segmentations", Format::toString(idata.getSegmentations().size()));

  // allocating and initializing aggregators
  aggregators.clear();
  for(int i=0; i<idata.getSegmentations().size(); i++)
  {
    string filename = File::normalizePath(fpath) + idata.getSegmentations().getSegmentation(i).name + ".csv";
    Aggregator *aggregator = new Aggregator(assets, numassets, assetsize, i, idata.getSegmentations().getSegmentation(i), filename, bforce);
    aggregators.push_back(aggregator);
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// execute
//===========================================================================
void ccruncher::MonteCarlo::run(bool *stop_)
{
  stop = stop_;
  if (stop != NULL && *stop) return;

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
  timer.start();
  nfthreads = numthreads;
  vector<Copula*> copulas(numthreads, (Copula*)NULL);
  numiterations = 0;
  timer3.reset();
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
    delete threads[i];
    threads[i] = NULL;
  }

  // closing aggregators
  timer3.resume();
  for(unsigned int i=0; i<aggregators.size(); i++)
  {
    delete aggregators[i];
    aggregators[i] = NULL;
  }
  timer3.stop();

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

    // checking stop criterias
    if (
         (maxiterations > 0 && numiterations > maxiterations-nfthreads) ||
         (maxseconds > 0 && timer.read() >  maxseconds) ||
         (stop != NULL && *stop)
       )
    {
      more = false;
    }
  }
  catch(Exception &e)
  {
    cerr << "error: " << e << endl;
    more = false;
  }

  // exit function
  if (!more) nfthreads--;
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
void ccruncher::MonteCarlo::setFilePath(const string &path, bool force)
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
// set the number of execution threads
//===========================================================================
void ccruncher::MonteCarlo::setNumThreads(int v)
{
  assert(0 < v && v <= MAX_NUM_THREADS);
  if (0 < v && v <= MAX_NUM_THREADS) {
    numthreads = v;
  }
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
        fcopulas << "\"" << obligors[i].ref.obligor->id << "\"" << (i!=obligors.size()-1?", ":"");
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
// returns iterations done
//===========================================================================
int ccruncher::MonteCarlo::getNumIterations() const
{
  return numiterations;
}

//===========================================================================
// returns maximum number of iterations
//===========================================================================
int ccruncher::MonteCarlo::getMaxIterations() const
{
  return maxiterations;
}

