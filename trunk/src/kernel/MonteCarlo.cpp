
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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
#include <cassert>
#include "kernel/MonteCarlo.hpp"
#include "portfolio/DateValues.hpp"
#include "utils/Utils.hpp"
#include "utils/Format.hpp"
#include "utils/File.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] s Streambuf where the trace will be written.
 */
ccruncher::MonteCarlo::MonteCarlo(std::streambuf *s) : log(s), chol(nullptr), stop(nullptr)
{
  pthread_mutex_init(&mutex, nullptr);
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
  numsegments = 0;
}

/**************************************************************************/
ccruncher::MonteCarlo::~MonteCarlo()
{
  release();
  pthread_mutex_destroy(&mutex);
}

/**************************************************************************/
void ccruncher::MonteCarlo::release()
{
  // removing threads
  for(unsigned int i=0; i<threads.size(); i++) {
    if (threads[i] != nullptr) {
      threads[i]->cancel();
      delete threads[i];
      threads[i] = nullptr;
    }
  }
  threads.clear();

  // dropping aggregators elements
  for(unsigned int i=0; i<aggregators.size(); i++) {
    if (aggregators[i] != nullptr) {
      delete aggregators[i];
      aggregators[i] = nullptr;
    }
  }
  aggregators.clear();

  // deallocating cholesky matrix
  if (chol != nullptr) {
    gsl_matrix_free(chol);
    chol = nullptr;
  }

  // flushing memory
  vector<SimulatedObligor>(0).swap(obligors);
  vector<SimulatedAsset>(0).swap(assets);
  vector<unsigned short>(0).swap(segments);
  floadings1.clear();
  floadings2.clear();
}

/**************************************************************************//**
 * @param[in] idata CCruncher input data.
 * @throw Exception Error initializing object.
 */
void ccruncher::MonteCarlo::setData(IData &idata)
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

/**************************************************************************//**
 * @details This method constructs the inverse functions. If default
 *          probabilities are not user-defined, it inferes these functions
 *          from the transition matrix.
 * @param[in] idata CCruncher input data.
 * @throw Exception Error initializing object.
 */
void ccruncher::MonteCarlo::initModel(IData &idata)
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
  floadings1 = idata.getFactors().getLoadings();

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

/**************************************************************************//**
 * @details Creates the list of simulated obligors. In this initialization
 *          stage, the SimulatedObligor::ref contains a reference to
 *          Obligor.
 * @param[in] idata CCruncher input data.
 * @throw Exception Error initializing object.
 */
void ccruncher::MonteCarlo::initObligors(IData &idata)
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

  // determining the obligors to simulate
  vector<Obligor *> &vobligors = idata.getPortfolio().getObligors();
  obligors.reserve(vobligors.size());
  for(size_t i=0; i<vobligors.size(); i++)
  {
    if (vobligors[i]->isActive(time0, timeT))
    {
      obligors.push_back(SimulatedObligor(vobligors[i]));
      // trick to transfer obligor ref to next stage (initAssets)
      obligors.back().setObligor(vobligors[i]);
    }
  }

  // sorting obligors list by factor and rating
  sort(obligors.begin(), obligors.end());

  // setting logger info
  log << "number of obligors" << split << idata.getPortfolio().getObligors().size() << endl;
  log << "number of simulated obligors" << split << obligors.size() << endl;

  // checking that exist obligors to simulate
  if (obligors.empty())
  {
    throw Exception("error initializing obligors: 0 obligors to simulate");
  }

  // exit function
  log << indent(-1);
}

/**************************************************************************//**
 * @details Creates the list of simulated assets. This initialization stage
 *          sets SimulatedObligor::ref to the simulated asset.
 * @param[in] idata CCruncher input data.
 * @throw Exception Error initializing object.
 */
