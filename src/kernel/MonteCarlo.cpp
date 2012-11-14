
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
#include <climits>
#include <algorithm>
#include "kernel/MonteCarlo.hpp"
#include "utils/Utils.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/File.hpp"
#include <cassert>

#define MAX_NUM_THREADS 16

//===========================================================================
// constructor
//===========================================================================
ccruncher::MonteCarlo::MonteCarlo(streambuf *s) : log(s), assets(NULL), chol(NULL), stop(NULL)
{
  pthread_mutex_init(&mutex, NULL);
  maxseconds = 0;
  numiterations = 0;
  maxiterations = 0;
  antithetic = false;
  seed = 0UL;
  hash = 0;
  fpath = "path not set";
  bforce = false;
  assetsize = 0;
  numassets = 0;
  nfthreads = 0;
  time0 = NAD;
  timeT = NAD;
  ndf = NAN;
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
  numassets = 0;

  // removing threads
  for(unsigned int i=0; i<threads.size(); i++) {
    if (threads[i] != NULL) {
      threads[i]->cancel();
      delete threads[i];
      threads[i] = NULL;
    }
  }
  threads.clear();

  // dropping aggregators elements
  for(unsigned int i=0; i<aggregators.size(); i++) {
    if (aggregators[i] != NULL) {
      delete aggregators[i];
      aggregators[i] = NULL;
    }
  }
  aggregators.clear();

  // deallocating choleky matrix
  if (chol != NULL) {
    gsl_matrix_free(chol);
    chol = NULL;
  }

  // flushing memory
  vector<SimulatedObligor>(0).swap(obligors);
  vector<DateValues>(0).swap(datevalues);
  floadings.clear();
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
    log << endl;
    log << "initialization procedure" << flood('*') << endl;
    log << indent(+1);

    // initializing model
    initModel(idata);

    // initializing obligors
    initObligors(idata);

    // initializing assets
    initAssets(idata);

    // initializing aggregators
    initAggregators(idata);

    // exit function
    log << indent(-1);
  }
  catch(Exception &e)
  {
    release();
    throw Exception(e, "error initializing Monte Carlo");
  }
}

//===========================================================================
// initModel
//===========================================================================
void ccruncher::MonteCarlo::initModel(IData &idata) throw(Exception)
{
  // setting logger header
  log << "setting parameters" << flood('-') << endl;
  log << indent(+1);

  // reading properties
  maxseconds = idata.getParams().maxseconds;
  maxiterations = idata.getParams().maxiterations;
  time0 = idata.getParams().time0;
  timeT = idata.getParams().timeT;
  antithetic = idata.getParams().antithetic;
  seed = idata.getParams().rng_seed;
  if (seed == 0UL) {
    // use a seed based on clock
    seed = Utils::trand();
  }

  // setting logger info
  log << "initial date" << split << time0 << endl;
  log << "end date" << split << timeT << endl;
  log << "number of ratings" << split << idata.getRatings().size() << endl;
  log << "number of sectors" << split << idata.getSectors().size() << endl;
  log << "copula type" << split << idata.getParams().copula_type << endl;

  DefaultProbabilities dprobs;

  if (idata.hasDefaultProbabilities())
  {
    log << "default probability functions" << split << "user defined" << endl;
    dprobs = idata.getDefaultProbabilities();
  }
  else
  {
    // setting logger info
    log << "transition matrix period (months)" << split << idata.getTransitions().getPeriod() << endl;
    Transitions tone = idata.getTransitions().scale(1);
    double rerror = tone.getRegularizationError();
    log << "transition matrix regularization error (1M)" << split << rerror << endl;
    log << "default probability functions" << split << "computed" << endl;

    // computing default probability functions using transition matrix
    int months = (int) ceil(diff(time0, timeT, 'M'));
    dprobs = tone.getDefaultProbabilities(time0, months+1);
  }

  // setting inverses
  ndf = -1.0; // gaussian case
  if (idata.getParams().getCopulaType() == "t") {
    ndf = idata.getParams().getCopulaParam();
  }

  string strsplines;
  for(int i=0; i<dprobs.size(); i++) {
    strsplines += dprobs.getInterpolationType(i)[0];
  }
  log << "default probability splines (linear, cubic, none)" << split << strsplines << endl;

  // model parameters
  inverses.init(ndf, timeT, dprobs);
  chol = idata.getCorrelations().getCholesky();
  floadings = idata.getCorrelations().getFactorLoadings();

  assert((int)chol->size1 == idata.getSectors().size());
  assert(chol->size1 == chol->size2);
  assert((int)floadings.size() == idata.getSectors().size());

  // performance tip: chol contains the w·chol (to avoid multiplications)
  // in simulation time and w contaings sqrt(1-w^2) (to avoid sqrt
  // and multiplications)
  for(unsigned int i=0; i<chol->size1; i++)
  {
    for(unsigned int j=0; j<chol->size2; j++)
    {
      double val = gsl_matrix_get(chol, i, j) * floadings[i];
      gsl_matrix_set(chol, i, j, val);
    }
    floadings[i] = sqrt(1.0-floadings[i]*floadings[i]);
  }

  // exit function
  log << indent(-1);
}

