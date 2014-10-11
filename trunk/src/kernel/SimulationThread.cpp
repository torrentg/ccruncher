
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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
ccruncher::SimulationThread::SimulationThread(MonteCarlo &mc, ulong seed) :
  Thread(), montecarlo(mc), obligors(mc.obligors), numSegmentsBySegmentation(mc.numSegmentsBySegmentation), 
  chol(mc.chol), floadings1(mc.floadings1), floadings2(mc.floadings2), inverses(mc.inverses),
  numfactors(mc.chol->size1), ndf(mc.ndf), time0(mc.time0), timeT(mc.timeT),
  antithetic(mc.antithetic), numsegments(mc.numsegments),
  blocksize(mc.blocksize), rng(nullptr)
{
  assert(chol != nullptr);
  assert(blocksize > 0);
  assert(numfactors == floadings1.size());
  assert(antithetic?(blocksize%2!=0?false:true):true);

  rng = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(rng, seed);
}

/**************************************************************************/
ccruncher::SimulationThread::~SimulationThread()
{
  if (rng != nullptr) gsl_rng_free(rng);
}

/**************************************************************************//**
 * @details Does simulations sending results to master until it indicates
 *          to stop.
 * @see Thread::run()
 */
void ccruncher::SimulationThread::run()
{
  vector<vector<double>> losses(blocksize, vector<double>(numsegments, 0.0));
  vector<vector<double>> z(blocksize/(antithetic?2:1), vector<double>(numfactors, 0.0));
  vector<double> s(blocksize/(antithetic?2:1), 1.0);
  vector<double> x(blocksize/(antithetic?2:1), 0.0);
  bool more = true;

  timer1.reset();
  timer2.reset();
  timer3.reset();

  while(more)
  {
    // simulating latent variables
    timer1.resume();
    rchisq(s);
    rmvnorm(z);
    timer1.stop();

    timer2.resume();

    // reset aggregated values
    for(size_t i=0; i<losses.size(); i++) {
      fill(losses[i].begin(), losses[i].end(), 0.0);
    }

    for(size_t iobligor=0; iobligor<obligors.size(); iobligor++)
    {
      // simulating default times
      Timer timer11(true);
      unsigned char ifactor = obligors[iobligor].ifactor;
      for(size_t j=0; j<x.size(); j++) {
        x[j] = s[j] * (z[j][ifactor] + floadings2[ifactor]*gsl_ran_gaussian_ziggurat(rng, 1.0));
      }
      timer11.stop();
      timer1 += timer11.read();
      timer2 -= timer11.read();

      // simulating obligor loss
      for(size_t j=0; j<blocksize; j++)
      {
        double val = NAN;

        if (!antithetic) {
          val = x[j];
        }
        else {
          if (j%2) {
            val = +x[j/2];
          }
          else {
            val = -x[j/2];
          }
        }

        assert(std::isfinite(val));
        int r = obligors[iobligor].irating;
        double days = inverses[r].evalue(val);
        Date timeDefault = time0 + (long)ceil(days);

        if (timeDefault <= timeT)
        {
          simuleObligorLoss(obligors[iobligor], timeDefault, losses[j]);
        }
      }
    }
    timer2.stop();

    // data transfer
    timer3.resume();
    more = montecarlo.append(losses);
    timer3.stop();
  }
}

/**************************************************************************//**
 * @details Fill the vector s with random chi-square values.
 * @param[out] ret Vector to fill.
 */
void ccruncher::SimulationThread::rchisq(vector<double> &ret)
{
  if (ndf > 0.0) {
    for(size_t n=0; n<ret.size(); n++) {
      double chisq = gsl_ran_chisq(rng, ndf);
      if (chisq < 1e-14) chisq = 1e-14; //avoid division by 0
      ret[n] = sqrt(ndf/chisq);
    }
  }
}

/**************************************************************************//**
 * @details Fill the matrix z with random multivariate Gaussian values.
 * @param[out] ret Vector to fill.
 */
void ccruncher::SimulationThread::rmvnorm(vector<vector<double>> &ret)
{
  gsl_vector aux;
  aux.size = numfactors;
  aux.stride = 1;
  aux.data = nullptr;
  aux.block = nullptr;
  aux.owner = 0;

  for(size_t n=0; n<ret.size(); n++) {
    aux.data = ret[n].data();
    for(size_t i=0; i<numfactors; i++) {
      aux.data[i] = gsl_ran_gaussian_ziggurat(rng, 1.0);
    }
    gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, chol, &aux);
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

  for(size_t i=0; i<obligor.assets.size(); i++)
  {
    const Asset &asset = obligor.assets[i];
    
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
        ushort isegment = asset.segments[iSegmentation];
        assert(isegment < numSegmentsBySegmentation[iSegmentation]);
        plosses[isegment] += loss;
        plosses += numSegmentsBySegmentation[iSegmentation];
      }
    }
  }
}

/**************************************************************************//**
 * @return Elapsed time creating random numbers (in seconds).
 */
double ccruncher::SimulationThread::getElapsedTime1()
{
  return timer1.read();
}

/**************************************************************************//**
 * @return Elapsed time simulating losses (in seconds).
 */
double ccruncher::SimulationThread::getElapsedTime2()
{
  return timer2.read();
}

/**************************************************************************//**
 * @return Elapsed time writing to disk (in seconds).
 */
double ccruncher::SimulationThread::getElapsedTime3()
{
  return timer3.read();
}

