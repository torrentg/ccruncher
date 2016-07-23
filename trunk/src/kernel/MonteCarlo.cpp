
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#include <set>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>
#include "kernel/MonteCarlo.hpp"
#include "kernel/Aggregator.hpp"
#include "kernel/SimulationThread.hpp"
#include "portfolio/Asset.hpp"
#include "portfolio/DateValues.hpp"
#include "params/Params.hpp"
#include "params/DefaultProbabilities.hpp"
#include "params/Factors.hpp"
#include "params/Correlations.hpp"
#include "params/Segmentations.hpp"
#include "utils/Utils.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace std::chrono;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] s Streambuf where the trace will be written.
 */
ccruncher::MonteCarlo::MonteCarlo(std::streambuf *s) :
    logger(s), chol(nullptr), mStop(nullptr), mStatus(status::fresh)
{
  maxseconds = 0UL;
  numiterations = 0UL;
  maxiterations = 0UL;
  antithetic = false;
  blocksize = 1;
  seed = 0UL;
  mHash = 0UL;
  nfthreads = 0UL;
  time0 = NAD;
  timeT = NAD;
  ndf = NAN;
  numsegments = 0UL;
}

/**************************************************************************/
ccruncher::MonteCarlo::~MonteCarlo()
{
  freeMemory();
}

/**************************************************************************/
void ccruncher::MonteCarlo::freeMemory()
{
  // removing threads
  for(auto &thread : threads) {
    delete thread;
    thread = nullptr;
  }
  threads.clear();

  // dropping aggregators
  for(auto &aggregator : aggregators) {
    delete aggregator;
    aggregator = nullptr;
  }
  aggregators.clear();

  // deallocating cholesky matrix
  gsl_matrix_free(chol);
  chol = nullptr;

  // flushing remaining objects
  numSegmentsBySegmentation.clear();
  obligors.clear();
  inverses.clear();
  floadings1.clear();
  floadings2.clear();
}

/**************************************************************************//**
 * @details Calls MonteCarlo::setXXX() methods using IData content.
 * @param[in] data CCruncher input file.
 * @param[in] path Directory path where output files will be put.
 * @param[in] mode Output file open mode.
 * @throw Exception Error initializing object.
 */
void ccruncher::MonteCarlo::init(Input &data, const string &path, char mode)
{
  try
  {
    setParams(data.getParams());
    setDefaultProbabilities(data.getCDFs());
    setFactorLoadings(data.getFactorLoadings());
    setCorrelations(data.getCorrelations());
    setObligors(data.getPortfolio());
    setInverses();
    setSegmentations(data.getSegmentations(), path, mode);
    mStatus = status::initialized;
  }
  catch(std::exception &e)
  {
    freeMemory();
    mStatus = status::error;
    throw Exception(e, "error initializing Monte Carlo");
  }
}

/**************************************************************************//**
 * @see http://www.ccruncher.net/ifileref.html#parameters
 * @see Params.hpp
 * @param[in] parameters List of parameters.
 * @throw Exception Invalid parameters.
 */
void ccruncher::MonteCarlo::setParams(const map<string,string> &parameters)
{
  // check parameters
  Params params(parameters.begin(), parameters.end());
  params.isValid(true);

  // assign parameters
  maxseconds = params.getMaxSeconds();
  maxiterations = params.getMaxIterations();
  time0 = params.getTime0();
  timeT = params.getTimeT();
  antithetic = params.getAntithetic();
  blocksize = params.getBlockSize();
  ndf = params.getNdf();
  seed = params.getRngSeed();

  // seed based on clock (if not set)
  if (seed == 0UL) {
    seed = Utils::trand();
  }
}

/**************************************************************************//**
 * @see http://www.ccruncher.net/ifileref.html#dprobs
 * @see DefaultProbabilities.hpp
 * @param[in] dprobs List of PDs.
 * @throw Exception Invalid PDs list.
 */