void ccruncher::MonteCarlo::initAssets(IData &idata)
{
  // setting logger header
  log << "setting assets to simulate" << flood('-') << endl;
  log << indent(+1);

  // note: this is the place where it must have the asset losses precomputation.
  // Asset losses has been moved to Obligor:insertAsset() with the purpose of 
  // being able flush memory on asset events just after precomputation because 
  // in massive portfolios memory can be exhausted

  // determining the assets to simulate
  size_t numassets = 0;
  size_t numdatevalues = 0;
  size_t cont = 0;
  for(size_t i=0; i<obligors.size(); i++)
  {
    Obligor *obligor = obligors[i].getObligor();
    vector<Asset*> &vassets = obligor->getAssets();
    for(size_t j=0; j<vassets.size(); j++)
    {
      cont++;
      if (vassets[j]->isActive(time0, timeT))
      {
        numassets++;
        numdatevalues += vassets[j]->getData().size();
        idata.getSegmentations().addComponents(vassets[j]);
      }
    }
  }
  log << "number of assets" << split << cont << endl;
  log << "number of simulated assets" << split << numassets << endl;
  log << "number of simulated data items" << split << numdatevalues << endl;

  // checking that exist assets to simulate
  if (numassets == 0)
  {
    throw Exception("error initializing assets: 0 assets to simulate");
  }

  // remove unused segments
  idata.getSegmentations().removeUnusedSegments();

  // creating the simulated assets array
  assert(assets.empty());
  assets.resize(numassets); // initialize values (pointers)
  assert(segments.empty());
  size_t numsegmentations = idata.getSegmentations().size();
  assert(numsegmentations >= 1);
  segments.reserve(numassets*numsegmentations);

  numassets = 0;
  for(size_t i=0; i<obligors.size(); i++)
  {
    Obligor *obligor = obligors[i].getObligor();
    obligors[i].lgd = obligor->lgd;
    obligors[i].numassets = 0;
    vector<Asset*> &vassets = obligor->getAssets();
    for(size_t j=0; j<vassets.size(); j++)
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

        // setting asset segments
        for(size_t k=0; k<numsegmentations; k++)
        {
          segments.push_back(static_cast<unsigned short>(vassets[j]->getSegment(k)));
        }

        // filling simulated asset
        assets[numassets].assign(vassets[j]);

        // incrementing num assets counters
        obligors[i].numassets++;
        numassets++;
      }
      vassets[j]->clearData();
    }
  }

  assert(assets.size() == numassets);
  assert(segments.size() == numassets*numsegmentations);

  // exit function
  log << indent(-1);
}

/**************************************************************************//**
 * @details Initializes output files.
 * @param[in] idata CCruncher input data.
 * @throw Exception Error initializing object.
 */
void ccruncher::MonteCarlo::initAggregators(IData &idata)
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
  size_t numsegmentations = idata.getSegmentations().size();
  numSegmentsBySegmentation.resize(numsegmentations, 0);
  aggregators.resize(numsegmentations, static_cast<Aggregator*>(nullptr));
  for(int i=0; i<(int)numsegmentations; i++)
  {
    if (idata.getSegmentations().getSegmentation(i).size() > USHRT_MAX) {
      const string &sname = idata.getSegmentations().getSegmentation(i).name;
      throw Exception("segmentation '" + sname + "' exceeds the maximum number of segments");
    }

    string ifile = idata.getFilename();
    string ofile = idata.getSegmentations().getSegmentation(i).getFilename(fpath);
    vector<double> exposures = getExposures(i, idata);
    Aggregator *aggregator = new Aggregator(i, idata.getSegmentations(), ofile, fmode, ifile, exposures);
    aggregators[i] = aggregator;
    numSegmentsBySegmentation[i] = (unsigned short)(idata.getSegmentations().getSegmentation(i).size());
    numsegments += numSegmentsBySegmentation[i];
    log << "segmentation" << split << "["+ofile+"]" << endl;
  }

  // exit function
  log << indent(-1);
}

/**************************************************************************//**
 * @details For each DateValues computes its expected exposure (weighted
 *          by its duration in the period T0-T1) and adds this values to
 *          the corresponding segment.
 * @param[in] isegmentation Segmentation index.
 * @param[in] idata CCruncher input data.
 * @return Segments' exposures.
 */
