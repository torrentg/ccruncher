
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

#include <numeric>
#include <algorithm>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_blas.h>
#include "kernel/SimulationThread.hpp"
#include "portfolio/DateValues.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

/*
   Note on LHS method
   ------------------
   We allow LHS variance reduction technique only on random variables
   S and Z. We don't do LHS in X (all the obligors) because:
   - we can exhaust memory when num_obligors and lhs_size  are high
   - lhs with dependence is ineficient (beta eval)
   - if we use quick version (0.5 instead of beta) gives discrete values

   Observe that lhs_size=1 means that LHS is disabled.

   Variance reduction papers
   -------------------------
   * 'Monte Carlo methods for security pricing'
      by Phelim Boyle, Mark Broadie, Paul Glasserman
   * 'A user's guide to LHS: Sandia's Latin Hypercube Sampling Software'
      by Gregory D. Wyss, Kelly H. Jorgensen
   * 'Latin hypercube sampling with dependence and applications in finance'
      by Natalie Packham, Wolfgang Schmidt

 */

// --------------------------------------------------------------------------

#define GAUSSIAN_POOL_SIZE 512
#define LHS_VALUES_Z(i,j) lhs_values_z[(i)*numfactors+(j)]

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulationThread::SimulationThread(int ti, MonteCarlo &mc, unsigned long seed, unsigned short bs) :
  Thread(), montecarlo(mc), obligors(mc.obligors), numSegmentsBySegmentation(mc.numSegmentsBySegmentation),
  chol(mc.chol), floadings1(mc.floadings1), floadings2(mc.floadings2), inverses(mc.inverses),
  rng(NULL), losses(0)
{
  assert(chol != NULL);
  assert(bs > 0);

  id = ti;
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(rng, seed);
  numfactors = chol->size1;
  ndf = mc.ndf;
  time0 = mc.time0;
  timeT = mc.timeT;
  antithetic = mc.antithetic;
  assetsize = mc.assetsize;
  numsegments = mc.numsegments;
  blocksize = bs;

  assert(antithetic?(blocksize%2!=0?false:true):true);

  losses.resize(numsegments*blocksize, 0.0);
  indexes.resize(blocksize);

  lhs_num = 0;
  lhs_size = mc.lhs_size;
  lhs_pos = lhs_size;
  lhs_values_z.resize(lhs_size*numfactors);
  lhs_values_s.resize(lhs_size);
  if (lhs_size > 1 && numfactors > 1) {
    lhs_aux.resize(lhs_size);
  }

  rngpool.resize(GAUSSIAN_POOL_SIZE);
  rngpool_pos = GAUSSIAN_POOL_SIZE;

  x.resize(blocksize/(antithetic?2:1));
  z.resize((blocksize*numfactors)/(antithetic?2:1));
  s.resize(blocksize/(antithetic?2:1));
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::SimulationThread::~SimulationThread()
{
  if (rng != NULL) gsl_rng_free(rng);
}

//===========================================================================
// thread method
//===========================================================================
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
    for(size_t iobligor=0; iobligor<obligors.size(); iobligor++)
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

      double val = NAN;

      // simulating obligor loss
      for(size_t j=0; j<blocksize; j++)
      {
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

        assert(!isnan(val));
        int r = obligors[iobligor].irating;
        double days = inverses.evalue(r, val);
        Date ddate = time0 + (long)ceil(days);

        if (ddate <= timeT)
        {
          simuleObligorLoss(obligors[iobligor], ddate, (double*) &(losses[j*numsegments]));
        }
      }
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

//===========================================================================
// rlatent
// fills z and s arrays
//===========================================================================
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

//===========================================================================
// comparator used to obtain rank
//===========================================================================
inline bool ccruncher::SimulationThread::pcomparator(const pair<double,size_t> &o1, const pair<double,size_t> &o2)
{
  return o1.first < o2.first;
}

//===========================================================================
// generate lhs_size random chi-square values
//===========================================================================
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
        assert(!isnan(lhs_values_s[n]) && !isinf(lhs_values_s[n]));
      }
      std::random_shuffle(lhs_values_s.begin(), lhs_values_s.end(), f_rand);
    }
  }
}

//===========================================================================
// generate lhs_size random factors
//===========================================================================
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
    auxz.block = NULL;
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
          assert(!isnan(LHS_VALUES_Z(n,i)) && !isinf(LHS_VALUES_Z(n,i)));
        }
      }
    }
  }
}

//===========================================================================
// simule obligor losses at dtime and adds it to ptr_losses
//===========================================================================
void ccruncher::SimulationThread::simuleObligorLoss(const SimulatedObligor &obligor, Date dtime, double *ptr_losses) const throw()
{
  assert(ptr_losses != NULL);

  double obligor_lgd = NAN;
  char *p = static_cast<char*>(obligor.ref.assets);

  for(unsigned short i=0; i<obligor.numassets; i++)
  {
    SimulatedAsset *asset = reinterpret_cast<SimulatedAsset*>(p+i*assetsize);

    // evalue asset loss
    if (dtime <= asset->maxdate && asset->mindate <= dtime)
    {
      DateValues *values = lower_bound(asset->begin, asset->end, dtime);
      assert(dtime <= (asset->end-1)->date);
      double ead = values->ead.getValue(rng);
      double lgd = values->lgd.getValue(rng);

      // non-lgd means that is inherited from obligor
      if (isnan(lgd))
      {
        if (isnan(obligor_lgd))
        {
          obligor_lgd = obligor.lgd.getValue(rng);
        }
        lgd = obligor_lgd;
      }

      // compute asset loss
      double loss = ead * lgd;

      // aggregate asset loss in the correspondent segment loss
      // remember: obligor segments was recoded to assets segments
      unsigned short *segments = &(asset->segments);
      double *closses = ptr_losses;
      for(size_t j=0; j<numSegmentsBySegmentation.size(); j++)
      {
        unsigned short isegment = segments[j];
        assert(isegment < numSegmentsBySegmentation[j]);
        closses[isegment] += loss;
        closses += numSegmentsBySegmentation[j];
      }
    }
  }
}

//===========================================================================
// returns elapsed time creating random numbers
//===========================================================================
double ccruncher::SimulationThread::getElapsedTime1()
{
  return timer1.read();
}

//===========================================================================
// returns elapsed time simulating default times
//===========================================================================
double ccruncher::SimulationThread::getElapsedTime2()
{
  return timer2.read();
}

//===========================================================================
// returns elapsed time writing to disk
//===========================================================================
double ccruncher::SimulationThread::getElapsedTime3()
{
  return timer3.read();
}

//===========================================================================
// returns a uniform gaussian variate
// we use the ziggurat algorithm because is very fast but only if it is
// called consecutively
//===========================================================================
inline double ccruncher::SimulationThread::rnorm()
{
  rngpool_pos++;
  if (rngpool_pos >= GAUSSIAN_POOL_SIZE) fillGaussianPool();
  return rngpool[rngpool_pos];
}

//===========================================================================
// fills gaussian variates pool
//===========================================================================
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

