
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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
#include "utils/Arrays.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define ISEGMENTS(i,j) (isegments[(j)+(i)*numassets])

//===========================================================================
// constructor
//===========================================================================
ccruncher::SimulationThread::SimulationThread(MonteCarlo &mc, Copula *cop_) : 
  Thread(), montecarlo(mc), copula(cop_), dtimes(0), alosses(0), isegments(0)
{
  borrowers = montecarlo.borrowers;
  dtimes = vector<Date>(borrowers.size(), NAD);
  assets = montecarlo.assets;
  alosses = vector<double>(assets.size(), NAN);
  numassets = assets.size();
  time0 = montecarlo.time0;
  timeT = montecarlo.timeT;
  survival = montecarlo.survival;
  antithetic = montecarlo.antithetic;
  reversed = montecarlo.reversed;
  numsegmentations = montecarlo.segmentations->size();
  losses.resize(numsegmentations);
  isegments = vector<int>(numassets*numsegmentations, 0);
  for(int isegmentation=0; isegmentation<numsegmentations; isegmentation++)
  {
    losses[isegmentation] = vector<double>(montecarlo.segmentations->getSegmentation(isegmentation).size(), 0.0);
    
    for(int i=0; i<numassets; i++)
    {
	  ISEGMENTS(isegmentation,i) = assets[i].ref->getSegment(isegmentation);
    }
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
  timer3.reset();
  
  while(more)
  {
    // generating random numbers
    timer1.resume();
    randomize();
    timer1.stop();

    // simulating default times
    timer2.resume();
    simulate();
    timer2.stop();

    // portfolio evaluation
    timer3.resume();
    evalue();
    aggregate();
    timer3.stop();

    // data transfer
    more = transfer();
  }
}

//===========================================================================
// generate random numbers
//===========================================================================
void ccruncher::SimulationThread::randomize()
{
  // generate a new realization for each copula
  if (!antithetic)
  {
    copula->next();
  }
  else // antithetic == true
  {
    if (!reversed)
    {
      copula->next();
      reversed = true;
    }
    else
    {
      reversed = false;
    }
  }
}

//===========================================================================
// simulate time-to-default for each borrower
//===========================================================================
void ccruncher::SimulationThread::simulate()
{
  for (unsigned int i=0; i<borrowers.size(); i++) 
  {
    dtimes[i] = simTimeToDefault(getRandom(i), borrowers[i].irating);
  }
}

//===========================================================================
// getRandom. Returns requested copula value
// encapsules antithetic management
//===========================================================================
double ccruncher::SimulationThread::getRandom(int iborrower)
{
  if (antithetic)
  {
    if (reversed)
    {
      return 1.0 - copula->get(iborrower);
    }
    else
    {
      return copula->get(iborrower);
    }
  }
  else
  {
    return copula->get(iborrower);
  }
}

//===========================================================================
// simule time to default
//===========================================================================
Date ccruncher::SimulationThread::simTimeToDefault(double u, int r)
{
  // simulate time where this borrower defaults (in months)
  double t = survival.inverse(r, u);

  // return simulated default date
  // TODO: assumed no leap years (this can be improved)
  return time0 + (long)(t*365.0/12.0);
}

//===========================================================================
// portfolio evaluation using simulated default times
//===========================================================================
void ccruncher::SimulationThread::evalue()
{
  for(int i=0; i<numassets; i++)
  {
    Date t = dtimes[assets[i].iborrower];
    
    if (assets[i].mindate <= t && t <= assets[i].maxdate)
    {
      alosses[i] = assets[i].ref->getLoss(t);
    }
    else
    {
      alosses[i] = 0.0;
    }
  }
}

//===========================================================================
// aggregate simulated losses
//===========================================================================
void ccruncher::SimulationThread::aggregate()
{
  for(int isegmentation=0; isegmentation<numsegmentations; isegmentation++)
  {
    for(unsigned int i=0; i<losses[isegmentation].size(); i++)
    {
	  losses[isegmentation][i] = 0.0;
    }

    for(int i=0; i<numassets; i++)
    {
      int isegment = ISEGMENTS(isegmentation,i);
	  losses[isegmentation][isegment] += alosses[i];
    }
  }
}

//===========================================================================
// transfer simulated data to master
//===========================================================================
bool ccruncher::SimulationThread::transfer()
{
  return montecarlo.append(losses, copula->get());
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

//===========================================================================
// returns ellapsed time evaluating portfolio
//===========================================================================
double ccruncher::SimulationThread::getEllapsedTime3()
{
  return timer3.read();
}

