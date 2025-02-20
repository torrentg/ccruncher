
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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
#include <numeric>
#include <algorithm>
#include <cassert>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_blas.h>
#include "kernel/SimulationThread.hpp"
#include "portfolio/DateValues.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] mc MonteCarlo manager.
 * @param[in] seed RNG seed.
 */
ccruncher::SimulationThread::SimulationThread(MonteCarlo &mc, unsigned long seed) :
  Thread(), montecarlo(mc), obligors(mc.obligors), numSegmentsBySegmentation(mc.numSegmentsBySegmentation), 
  chol(mc.chol), floadings2(mc.floadings2), inverses(mc.inverses),
  numfactors(mc.chol->size1), ndf(mc.ndf), time0(mc.time0), timeT(mc.timeT),
  antithetic(mc.antithetic), numsegments(mc.numsegments),
  blocksize(mc.blocksize), rng(nullptr), vec(nullptr)
{
  assert(blocksize > 0);
  assert(numfactors > 0);
  assert(chol != nullptr);
  assert(chol->size1 == chol->size2);
  assert(chol->size1 == numfactors);
  assert(numfactors == floadings2.size());
  assert(antithetic?(blocksize%2!=0?false:true):true);

  vec = gsl_vector_alloc(numfactors);
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(rng, seed);
}

/**************************************************************************/
ccruncher::SimulationThread::~SimulationThread()
{
  gsl_rng_free(rng);
  gsl_vector_free(vec);
}

/**************************************************************************//**
 * @details Does simulations sending results to master until it indicates
 *          to stop.
 * @see Thread::run()
 */
void ccruncher::SimulationThread::run()
{
  vector<vector<double>> losses(blocksize, vector<double>(numsegments, 0.0));
  vector<vector<double>> z(numfactors, vector<double>(blocksize/(antithetic?2:1), 0.0));
  vector<double> s(blocksize/(antithetic?2:1), 1.0);
  vector<double> x(blocksize/(antithetic?2:1), 0.0);
  bool more = true;

  while(more)
  {
    // simulating latent variables
    rchisq(s);
    rmvnorm(z);

    // reset aggregated values
    for(size_t i=0; i<losses.size(); i++) {
      fill(losses[i].begin(), losses[i].end(), 0.0);
    }

    for(size_t iobligor=0; iobligor<obligors.size(); iobligor++)
    {
      // simulating iid N(0,1) values (epsilons)
      for(size_t j=0; j<x.size(); j++) {
        x[j] = gsl_ran_gaussian_ziggurat(rng, 1.0);
      }

      // simulating multi-variate t-student
      unsigned char ifactor = obligors[iobligor].ifactor;

      for(size_t j=0; j<x.size(); j++) {
        // z[ifactor] values are already multiplied by w[ifactor] (see chol matrix creation)
        x[j] = s[j] * (z[ifactor][j] + floadings2[ifactor]*x[j]);
      }

      // simulating obligor loss
      for(size_t j=0; j<blocksize; j++)
      {
        double val = getValue(x, j);
        unsigned char irating = obligors[iobligor].irating;
        double days = inverses[irating].evalue(val);
        Date timeDefault = time0 + (long)ceil(days);

        if (timeDefault <= timeT) {
          simuleObligorLoss(obligors[iobligor], timeDefault, losses[j]);
        }
      }
    }

    // data transfer
    more = montecarlo.append(losses);
  }
}

/**************************************************************************//**
 * @param[in] x Vector of simulated t-student values (only iobligor component).
 * @param[in] j Index of the simulation into the block.
 * @return t-Student component.
 */
inline double ccruncher::SimulationThread::getValue(const vector<double> &x, size_t j)
{
  if (!antithetic) {
    return x[j];
  }
  else {
    if (j%2) {
      return +x[j/2];
    }
    else {
      return -x[j/2];
    }
  }
}

/**************************************************************************//**
 * @details Fill the vector s with random chi-square values.
 * @param[out] s Vector to fill.
 */
void ccruncher::SimulationThread::rchisq(vector<double> &s)
{
  if (isfinite(ndf)) {
    for(size_t n=0; n<s.size(); n++) {
      double chisq = gsl_ran_chisq(rng, ndf);
      if (chisq < 1e-14) chisq = 1e-14; //avoid division by 0
      s[n] = sqrt(ndf/chisq);
    }
  }
}

/**************************************************************************//**
 * @details Fill the matrix z with random multivariate Gaussian values.
 * @param[out] z Vector to fill.
 */
void ccruncher::SimulationThread::rmvnorm(vector<vector<double>> &z)
{
  size_t len = z[0].size();
  for(size_t n=0; n<len; n++) {
    //TODO: use gsl_ran_multivariate_gaussian() new in gsl-2.2
    for(size_t i=0; i<numfactors; i++) {
      gsl_vector_set(vec, i, gsl_ran_gaussian_ziggurat(rng, 1.0));
    }
    gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, chol, vec);
    for(size_t i=0; i<numfactors; i++) {
      z[i][n] = gsl_vector_get(vec, i);
    }
  }
}

/**************************************************************************//**
 * @details Given a default time simulates obligors losses and aggregates
 *          them in the corresponding segmentation-segment.
 * @param[in] obligor Obligor to simulate.
 * @param[in] dtime Default time.
 * @param[out] losses Cumulated losses by segmentation-segment.
 */
void ccruncher::SimulationThread::simuleObligorLoss(const Obligor &obligor, Date dtime, vector<double> &losses) const noexcept
{
  double obligor_lgd = NAN;

  for(const Asset &asset : obligor.assets)
  {
    // evalue asset loss
    if (dtime <= asset.values.back().date)
    {
      auto item = lower_bound(asset.values.begin(), asset.values.end(), dtime);
      double ead = item->ead.getValue(rng);
      double lgd = item->lgd.getValue(rng);

      // non-lgd means that is inherited from obligor
      if (std::isnan(lgd)) {
        if (std::isnan(obligor_lgd)) {
          obligor_lgd = obligor.lgd.getValue(rng);
        }
        lgd = obligor_lgd;
      }

      // compute asset loss
      double loss = ead * lgd;
      assert(std::isfinite(loss));

      // aggregate asset loss in the correspondent segment loss
      double *plosses = losses.data();
      for(size_t iSegmentation=0; iSegmentation<numSegmentsBySegmentation.size(); iSegmentation++)
      {
        unsigned short isegment = asset.segments[iSegmentation];
        assert(isegment < numSegmentsBySegmentation[iSegmentation]);
        plosses[isegment] += loss;
        plosses += numSegmentsBySegmentation[iSegmentation];
      }
    }
  }
}

