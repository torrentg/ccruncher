
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

#include <numeric>
#include <algorithm>
#include <cassert>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_blas.h>
#include "kernel/SimulationThread.hpp"
#include "portfolio/DateValues.hpp"

using namespace std;
using namespace ccruncher;

#define GAUSSIAN_POOL_SIZE 512
#define LHS_VALUES_Z(i,j) lhs_values_z[(i)*numfactors+(j)]

/**************************************************************************//**
 * @param[in] ti Thread identifier.
 * @param[in] mc MonteCarlo manager.
 * @param[in] seed RNG seed.
 */
ccruncher::SimulationThread::SimulationThread(int ti, MonteCarlo &mc, unsigned long seed) :
  Thread(), montecarlo(mc), obligors(mc.obligors), assets(mc.assets), segments(mc.segments),
  numSegmentsBySegmentation(mc.numSegmentsBySegmentation), chol(mc.chol),
  floadings1(mc.floadings1), floadings2(mc.floadings2), inverses(mc.inverses),
  numfactors(mc.chol->size1), ndf(mc.ndf), time0(mc.time0), timeT(mc.timeT),
  antithetic(mc.antithetic), numsegments(mc.numsegments),
  blocksize(mc.blocksize), lhs_size(mc.lhs_size), rng(nullptr), losses(0)
{
  assert(chol != nullptr);
  assert(blocksize > 0);
  assert(numfactors == floadings1.size());
  assert(antithetic?(blocksize%2!=0?false:true):true);

  id = ti;

  rng = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(rng, seed);
  rngpool.resize(GAUSSIAN_POOL_SIZE);
  rngpool_pos = GAUSSIAN_POOL_SIZE;

  losses.resize(numsegments*blocksize, 0.0);
  indexes.resize(blocksize);

  lhs_num = 0;
  lhs_pos = lhs_size;
  lhs_values_z.resize(lhs_size*numfactors);
  lhs_values_s.resize(lhs_size);
  if (lhs_size > 1 && numfactors > 1) {
    lhs_aux.resize(lhs_size);
  }

  x.resize(blocksize/(antithetic?2:1));
  z.resize((blocksize*numfactors)/(antithetic?2:1));
  s.resize(blocksize/(antithetic?2:1));
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

  lhs_num = 0;
  lhs_pos = lhs_size;
  rngpool_pos = GAUSSIAN_POOL_SIZE;

  while(more)
  {
    // simulating latent values (fill arrays z and s)
    timer1.resume();
    simuleLatentVars();
    timer1.stop();

    timer2.resume();
    for(size_t iobligor=0, iasset=0, isegment=0; iobligor<obligors.size(); iobligor++)
    {
      unsigned char ifactor = obligors[iobligor].ifactor;

      // simulating default times
      for(size_t j=0; j<x.size(); j++)
      {
        // TODO: store z as: 1111222233334444 (eg. 4-factors)
        //       instead of: 1234123412341234 (num=ith-factor)
        //       to take advantage of cache line size
        x[j] = s[j] * (z[j*numfactors+ifactor] + floadings2[ifactor]*rnorm());
      }

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

        assert(!std::isnan(val));
        int r = obligors[iobligor].irating;
        double days = inverses[r].evalue(val);
        Date ddate = time0 + (long)ceil(days);

        if (ddate <= timeT)
        {
          const SimulatedAsset *ptr_assets = &(assets[iasset]);
          const unsigned short *ptr_segments = &(segments[isegment]);
          double *ptr_losses = (double*) &(losses[j*numsegments]);
          simuleObligorLoss(obligors[iobligor], ddate, ptr_assets, ptr_segments, ptr_losses);
        }
      }

      iasset += obligors[iobligor].numassets;
      isegment += obligors[iobligor].numassets * numSegmentsBySegmentation.size();
    }
    timer2.stop();

    timer3.resume();
    // data transfer
    more = montecarlo.append(id, indexes, (double*) &(losses[0]));
    // reset aggregated values
    fill(losses.begin(), losses.end(), 0.0);
    timer3.stop();
  }
}

/**************************************************************************//**
 * @details Simulate blocksize times (only half when antithetic) the latent
 *          variables: factors (z), chi-square (s). Sets simulation info in
 *          indexes array.
 */