void ccruncher::MonteCarlo::setDefaultProbabilities(const vector<CDF> &cdfs)
{
  DefaultProbabilities::isValid(cdfs, true);
  dprobs = cdfs;
}

/**************************************************************************//**
 * @see http://www.ccruncher.net/ifileref.html#factors
 * @param[in] loadings List of factor loadings.
 * @throw Exception Invalid factor loading list.
 */
void ccruncher::MonteCarlo::setFactorLoadings(const vector<double> &loadings)
{
  // check factors
  Factors::isValid(loadings, true);

  // set factor loadings
  floadings1 = loadings;

  // performance tip: precompute sqrt(1-w^2)
  floadings2 = floadings1;
  for(size_t i=0; i<floadings2.size(); i++) {
    floadings2[i] = sqrt(1.0 - floadings1[i]*floadings1[i]);
  }
}

/**************************************************************************//**
 * @see http://www.ccruncher.net/ifileref.html#factors
 * @param[in] correlations Factor correlation matrix.
 * @throw Exception Invalid correlation matrix.
 */
void ccruncher::MonteCarlo::setCorrelations(const vector<vector<double>> &correlations)
{
  // check correlations
  Correlations::isValid(correlations, true);
  if (correlations.size() != floadings1.size()) {
    throw Exception("invalid correlation matrix dim");
  }

  // obtain cholesky decomposition
  gsl_matrix_free(chol);
  chol = nullptr;
  chol = Correlations::getCholesky(correlations);

  // performance tip: chol contains w·chol
  for(size_t i=0; i<chol->size1; i++) {
    for(size_t j=0; j<chol->size2; j++) {
      double val = gsl_matrix_get(chol, i, j)*floadings1[i];
      gsl_matrix_set(chol, i, j, val);
    }
  }
}

/**************************************************************************//**
 * @details Imports portfolio obligors (portfolio returns empty)
 * @param[in] obligors List of obligors.
 * @throw Exception Empty list or exists an invalid obligor.
 */
void ccruncher::MonteCarlo::setObligors(vector<Obligor> &portfolio)
{
  assert(chol != nullptr);
  size_t numFactors = chol->size1;
  size_t numRatings = dprobs.size();

  for(const Obligor &obligor : portfolio) {
    if (obligor.ifactor >= numFactors || obligor.irating >= numRatings) {
      throw Exception("invalid obligor");
    }
  }

  if (portfolio.empty()) {
    throw Exception("0 obligors to simulate");
  }

  obligors.clear();
  obligors.swap(portfolio);
  sort(obligors.begin(), obligors.end(),
       [](const Obligor &a, const Obligor &b) -> bool {
         return a.irating < b.irating;
       });

}

/**************************************************************************//**
 * @details Creates the list of simulated assets. This initialization stage
 *          sets SimulatedObligor::ref to the simulated asset.
 * @param[in] segmentations List of segmentations.
 * @param[in] path Directory path where output will be placed.
 * @param[in] mode File creation mode: a (append), w (overwrite), c (create)
 * @throw Exception Error initializing object.
 */