vector<double> ccruncher::MonteCarlo::getExposures(int isegmentation, IData &idata) const
{
  double numdays = timeT - time0;
  size_t numsegmentations = idata.getSegmentations().size();
  int len = idata.getSegmentations().getSegmentation(isegmentation).size();
  vector<double> ret(len, 0.0);

  for(size_t iobligor=0, iasset=0, isegment=isegmentation; iobligor<obligors.size(); iobligor++)
  {
    for(size_t i=0; i<obligors[iobligor].numassets; i++)
    {
      unsigned short segment = segments[isegment];
      Date prevt = time0;
      for(DateValues *dv=assets[iasset].begin; dv < assets[iasset].end; ++dv)
      {
        double weight = (min(dv->date,timeT) - prevt)/numdays;
        ret[segment] += weight * dv->ead.getExpected();
        prevt = dv->date;
      }
      iasset++;
      isegment += numsegmentations;
    }
  }

  return ret;
}

/**************************************************************************//**
 * @details Starts the simulation procedure. If there is only 1 thread,
 *          then uses the current thread (simplifies debug), otherwise
 *          creates one simulation per thread.
 * @param[in] numthreads Number of threads to use.
 * @param[in] nhash Number of simulation per hash (0 = no hash trace).
 * @param[in] stop_ Variable to stop simulation from outside.
 */
void ccruncher::MonteCarlo::run(unsigned char numthreads, size_t nhash, bool *stop_)
{
  stop = stop_;
  if (stop != nullptr && *stop) return;

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
  log << "block size" << split << blocksize << endl;
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
  threads.assign(numthreads, static_cast<SimulationThread*>(nullptr));
  for(int i=0; i<numthreads; i++)
  {
    threads[i] = new SimulationThread(i+1, *this, seed+i);
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
    threads[i] = nullptr;
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
    aggregators[i] = nullptr;
  }
  etime3 += timer3_aux.stop();

  // exit function
  if (nhash > 0) log << endl;
  log << "simulations realized" << split <<numiterations << endl;
  log << "elapsed time creating random numbers" << split << Timer::format(etime1/numthreads) << endl;
  log << "elapsed time simulating losses" << split << Timer::format(etime2/numthreads) << endl;
  log << "elapsed time writing data to disk" << split << Timer::format(etime3/numthreads) << endl;
  log << "total simulation time" << split << timer << endl;
  log << indent(-2) << endl;
}

/**************************************************************************//**
 * @param[in] ithread Thread identifier.
 * @param[in] vi Vector of indexes (info related to lhs & antithetic usage).
 * @param[in] losses Simulated data. This is an array with the following
 *            structure: X1, X2, X3, ..., Xk where Xi is an array containing
 *            the data of the i-th simulation (k = blocksize). Xi has the
 *            following structure: S1, S2, ..., Sm where Si are the segments
 *            losses of the i-th segmentation (m=number of segmentations).
 *            Finally, Si has the following structure: L1, L2, ..., Ln where
 *            Li is the simulated loss of the i-th segment (n = number of
 *            segments of the segmentation).
 */
bool ccruncher::MonteCarlo::append(int ithread, const std::vector<short> &vi,
    const double *losses) noexcept
{
  assert(losses != nullptr);
  assert(vi.size() == blocksize);

  pthread_mutex_lock(&mutex);
  bool more = true;

  try
  {
    for(size_t iblock=0; iblock<vi.size(); iblock++)
    {
      // trace indexes (if required)
      if (findexes.is_open()) {
        findexes << ithread << ", " << std::abs(int(vi[iblock]))
                 << ", " << (vi[iblock]<0?1:0) << "\n";
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
           (stop != nullptr && *stop)
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

/**************************************************************************//**
 * @param[in] path Path to oputput directory.
 * @param[in] mode Output file mode (a=append, w=overwrite, c=create).
 * @param[in] indexes Create file indexes.csv containing info about simulation
 *            procedure.
 * @throw std::exception Error creating files.
 */
void ccruncher::MonteCarlo::setFilePath(const string &path, char mode, bool indexes)
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

/**************************************************************************//**
 * @return Number of iterations done.
 */
int ccruncher::MonteCarlo::getNumIterations() const
{
  return numiterations;
}

/**************************************************************************//**
 * @return Maximum number of iterations to do/done.
 */
int ccruncher::MonteCarlo::getMaxIterations() const
{
  return maxiterations;
}