inline void ccruncher::SimulationThread::simuleLatentVars()
{
  double *ptrz = &(z[0]);

  // simulating latent variables
  for(size_t j=0; j<s.size(); j++)
  {
    // latin hypercube sample management
    if (lhs_pos+1 >= lhs_size) {
      lhs_num++;
      lhs_pos = 0;
      rchisq();
      rmvnorm();
    }
    else {
      lhs_pos++;
    }

    double *auxz = (double *) &(LHS_VALUES_Z(lhs_pos,0));
    memcpy(ptrz, auxz, numfactors*sizeof(double));
    ptrz += numfactors;

    if (ndf > 0) {
      double chisq = lhs_values_s[lhs_pos];
      if (chisq < 1e-14) chisq = 1e-14; //avoid division by 0
      s[j] = sqrt(ndf/chisq);
    }
    else {
      s[j] = 1.0;
    }

    if (!antithetic) {
      indexes[j] = +lhs_num;
    }
    else {
      indexes[2*j+0] = +lhs_num;
      indexes[2*j+1] = -lhs_num;
    }
  }
}

/**************************************************************************//**
 * @param[in] o1 First pair (value-position).
 * @param[in] o2 Second pair (value-position).
 * @return true if o1.value < o2.value, false otherwise
 */
inline bool ccruncher::SimulationThread::pcomparator(const std::pair<double,size_t> &o1,
    const std::pair<double,size_t> &o2)
{
  return o1.first < o2.first;
}

/**************************************************************************//**
 * @details Generate a sample of lhs_size random chi-square values.
 *          Let values in lhs_values_s.
 */
void ccruncher::SimulationThread::rchisq()
{
  if (ndf > 0.0)
  {
    if (lhs_size == 1)
    {
      // raw Monte Carlo
      lhs_values_s[0] = gsl_ran_chisq(rng, ndf);
    }
    else
    {
      // randomizer wrapper
      frand f_rand(rng);

      // Latin Hypercube Sampling (chisq variable)
      for(size_t n=0; n<lhs_size; n++)
      {
        double u = gsl_ran_flat(rng, double(n)/double(lhs_size), double(n+1)/double(lhs_size));
        lhs_values_s[n] = gsl_cdf_chisq_Pinv(u, ndf);
        assert(!std::isnan(lhs_values_s[n]) && !std::isinf(lhs_values_s[n]));
      }
      std::random_shuffle(lhs_values_s.begin(), lhs_values_s.end(), f_rand);
    }
  }
}

/**************************************************************************//**
 * @details Generate a sample of lhs_size random multivariate Gaussian
 *          corresponding to factors.
 *          Let values in lhs_values_z.
 */
void ccruncher::SimulationThread::rmvnorm()
{
  if (numfactors == 1 && lhs_size > 1)
  {
    // randomizer wrapper
    frand f_rand(rng);

    // Latin Hypercube Sampling (1-factor variable)
    for(size_t n=0; n<lhs_size; n++)
    {
      double u = gsl_ran_flat(rng, double(n)/double(lhs_size), double(n+1)/double(lhs_size));
      // we multiply by factor loading to reduce the number of mults in rmvdist
      lhs_values_z[n] = gsl_cdf_ugaussian_Pinv(u) * floadings1[0];
    }
    std::random_shuffle(lhs_values_z.begin(), lhs_values_z.end(), f_rand);
  }
  else // numfactors > 1 || lhs_size == 1
  {
    gsl_vector auxz;
    auxz.size = numfactors;
    auxz.stride = 1;
    auxz.data = (double *) &(lhs_values_z[0]);
    auxz.block = nullptr;
    auxz.owner = 0;

    // creating sample of size lhs_size
    for(size_t n=0; n<lhs_size; n++)
    {
      // simulating N(0,R)
      for(size_t i=0; i<numfactors; i++) {
        auxz.data[i] = rnorm();
      }
      gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, chol, &auxz);
      auxz.data += numfactors;
    }

    // if lhs enabled and numfactors>1
    if (lhs_size > 1)
    {
      // Latin Hypercube Sampling with Dependence
      // see 'Latin hypercube sampling with dependence and applications in finance'
      // by Natalie Packham, Wolfgang Schmidt

      // caution:
      // document contains an errata in (pag 5) refered to the
      // distribution of the k-th order statistic of n independent uniform random variables
      // the Feller reference also is incorrect.
      // see http://en.wikipedia.org/wiki/Order_statistic#The_order_statistics_of_the_uniform_distribution

      for(size_t i=0; i<numfactors; i++)
      {
        for(size_t n=0; n<lhs_size; n++) {
          lhs_aux[n].first = LHS_VALUES_Z(n,i);
          lhs_aux[n].second = n;
        }

        // obs: CDF preserves rank [ rank(X) = rank(CDF(X)) ]
        std::sort(lhs_aux.begin(), lhs_aux.end(), SimulationThread::pcomparator);

        for(size_t n=0; n<lhs_size; n++) {
          size_t pos = lhs_aux[n].second;
          // we avoid reusing always the same values and allowing 'extreme' events
          //double u = (n+0.5)/double(lhs_size);
          double u = gsl_cdf_ugaussian_P(LHS_VALUES_Z(pos,i));
          //double u = lhs_aux[n].first;
          double eta = gsl_cdf_beta_P(u, n+1, lhs_size-n);
          u = (n+eta)/double(lhs_size);
          LHS_VALUES_Z(pos,i) = gsl_cdf_ugaussian_Pinv(u);
        }
      }

      // multiplying factors by loadings to reduce the number of mults
      // because is not multiplied by w when lhs>1 to allow previous step
      for(size_t n=0; n<lhs_size; n++) {
        for(size_t i=0; i<numfactors; i++) {
          LHS_VALUES_Z(n,i) *= floadings1[i];
          assert(!std::isnan(LHS_VALUES_Z(n,i)) && !std::isinf(LHS_VALUES_Z(n,i)));
        }
      }
    }
  }
}