void ccruncher::MonteCarlo::setSegmentations(const vector<Segmentation> &segmentations, const string &path, char mode)
{
  // checking segmentations
  Segmentations::isValid(segmentations, true);

  // checking obligor's data related to segmentation-segments
  for(Obligor &obligor : obligors) {
    for(Asset &asset : obligor.assets) {
      // check that the number of segmentations is correct
      if (asset.segments.size() != segmentations.size()) {
        throw Exception("obligor with invalid number of segmentations");
      }
      // check that segmentation-segment identifiers are valid
      for(unsigned short isegmentation=0; isegmentation<segmentations.size(); isegmentation++) {
        if (asset.segments[isegmentation] >= segmentations[isegmentation].size()) {
          throw Exception("obligor with invalid segment");
        }
      }
    }
  }

  // removing previous aggregators
  for(auto &aggregator : aggregators) {
    delete aggregator;
    aggregator = nullptr;
  }
  aggregators.clear();

  // allocating and initializing aggregators
  numsegments = 0UL;
  numSegmentsBySegmentation.assign(segmentations.size(), 0);
  aggregators.assign(segmentations.size(), nullptr);
  
  for(unsigned short isegmentation=0; isegmentation<(unsigned short)(segmentations.size()); isegmentation++) 
  {
    const Segmentation &segmentation = segmentations[isegmentation];
    
    numSegmentsBySegmentation[isegmentation] = segmentation.size();
    numsegments += numSegmentsBySegmentation[isegmentation];
    
    string ofile = segmentation.getFilename(path);
    Aggregator *aggregator = new Aggregator(ofile, mode, segmentation.size());
    
    vector<double> exposures = Segmentation::getExposures(isegmentation, obligors, time0, timeT);
    assert(exposures.size() == segmentation.size());
    aggregator->printHeader(segmentation, exposures);
    
    aggregators[isegmentation] = aggregator;
  }

  // tracing log info
  logger << endl;
  logger << "output files" << flood('*') << endl;
  logger << indent(+1);
  logger << "directory" << split << "[" + Utils::realpath(path) + "]" << endl;
  for(size_t i=0; i<aggregators.size(); i++) {
    logger << "segmentation" << split << "[" + aggregators[i]->getFilename() + "]" << endl;
  }
  logger << indent(-1);

}

/**************************************************************************//**
 * @details Create Finv(t(x)) spline functions. This is a strictly increasing
 *          function. We only need accuracy in the asset event dates.
 */
void ccruncher::MonteCarlo::setInverses()
{
  // obtaining date nodes
  set<Date> aux;
  for(const Obligor &obligor : obligors) {
    for(const Asset &asset : obligor.assets) {
      for(const DateValues &value: asset.values) {
        aux.insert(value.date);
      }
    }
  }

  // converting date nodes to day nodes
  vector<int> nodes;
  nodes.reserve(aux.size());
  for(auto it=aux.begin(); it!=aux.end(); ++it) {
    nodes.push_back(*it-time0);
  }
  sort(nodes.begin(), nodes.end());

  // create PDinv(t(x)) splines
  inverses.resize(dprobs.size());
  for(size_t i=0; i<dprobs.size(); i++) {
    inverses[i].init(ndf, timeT-time0, dprobs[i], nodes);
  }
}

/**************************************************************************//**
 * @details Starts the simulation procedure. If there is only 1 thread,
 *          then uses the current thread (simplifies debug), otherwise
 *          creates one simulation per thread.
 * @param[in] numthreads Number of threads to use (0 = num cores).
 * @param[in] nhash Number of simulations per hash (0 = no hashes).
 * @param[in] stop Variable to stop process from outside.
 * @return Exception Error running Monte Carlo.
 */
