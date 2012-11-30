
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

#include <gsl/gsl_blas.h>
#include "kernel/SimulationThread.hpp"
#include <cassert>

// --------------------------------------------------------------------------

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulationThread::SimulationThread(MonteCarlo &mc, unsigned long seed) : Thread(),
  montecarlo(mc), obligors(mc.obligors), assets(mc.assets), rng(NULL), factors(NULL),
  chol(mc.chol), floadings(mc.floadings), inverses(mc.inverses), losses(0), uvalues(0)
{
  assert(chol != NULL);
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(rng, seed);
  factors = gsl_vector_alloc(chol->size1);
  assetsize = mc.assetsize;
  ndf = mc.ndf;
  time0 = mc.time0;
  timeT = mc.timeT;
  antithetic = mc.antithetic;
  reversed = true;
  uvalues.resize(obligors.size());
  numsegmentations = mc.aggregators.size();
  losses.resize(numsegmentations);
  for(int i=0; i<numsegmentations; i++)
  {
    losses[i] = vector<double>(mc.aggregators[i]->size());
  }
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::SimulationThread::~SimulationThread()
{
  if (rng != NULL) gsl_rng_free(rng);
  if (factors != NULL) gsl_vector_free(factors);
}

//===========================================================================
// thread method
//===========================================================================
void ccruncher::SimulationThread::run()
{
  bool more = true;
  timer1.reset();
  timer2.reset();
  
  while(more)
  {
    // initialize aggregated values
    for(int i=0; i<numsegmentations; i++)
    {
      for(int j=losses[i].size()-1; j>=0; j--)
      {
        losses[i][j] = 0.0;
      }
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
    more = montecarlo.append(losses);
  }
}

//===========================================================================
// simulate a multivariate normal and let result in values
//===========================================================================
void ccruncher::SimulationThread::rmvnorm()
{
  // simulate w·N(0,R)
  for(size_t i=0; i<factors->size; i++) {
    gsl_vector_set(factors, i, gsl_ran_ugaussian(rng));
  }
  gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, chol, factors);

  if (ndf <= 0.0)
  {
    // simulate multivariate normal
    for(size_t i=0; i<obligors.size(); i++)
    {
      size_t ifactor = obligors[i].ifactor;
      uvalues[i] = gsl_vector_get(factors, ifactor) + floadings[ifactor]*gsl_ran_ugaussian(rng);
    }
  }
  else
  {
    // simulate the chi-square value
    double chisq =  gsl_ran_chisq(rng, ndf);
    if (chisq < 1e-14) chisq = 1e-14; //avoid division by 0
    double chival = sqrt(ndf/chisq);

    // simulate multivariate t-Student
    for(size_t i=0; i<obligors.size(); i++)
    {
      size_t ifactor = obligors[i].ifactor;
      uvalues[i] = chival * (gsl_vector_get(factors, ifactor) + floadings[ifactor]*gsl_ran_ugaussian(rng));
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
    rmvnorm();
  }
  else // antithetic == true
  {
    if (reversed)
    {
      rmvnorm();
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
    return -uvalues[iobligor];
  }
  else
  {
    return uvalues[iobligor];
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
  Date dtime = inverses.evalueAsDate(r, x);
  if (timeT < dtime) return;

  // evalue obligor losses
  double obligor_recovery = NAN;
  char *p = (char*)(obligors[iobligor].ref.assets);
  for(unsigned short i=0; i<obligors[iobligor].numassets; i++)
  {
    SimulatedAsset *asset = (SimulatedAsset*)(p);
    p += assetsize;

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

      // aggregate asset loss
      unsigned short *segments = &(asset->segments);
      for(int j=0; j<numsegmentations; j++)
      {
        unsigned short isegment = segments[j];
        assert(isegment < losses[j].size());
        losses[j][isegment] += loss;
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

