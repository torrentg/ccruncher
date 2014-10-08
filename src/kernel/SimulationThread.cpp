
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
  blocksize(mc.blocksize), rng(nullptr), losses(0)
{
  assert(chol != nullptr);
  assert(blocksize > 0);
  assert(numfactors == floadings1.size());
  assert(antithetic?(blocksize%2!=0?false:true):true);

  rng = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(rng, seed);

  losses.assign(numsegments*blocksize, 0.0);
  x.assign(blocksize/(antithetic?2:1), 0.0);
  z.assign((blocksize*numfactors)/(antithetic?2:1), 0.0);
  s.assign(blocksize/(antithetic?2:1), 1.0);
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
  bool more = true;

  timer1.reset();
  timer2.reset();
  timer3.reset();

  while(more)
  {
    // simulating latent values (fill arrays z and s)
    timer1.resume();
    rchisq();
    rmvnorm();
    timer1.stop();

    timer2.resume();

    // reset aggregated values
    fill(losses.begin(), losses.end(), 0.0);

    for(size_t iobligor=0; iobligor<obligors.size(); iobligor++)
    {
      // simulating default times
      Timer timer11(true);
      unsigned char ifactor = obligors[iobligor].ifactor;
      for(size_t j=0; j<x.size(); j++) {
        x[j] = s[j] * (z[j*numfactors+ifactor] + floadings2[ifactor]*gsl_ran_gaussian_ziggurat(rng, 1.0));
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
          double *ptr_losses = (double*) &(losses[j*numsegments]);
          simuleObligorLoss(obligors[iobligor], timeDefault, ptr_losses);
        }
      }
    }
    timer2.stop();

    // data transfer
    timer3.resume();
    more = montecarlo.append((double*) &(losses[0]));
    timer3.stop();
  }
}

/**************************************************************************//**
 * @details Fill the vector s with random chi-square values.
 */
void ccruncher::SimulationThread::rchisq()
{
  if (ndf > 0.0) {
    for(size_t n=0; n<s.size(); n++) {
      double chisq = gsl_ran_chisq(rng, ndf);
      if (chisq < 1e-14) chisq = 1e-14; //avoid division by 0
      s[n] = sqrt(ndf/chisq);
    }
  }
}

/**************************************************************************//**
 * @details Fill the matrix z with random multivariate Gaussian values.
 */
void ccruncher::SimulationThread::rmvnorm()
{
  gsl_vector auxz;
  auxz.size = numfactors;
  auxz.stride = 1;
  auxz.data = (double *) &(z[0]);
  auxz.block = nullptr;
  auxz.owner = 0;

  for(size_t n=0; n<s.size(); n++) {
    for(size_t i=0; i<numfactors; i++) {
      auxz.data[i] = gsl_ran_gaussian_ziggurat(rng, 1.0);
    }
    gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, chol, &auxz);
    auxz.data += numfactors;
  }
}

/**************************************************************************//**
 * @details Given a default time simulates obligors losses and aggregates
 *          them in the corresponding segmentation-segment.
 * @param[in] obligor Obligor to simulate.
 * @param[in] dtime Default time.
 * @param[out] closses Cumulated losses by segmentation-segment.
 */
void ccruncher::SimulationThread::simuleObligorLoss(const Obligor &obligor, Date dtime, double *closses) const noexcept
{
  assert(closses != nullptr);

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

      // aggregate asset loss in the correspondent segment loss
      double *vlosses = closses;
      for(size_t iSegmentation=0; iSegmentation<numSegmentsBySegmentation.size(); iSegmentation++)
      {
        ushort isegment = asset.segments[iSegmentation];
        assert(isegment < numSegmentsBySegmentation[iSegmentation]);
        vlosses[isegment] += loss;
        vlosses += numSegmentsBySegmentation[iSegmentation];
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

