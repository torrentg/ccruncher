
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

#define LHS_VALUES_Z(i,j) lhs_values_z[(i)*numfactors+(j)]

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulationThread::SimulationThread(int ti, MonteCarlo &mc, unsigned long seed) :
  Thread(), id(ti), montecarlo(mc), obligors(mc.obligors), assets(mc.assets),
  segments(mc.segments), numsegments(mc.numsegments), rng(NULL), chol(mc.chol),
  floadings1(mc.floadings1), floadings2(mc.floadings2), inverses(mc.inverses),
  losses(0)
{
  assert(chol != NULL);
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(rng, seed);
  numfactors = chol->size1;
  ndf = mc.ndf;
  time0 = mc.time0;
  timeT = mc.timeT;
  antithetic = mc.antithetic;
  reversed = true;

  int num = 0;
  for(size_t i=0; i<numsegments.size(); i++) num += numsegments[i];
  losses.resize(num, 0.0);

  lhs_num = 0;
  lhs_size = mc.lhs_size;
  lhs_pos = lhs_size; // to force an initial sample
  lhs_values_z.resize(lhs_size*numfactors);
  if (ndf > 0.0) {
    lhs_values_s.resize(lhs_size);
  }
  if (lhs_size > 1 && numfactors > 1) {
    lhs_aux.resize(lhs_size);
  }
  xvalues.resize(obligors.size());
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
  reversed = (antithetic?true:false);
  
  while(more)
  {
    // initialize aggregated values
    for(vector<double>::iterator it=losses.begin(); it!=losses.end(); ++it)
    {
      *it = 0.0;
    }

    // generating random numbers
    timer1.resume();
    randomize();
    timer1.stop();

    // simulating default times & evalue losses & aggregate
    timer2.resume();
    for(size_t i=0; i<obligors.size(); i++)
    {
      simule(i);
    }
    timer2.stop();

    // data transfer
    more = montecarlo.append(id, lhs_num, reversed, losses);
  }
}

//===========================================================================
// simulate the multivariate distribution (normal o t-student)
//===========================================================================
void ccruncher::SimulationThread::rmvdist()
{
  // latin hypercube sample management
  if (lhs_pos+1 >= lhs_size)
  {
    lhs_num++;
    lhs_pos = 0;
    rchisq();
    rfactors();
  }
  else
  {
    lhs_pos++;
  }

  // simulate multivariate normal
  double *wz = (double *) &(LHS_VALUES_Z(lhs_pos,0));
  for(size_t i=0; i<obligors.size(); i++)
  {
    size_t ifactor = obligors[i].ifactor;
    xvalues[i] = wz[ifactor] + floadings2[ifactor]*gsl_ran_ugaussian(rng);
  }

  // simulate multivariate t-student
  if (ndf > 0.0)
  {
    double chisq = lhs_values_s[lhs_pos];
    if (chisq < 1e-14) chisq = 1e-14; //avoid division by 0
    double chival = sqrt(ndf/chisq);
    for(size_t i=0; i<obligors.size(); i++) {
      xvalues[i] *= chival;
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
      // see 'A user's guide to LHS: Sandia's Latin Hypercube Sampling Software'
      // by Gregory D. Wyss, Kelly H. Jorgensen
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
void ccruncher::SimulationThread::rfactors()
{
  if (numfactors == 1 && lhs_size > 1)
  {
    // randomizer wrapper
    frand f_rand(rng);

    // Latin Hypercube Sampling (1-factor variable)
    // see 'A user's guide to LHS: Sandia's Latin Hypercube Sampling Software'
    // by Gregory D. Wyss, Kelly H. Jorgensen
    for(size_t n=0; n<lhs_size; n++)
    {
      double u = gsl_ran_flat(rng, double(n)/double(lhs_size), double(n+1)/double(lhs_size));
      lhs_values_z[n] = gsl_cdf_ugaussian_Pinv(u);
    }
    std::random_shuffle(lhs_values_z.begin(), lhs_values_z.end(), f_rand);
  }
  else
  {
    gsl_vector z;
    z.size = numfactors;
    z.stride = 1;
    z.data = NULL;
    z.block = NULL;
    z.owner = 0;

    // creating sample of size lhs_size
    double *z_ptr = (double *) &(lhs_values_z[0]);
    for(size_t n=0; n<lhs_size; n++)
    {
      // simulating N(0,R)
      for(size_t i=0; i<numfactors; i++) {
        z_ptr[i] = gsl_ran_ugaussian(rng);
      }
      z.data = z_ptr;
      gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, chol, &z);
      z_ptr += numfactors;
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
// generate random numbers
//===========================================================================
void ccruncher::SimulationThread::randomize() throw()
{
  if (!antithetic)
  {
    rmvdist();
  }
  else // antithetic == true
  {
    if (reversed)
    {
      rmvdist();
      reversed = false;
    }
    else
    {
      reversed = true;
    }
  }
}

//===========================================================================
// getRandom. Returns i-th component of the simulated multivariate normal
// encapsules antithetic management
//===========================================================================
inline double ccruncher::SimulationThread::getRandom(int iobligor) throw()
{
  if (antithetic && reversed)
  {
    return -xvalues[iobligor];
  }
  else
  {
    return +xvalues[iobligor];
  }
}

//===========================================================================
// simule iobligor
//===========================================================================
void ccruncher::SimulationThread::simule(int iobligor) throw()
{
  // simule default time
  double x = getRandom(iobligor);
  int r = obligors[iobligor].irating;
  double days = inverses.evalue(r, x);
  Date dtime = time0 + (long)ceil(days);
  if (timeT < dtime) return;

  // evalue obligor losses
  double obligor_recovery = NAN;
  for(unsigned short i=0; i<obligors[iobligor].numassets; i++)
  {
    SimulatedAsset *asset = obligors[iobligor].ref.assets + i;

    // evalue asset loss
    if (dtime <= asset->maxdate && asset->mindate <= dtime)
    {
      const DateValues &values = *(lower_bound(asset->begin, asset->end, DateValues(dtime)));
      assert(dtime <= (asset->end-1)->date);
      double recovery = values.recovery.getValue(rng);
      double exposure = values.exposure.getValue(rng);

      // non-recovery means that is inherited from obligor
      if (isnan(recovery))
      {
        if (isnan(obligor_recovery))
        {
          obligor_recovery = obligors[iobligor].recovery.getValue(rng);
        }
        recovery = obligor_recovery;
      }

      // compute asset loss
      double loss = exposure * (1.0 - recovery);

      // aggregate asset loss to each segmentation
      // in the correspondent segment
      size_t numsegmentations = numsegments.size();
      const unsigned short *ptr_segments = static_cast<const unsigned short*>(&(segments[iobligor*numsegmentations]));
      double *ptr_losses = static_cast<double*>(&(losses[0]));
      for(size_t j=0; j<numsegmentations; j++)
      {
        unsigned short isegment = ptr_segments[j];
        assert(isegment < numsegments[j]);
        ptr_losses[isegment] += loss;
        ptr_losses += numsegments[j];
      }
    }
  }
}

//===========================================================================
// returns ellapsed time creating random numbers
//===========================================================================
double ccruncher::SimulationThread::getEllapsedTime1()
{
  return timer1.read();
}

//===========================================================================
// returns ellapsed time simulating default times
//===========================================================================
double ccruncher::SimulationThread::getEllapsedTime2()
{
  return timer2.read();
}