void ccruncher::MonteCarlo::run(unsigned char numthreads, size_t nhash, bool *stop)
{
  if (mStatus != status::initialized) {
    throw Exception("Monte Carlo not initialized");
  }

  if (stop != nullptr && *stop) return;

  mStatus = status::running;
  mStop = stop;
  mHash = nhash;

  // setting number of threads (if not set)
  if (numthreads == 0) {
    numthreads = Utils::getNumCores();
  }

  // check that number of threads is lower than number of iterations
  if (maxiterations > 0 && numthreads*blocksize > maxiterations) {
    numthreads = ceil(maxiterations/blocksize);
  }

  // tracing log info
  logger << endl;
  logger << "Monte Carlo" << flood('*') << endl;
  logger << indent(+1);
  logger << "seed used to initialize randomizer" << split << seed << endl;
  logger << "maximum execution time (seconds)" << split << maxseconds << endl;
  logger << "maximum number of iterations" << split << maxiterations << endl;
  logger << "antithetic mode" << split << antithetic << endl;
  logger << "block size" << split << blocksize << endl;
  logger << "number of threads" << split << int(numthreads) << endl;
  if (mHash != 0)  {
    logger << "running Monte Carlo";
    logger << " [" << to_string(mHash) << " simulations per hash]";
    logger << flood('-') << endl;
  }
  logger << indent(+1);

  // creating and launching simulation threads
  t1 = steady_clock::now();
  nfthreads = numthreads;
  numiterations = 0UL;
  threads.assign(numthreads, nullptr);
  for(int i=0; i<numthreads; i++)
  {
    threads[i] = new SimulationThread(*this, seed+i);
    if (numthreads == 1) {
      threads[i]->run();
    }
    else {
      threads[i]->start();
    }
  }

  // awaiting threads
  for(int i=0; i<numthreads; i++) {
    threads[i]->join();
    delete threads[i];
    threads[i] = nullptr;
  }
  threads.clear();

  // closing aggregators
  for(size_t i=0; i<aggregators.size(); i++) {
    delete aggregators[i];
    aggregators[i] = nullptr;
  }
  aggregators.clear();

  // exit function
  logger << indent(-1);
  if (nhash > 0) logger << endl;
  logger << "simulations realized" << split << numiterations << endl;
  auto t2 = steady_clock::now();
  long millis = duration_cast<milliseconds>(t2-t1).count();
  logger << "total simulation time" << split << Utils::millisToString(millis) << endl;
  logger << indent(-1) << endl;

  if (mStatus == status::error) {
    throw Exception("error running Monte Carlo");
  }
  else if (mStatus == status::running) {
    mStatus = status::finished;
  }
}

/**************************************************************************//**
 * @param[in] losses Simulated data. It is a matrix where each row contains
 *            the losses of one simulation. Rows have the following
 *            structure: S1, S2, ..., Sm where Si are the segments
 *            losses of the i-th segmentation (m=number of segmentations).
 *            Finally, Si has the following structure: L1, L2, ..., Ln where
 *            Li is the simulated loss of the i-th segment (n = number of
 *            segments of the segmentation).
 */
bool ccruncher::MonteCarlo::append(const vector<vector<double>> &losses) noexcept
{
  assert(losses.size() == blocksize);
  assert(!aggregators.empty());
  assert(aggregators.size() == numSegmentsBySegmentation.size());
  assert(nfthreads > 0);

  lock_guard<mutex> lock(mMutex);
  bool more = true;

  // if another thread has crashed
  if (mStatus == status::error) {
    goto mcExecErr;
  }

  try
  {
    for(size_t iblock=0; iblock<losses.size(); iblock++)
    {
      // aggregating simulation result
      assert(losses[iblock].size() == numsegments);
      const double *plosses = losses[iblock].data();
      for(size_t i=0; i<aggregators.size(); i++) {
        aggregators[i]->append(plosses);
        plosses += numSegmentsBySegmentation[i];
      }

      // counter increment
      numiterations++;

      // printing hashes
      if (mHash > 0 && numiterations%mHash == 0) {
        logger << '.' << flush;
      }

      // checking maximum number of iterations stop criterion
      if (maxiterations > 0 && numiterations + (nfthreads-1)*blocksize >= maxiterations) {
        more = false;
        break;
      }
    }
  }
  catch(Exception &e)
  {
    logger << "error: " << e << endl;
    mStatus = status::error;
  }

  // checking time stop criterion
  if (more && maxseconds > 0) {
    long secs = duration_cast<seconds>(steady_clock::now()-t1).count();
    if (secs >= (long)maxseconds) {
      more = false;
    }
  }

  // checking stop requested by user
  if (mStop != nullptr && *mStop) {
    more = false;
  }

mcExecErr:
  // if error or previous error
  if (mStatus == status::error) {
    more = false;
  }

  // exit function
  if (!more) nfthreads--;
  return(more);
}

/**************************************************************************//**
 * @return Number of iterations done.
 */
size_t ccruncher::MonteCarlo::getNumIterations() const
{
  return numiterations;
}

/**************************************************************************//**
 * @return Maximum number of iterations to do/done.
 */
size_t ccruncher::MonteCarlo::getMaxIterations() const
{
  return maxiterations;
}

