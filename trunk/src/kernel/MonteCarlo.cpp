
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include "kernel/MonteCarlo.hpp"
#include "portfolio/DateValues.hpp"
#include "utils/Utils.hpp"
#include "utils/Format.hpp"
#include "utils/File.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

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
  lhs_size = 1;
  blocksize = 1;
  seed = 0UL;
  hash = 0;
  fpath = "path not set";
  fmode = 'c';
  nfthreads = 0;
  time0 = NAD;
  timeT = NAD;
  ndf = NAN;
  assetsize = 0;
  numassets = 0;
  numsegments = 0;
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
  // removing threads
  for(unsigned int i=0; i<threads.size(); i++) {
    if (threads[i] != NULL) {
      threads[i]->cancel();
      delete threads[i];
      threads[i] = NULL;
    }
  }
  threads.clear();

  // deallocating assets
  if (assets != NULL)
  {
    for (size_t i=0; i<numassets; i++) {
      SimulatedAsset *p = reinterpret_cast<SimulatedAsset*>(assets+i*assetsize);
      delete [] p->begin;
      p->begin = NULL;
    }
    delete [] assets;
    assets = NULL;
  }
  numassets = 0;

  // dropping aggregators elements
  for(unsigned int i=0; i<aggregators.size(); i++) {
    if (aggregators[i] != NULL) {
      delete aggregators[i];
      aggregators[i] = NULL;
    }
  }
  aggregators.clear();

  // deallocating cholesky matrix
  if (chol != NULL) {
    gsl_matrix_free(chol);
    chol = NULL;
  }

  // flushing memory
  vector<SimulatedObligor>(0).swap(obligors);
  floadings1.clear();
  floadings2.clear();
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
  catch(std::exception &e)
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
  lhs_size = idata.getParams().lhs_size;
  blocksize = idata.getParams().blocksize;
  seed = idata.getParams().rng_seed;
  if (seed == 0UL) {
    // use a seed based on clock
    seed = Utils::trand();
  }

  // checking limits (see SimulatedAsset::segments field)
  if (idata.getSegmentations().size() == 0 || USHRT_MAX < idata.getSegmentations().size()) {
    throw Exception("invalid number of segmentations");
  }

  // setting logger info
  log << "initial date" << split << time0 << endl;
  log << "end date" << split << timeT << endl;
  log << "number of ratings" << split << idata.getRatings().size() << endl;
  log << "number of factors" << split << idata.getFactors().size() << endl;
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

  // setting degrees of freedom
  ndf = -1.0; // gaussian case
  if (idata.getParams().getCopulaType() == "t") {
    ndf = idata.getParams().getCopulaParam();
  }

  // setting default probs info
  string strsplines;
  for(int i=0; i<dprobs.size(); i++) {
    strsplines += dprobs.getInterpolationType(i)[0];
  }
  log << "default probability splines (linear, cubic, none)" << split << strsplines << endl;

  // model parameters
  inverses.init(ndf, timeT, dprobs);
  chol = idata.getCorrelations().getCholesky();
  floadings1 = idata.getCorrelations().getFactorLoadings();

  // performance tip: contaings sqrt(1-w^2) (to avoid sqrt and multiplication)
  floadings2 = floadings1;
  for(size_t i=0; i<floadings2.size(); i++) {
    floadings2[i] = sqrt(1.0 - floadings1[i]*floadings1[i]);
  }

  // performance tip: chol contains w·chol (to avoid multiplications)
  // if LHS>1 this is not possible and mults are done in SimulationThread
  if (lhs_size == 1) {
    for(size_t i=0; i<chol->size1; i++) {
      for(size_t j=0; j<chol->size2; j++) {
        double val = gsl_matrix_get(chol, i, j)*floadings1[i];
        gsl_matrix_set(chol, i, j, val);
      }
    }
  }

  assert((int)chol->size1 == idata.getFactors().size());
  assert(chol->size1 == chol->size2);
  assert((int)floadings1.size() == idata.getFactors().size());

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
  if (idata.getFactors().size() == 0 || UCHAR_MAX < idata.getFactors().size()) {
    throw Exception("invalid number of factors");
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
  
  // important: sorting obligors list by factor and rating
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
  // setting logger header
  log << "setting assets to simulate" << flood('-') << endl;
  log << indent(+1);

  // note: this is the place where it must have the asset losses precomputation.
  // Asset losses has been moved to Obligor:insertAsset() with the purpose of 
  // being able flush memory on asset events just after precomputation because 
  // in massive portfolios memory can be exhausted

  // determining the assets to simulate
  numassets = 0;
  size_t cont = 0;
  for(size_t i=0; i<obligors.size(); i++)
  {
    vector<Asset*> &vassets = obligors[i].ref.obligor->getAssets();
    for(size_t j=0; j<vassets.size(); j++)
    {
      cont++;
      if (vassets[j]->isActive(time0, timeT)) 
      {
        numassets++;
        idata.getSegmentations().addComponents(vassets[j]);
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

  // remove unused segments
  idata.getSegmentations().removeUnusedSegments();

  // creating the simulated assets array
  assert(assets == NULL);
  assetsize = sizeof(SimulatedAsset) + sizeof(unsigned short)*(idata.getSegmentations().size()-1);
  assets = new char[assetsize*numassets];
  memset(assets, 0, assetsize*numassets); // set pointers to NULL

  numassets = 0;
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
        // checking ranges
        if (obligors[i].numassets == USHRT_MAX)
        {
          throw Exception("exceeded maximum number of assets by obligor");
        }

        // recode segments
        idata.getSegmentations().recodeSegments(vassets[j]);

        // setting asset
        SimulatedAsset *p = reinterpret_cast<SimulatedAsset *>(assets+numassets*assetsize);

        // creating asset
        p->mindate = vassets[j]->getMinDate();
        p->maxdate = vassets[j]->getMaxDate();
        size_t len = vassets[j]->getData().size();
        if (len > 0) {
          p->begin = new DateValues[len];
          p->end = p->begin + len;
          memcpy(p->begin, &(vassets[j]->getData()[0]), len*sizeof(DateValues));
          vassets[j]->clearData(); // too avoid memory exhaustion
        }

        // setting asset segments
        unsigned short *segments = &(p->segments);
        for(int k=0; k<idata.getSegmentations().size(); k++)
        {
          segments[k] = static_cast<unsigned short>(vassets[j]->getSegment(k));
        }

        // assigning obligor.assets to first asset
        if (obligors[i].ref.assets == NULL)
        {
          obligors[i].ref.assets = p;
        }

        // incrementing num assets counters
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
  log << "output data directory" << split << "["+fpath+"]" << endl;

  // allocating and initializing aggregators
  numsegments = 0;
  int numsegmentations = idata.getSegmentations().size();
  numSegmentsBySegmentation.resize(numsegmentations, 0);
  aggregators.resize(numsegmentations, static_cast<Aggregator*>(NULL));
  for(int i=0; i<numsegmentations; i++)
  {
    if (idata.getSegmentations().getSegmentation(i).size() > USHRT_MAX) {
      const string &sname = idata.getSegmentations().getSegmentation(i).name;
      throw Exception("segmentation '" + sname + "' exceeds the maximum number of segments");
    }

    string filename = idata.getSegmentations().getSegmentation(i).getFilename(fpath);
    Aggregator *aggregator = new Aggregator(i, idata.getSegmentations(), filename, fmode);
    aggregators[i] = aggregator;
    numSegmentsBySegmentation[i] = (unsigned short)(idata.getSegmentations().getSegmentation(i).size());
    numsegments += numSegmentsBySegmentation[i];
    log << "segmentation" << split << "["+filename+"]" << endl;
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
  log << "latin hypercube sampling" << split << (lhs_size==1?"false":Format::toString(size_t(lhs_size))) << endl;
  log << "thread block size" << split << blocksize << endl;
  log << "number of threads" << split << int(numthreads) << endl;
  log << indent(-1);
  log << "running Monte Carlo";
  if (hash != 0) log << " [" << Format::toString(hash) << " simulations per hash]";
  log << flood('-') << endl;
  log << indent(+1);

  // creating and launching simulation threads
  timer.start();
  nfthreads = numthreads;
  numiterations = 0;
  threads.assign(numthreads, static_cast<SimulationThread*>(NULL));
  for(int i=0; i<numthreads; i++)
  {
    threads[i] = new SimulationThread(i+1, *this, seed+i, blocksize);
    if (numthreads == 1) {
      threads[i]->run();
    }
    else {
      threads[i]->start();
    }
  }

  // awaiting threads
  double etime1 = 0.0; // elapsed time generating random numbers
  double etime2 = 0.0; // elapsed time simulating losses
  double etime3 = 0.0; // elapsed time writting to disk

  for(int i=0; i<numthreads; i++)
  {
    threads[i]->wait();
    etime1 += threads[i]->getElapsedTime1();
    etime2 += threads[i]->getElapsedTime2();
    etime3 += threads[i]->getElapsedTime3();
    delete threads[i];
    threads[i] = NULL;
  }

  // closing aggregators
  Timer timer3_aux(true);
  if (findexes.is_open())
  {
    findexes.close();
  }
  for(unsigned int i=0; i<aggregators.size(); i++)
  {
    delete aggregators[i];
    aggregators[i] = NULL;
  }
  etime3 += timer3_aux.stop();

  // exit function
  if (nhash > 0) log << endl;
  log << "simulations realized" << split <<numiterations << endl;
  log << "elapsed time creating random numbers" << split << Timer::format(etime1/numthreads) << endl;
  log << "elapsed time simulating obligors" << split << Timer::format(etime2/numthreads) << endl;
  log << "elapsed time writing data to disk" << split << Timer::format(etime3/numthreads) << endl;
  log << "total simulation time" << split << timer << endl;
  log << indent(-2) << endl;
}

//===========================================================================
// append a simulation result
//===========================================================================
bool ccruncher::MonteCarlo::append(int ithread, const vector<short> &vi, const double *losses) throw()
{
  assert(losses != NULL);
  assert(vi.size() == blocksize);

  pthread_mutex_lock(&mutex);
  bool more = true;

  try
  {
    for(size_t iblock=0; iblock<vi.size(); iblock++)
    {
      // trace indexes (if required)
      if (findexes.is_open()) {
        findexes << ithread << ", " << std::abs(int(vi[iblock])) << ", " << (vi[iblock]<0?1:0) << "\n";
      }

      // aggregating simulation result
      for(unsigned int i=0; i<aggregators.size(); i++)
      {
        aggregators[i]->append(losses);
        losses += numSegmentsBySegmentation[i];
      }

      // counter increment
      numiterations++;

      // printing hashes
      if (hash > 0 && numiterations%hash == 0)
      {
        log << '.' << flush;
      }

      // checking stop criterias
      assert(nfthreads > 0);
      if (
           (maxiterations > 0 && numiterations + (nfthreads-1)*blocksize >= maxiterations) ||
           (stop != NULL && *stop)
         )
      {
        more = false;
        break;
      }
    }
  }
  catch(Exception &e)
  {
    cerr << "error: " << e << std::endl;
    more = false;
  }

  // checking time stop criterion
  if (maxseconds > 0 && timer.read() >  maxseconds) {
    more = false;
  }

  // exit function
  if (!more) nfthreads--;
  pthread_mutex_unlock(&mutex);
  return(more);
}

//===========================================================================
// setFilePath
//===========================================================================
void ccruncher::MonteCarlo::setFilePath(const string &path, char mode, bool indexes) throw(std::exception)
{
  fpath = path;
  fmode = mode;

  if (indexes)
  {
    string filename = File::normalizePath(fpath) + "indexes.csv";
    findexes.exceptions(ios::failbit | ios::badbit);
    findexes.open(filename.c_str(), ios::out|(fmode=='a'?(ios::app):(ios::trunc)));
    if (fmode != 'a' || (fmode == 'a' && File::filesize(filename) == 0)) {
      findexes << "\"thread\", \"lhs\", \"antithetic\"" << std::endl;
    }
  }
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

