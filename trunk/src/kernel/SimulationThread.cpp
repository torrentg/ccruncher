
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

#include "kernel/SimulationThread.hpp"
#include <cassert>

// --------------------------------------------------------------------------

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulationThread::SimulationThread(MonteCarlo &mc, Copula *cop_) : Thread(), 
  montecarlo(mc), obligors(mc.obligors), assets(mc.assets), copula(cop_), 
  losses(0), uvalues(NULL)
{
  assert(copula != NULL);
  rng = copula->getRng();
  assetsize = mc.assetsize;
  numassets = mc.numassets;
  time0 = montecarlo.time0;
  timeT = montecarlo.timeT;
  survival = montecarlo.survival;
  antithetic = montecarlo.antithetic;
  reversed = montecarlo.reversed;
  uvalues = copula->get();
  numsegmentations = montecarlo.segmentations->size();
  losses.resize(numsegmentations);
  for(int i=0; i<numsegmentations; i++)
  {
    losses[i] = vector<double>(mc.segmentations->getSegmentation(i).size());
  }
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::SimulationThread::~SimulationThread()
{
  // nothing to do
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
    // generating random numbers
    timer1.resume();
    randomize();
    timer1.stop();

    // initialize aggregated values
    for(int i=0; i<numsegmentations; i++)
    {
      for(int j=0; j<(int)losses[i].size(); j++)
      {
        losses[i][j] = 0.0;
      }
    }

    // simulating default times & evalue losses & aggregate
    timer2.resume();
    for(size_t i=0; i<obligors.size(); i++)
    {
      simule(i);
    }
    timer2.stop();

    // data transfer
    more = montecarlo.append(losses, uvalues);
  }
}

//===========================================================================
// generate random numbers
//===========================================================================
void ccruncher::SimulationThread::randomize() throw()
{
  // generate a new realization for each copula
  if (!antithetic)
  {
    copula->next();
  }
  else // antithetic == true
  {
    if (reversed)
    {
      copula->next();
      reversed = false;
    }
    else
    {
      reversed = true;
    }
  }
}

//===========================================================================
// getRandom. Returns requested copula value
// encapsules antithetic management
//===========================================================================
inline double ccruncher::SimulationThread::getRandom(int iobligor) throw()
{
  if (antithetic && reversed)
  {
    return 1.0 - uvalues[iobligor];
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
  double u = getRandom(iobligor);
  int r = obligors[iobligor].irating;
  double t = survival.inverse(r, u);
  // TODO: assumed no leap years (this can be improved)
  Date dtime = time0 + (long)(t*365.0/12.0);
  
  // evalue obligor losses
  double obligor_recovery = NAN;
  char *p = (char*)(obligors[iobligor].assets);
  for(int i=0; i<obligors[iobligor].numassets; i++)
  {
    // evalue asset loss
    double loss = 0.0;
    SimulatedAsset *asset = (SimulatedAsset*)(p+i*assetsize);
    int *segments = &(asset->segments);

    if (dtime <= timeT && asset->mindate <= dtime && dtime <= asset->maxdate) // time0 <= t by-design
    {
      // not called Asset::getValues() due to memory access latency
      const DateValues &values = *(lower_bound(asset->begin, asset->end, DateValues(dtime)));
      assert(dtime <= (asset->end-1)->date);
      double recovery = values.recovery.getValue(rng);
      double exposure = values.exposure.getValue(rng);

      // non-recovery means that is inherited from obligor
      if (isnan(recovery))
      {
        if (isnan(obligor_recovery))
        {
          obligor_recovery = obligors[iobligor].ref->recovery.getValue(rng);
        }
        recovery = obligor_recovery;
      }
      
      loss = exposure * (1.0 - recovery);
    }

    // aggregate asset loss
    for(int j=0; j<numsegmentations; j++)
    {
      int isegment = segments[j];
      assert(0 <= isegment && isegment < (int) losses[j].size());
      losses[j][isegment] += loss;
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