/**************************************************************************//**
 * @details Given a default time simulates obligors losses and aggregates
 *          them in the corresponding segmentation-segment.
 * @param[in] obligor Obligor to simulate.
 * @param[in] dtime Default time.
 * @param[in] vassets List of assets.
 * @param[in] vsegments List of segments.
 * @param[out] vlosses Cumulated losses by segmentation-segment.
 */
void ccruncher::SimulationThread::simuleObligorLoss(const SimulatedObligor &obligor,
    Date dtime, const SimulatedAsset *vassets, const unsigned short *vsegments,
    double *vlosses) const noexcept
{
  assert(vassets != nullptr);
  assert(vsegments != nullptr);
  assert(vlosses != nullptr);

  double obligor_lgd = NAN;

  for(unsigned short i=0; i<obligor.numassets; i++)
  {
    // evalue asset loss
    if (dtime <= vassets[i].maxdate && vassets[i].mindate <= dtime)
    {
      DateValues *values = lower_bound(vassets[i].begin, vassets[i].end, dtime);
      assert(dtime <= (vassets[i].end-1)->date);
      double ead = values->ead.getValue(rng);
      double lgd = values->lgd.getValue(rng);

      // non-lgd means that is inherited from obligor
      if (std::isnan(lgd))
      {
        if (std::isnan(obligor_lgd))
        {
          obligor_lgd = obligor.ref.lgd.getValue(rng);
        }
        lgd = obligor_lgd;
      }

      // compute asset loss
      double loss = ead * lgd;

      // aggregate asset loss in the correspondent segment loss
      // remember: obligor segments was recoded to assets segments
      double *closses = vlosses;
      for(size_t j=0; j<numSegmentsBySegmentation.size(); j++)
      {
        unsigned short isegment = vsegments[j];
        assert(isegment < numSegmentsBySegmentation[j]);
        closses[isegment] += loss;
        closses += numSegmentsBySegmentation[j];
      }
      vsegments += numSegmentsBySegmentation.size();
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

/**************************************************************************//**
 * @return Simulated N(0,1) value.
 */
inline double ccruncher::SimulationThread::rnorm()
{
  rngpool_pos++;
  if (rngpool_pos >= GAUSSIAN_POOL_SIZE) fillGaussianPool();
  return rngpool[rngpool_pos];
}

/**************************************************************************//**
 * @details We use the ziggurat algorithm because is very fast but only if
 *          it is called consecutively.
 */
void ccruncher::SimulationThread::fillGaussianPool()
{
  Timer timer(false);
  if (timer2.isRunning()) timer.start();

  for(size_t i=0; i<GAUSSIAN_POOL_SIZE; i++)
  {
    rngpool[i] = gsl_ran_gaussian_ziggurat(rng, 1.0);
  }
  rngpool_pos = 0;

  if (timer.isRunning()) {
    timer.stop();
    timer1 += timer.read();
    timer2 -= timer.read();
  }
}