//===========================================================================
// initObligors
//===========================================================================
void ccruncher::MonteCarlo::initObligors(IData &idata) throw(Exception)
{
  // doing assertions
  assert(obligors.empty());

  // setting logger header
  log << "setting obligors to simulate" << flood('-') << endl;
  log << indent(+1);

  // checking limits (see SimulatedObligor fields)
  if (idata.getRatings().size() == 0 || UCHAR_MAX < idata.getRatings().size()) {
    throw Exception("invalid number of ratings");
  }
  if (idata.getSectors().size() == 0 || UCHAR_MAX < idata.getSectors().size()) {
    throw Exception("invalid number of sectors");
  }

  // setting logger info
  log << "simulate only active obligors" << split << idata.getParams().onlyactive << endl;
  log << "number of obligors" << split << idata.getPortfolio().getObligors().size() << endl;

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
  log << "number of simulated obligors" << split << obligors.size() << endl;

  // checking that exist obligors to simulate
  if (obligors.empty())
  {
    throw Exception("error initializing obligors: 0 obligors to simulate");
  }

  // exit function
  log << indent(-1);
}

//===========================================================================
// initAssets
//===========================================================================
void ccruncher::MonteCarlo::initAssets(IData &idata) throw(Exception)
{
  UNUSED(idata);
  
  // doing assertions
  assert(assets == NULL);

  // checking limits (see SimulatedAsset::segments field)
  if (idata.getSegmentations().size() == 0 || USHRT_MAX < idata.getSegmentations().size()) {
    throw Exception("invalid number of segmentations");
  }

  // setting logger header
  log << "setting assets to simulate" << flood('-') << endl;
  log << indent(+1);

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
  log << "number of assets" << split << cont << endl;
  log << "number of simulated assets" << split << numassets << endl;

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
    obligors[i].numassets = 0;
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
        unsigned short *segments = &(p->segments);
        for(int k=0; k<idata.getSegmentations().size(); k++)
        {
          segments[k] = static_cast<unsigned short>(vassets[j]->getSegment(k));
        }

        // assigning asset to obligor
        if (obligors[i].ref.assets == NULL)
        {
          obligors[i].ref.assets = p;
        }

        // incrementing num assets counters
        if (obligors[i].numassets == USHRT_MAX)
        {
          throw Exception("exceeded maximum number of assets by obligor");
        }
        obligors[i].numassets++;
        numassets++;
      }
    }
  }

  // exit function
  log << indent(-1);
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
  log << "initializing aggregators" << flood('-') << endl;
  log << indent(+1);

  // setting logger info
  log << "output data directory" << split << fpath << endl;
  log << "number of segmentations" << split << idata.getSegmentations().size() << endl;

  // allocating and initializing aggregators
  aggregators.clear();
  for(int i=0; i<idata.getSegmentations().size(); i++)
  {
    string filename = File::normalizePath(fpath) + idata.getSegmentations().getSegmentation(i).name + ".csv";
    Aggregator *aggregator = new Aggregator(assets, numassets, assetsize, i, idata.getSegmentations().getSegmentation(i), filename, bforce);
    aggregators.push_back(aggregator);
  }

  // exit function
  log << indent(-1);
}

//===========================================================================
// execute
//===========================================================================
void ccruncher::MonteCarlo::run(unsigned char numthreads, size_t nhash, bool *stop_)
{
  stop = stop_;
  if (stop != NULL && *stop) return;

  double etime1=0.0; // ellapsed time generating random numbers
  double etime2=0.0; // ellapsed time simulating obligors & segmentations

  // assertions
  assert(fpath != "" && fpath != "path not set");

  // check that number of threads is lower than number of iterations
  if (maxiterations > 0 && numthreads > maxiterations) 
  {
    numthreads = maxiterations;
  }

  // setting hash
  hash = nhash;

  // setting logger header
  log << endl;
  log << "Monte Carlo" << flood('*') << endl;
  log << indent(+1);
  log << "configuration" << flood('-') << endl;
  log << indent(+1);
  log << "seed used to initialize randomizer" << split << seed << endl;
  log << "maximum execution time (seconds)" << split << maxseconds << endl;
  log << "maximum number of iterations" << split << maxiterations << endl;
  log << "antithetic mode" << split << antithetic << endl;
  log << "number of threads" << split << (int)numthreads << endl;
  log << indent(-1);
  log << "running Monte Carlo";
  if (hash != 0) log << " [" << Format::toString(hash) << " simulations per hash]";
  log << flood('-') << endl;
  log << indent(+1);

  // creating and launching simulation threads
  timer.start();
  nfthreads = numthreads;
  numiterations = 0;
  timer3.reset();
  threads.assign(numthreads, (SimulationThread*)NULL);
  for(int i=0; i<numthreads; i++)
  {
    threads[i] = new SimulationThread(*this, seed+i);
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

  // exit function
  log << "simulations realized" << split <<numiterations << endl;
  log << "elapsed time creating random numbers" << split << Timer::format(etime1/numthreads) << endl;
  log << "elapsed time simulating obligors" << split << Timer::format(etime2/numthreads) << endl;
  log << "elapsed time writing data to disk" << split << timer3 << endl;
  log << "total simulation time" << split << timer << endl;
  log << indent(-2) << endl;
}

//===========================================================================
// append a simulation result
//===========================================================================
bool ccruncher::MonteCarlo::append(vector<vector<double> > &losses) throw()
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

    // counter increment
    numiterations++;

    // printing hashes
    if (hash > 0 && numiterations%hash == 0)
    {
      log << '.';
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
    cerr << "error: " << e << std::endl;
    more = false;
  }

  // exit function
  if (!more) nfthreads--;
  timer3.stop();
  pthread_mutex_unlock(&mutex);
  return(more);
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

