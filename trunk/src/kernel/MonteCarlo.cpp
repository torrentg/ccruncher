
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

#include <cmath>
#include <cfloat>
#include <algorithm>
#include "kernel/MonteCarlo.hpp"
#include "segmentations/Segmentations.hpp"
#include "math/BlockGaussianCopula.hpp"
#include "math/BlockTStudentCopula.hpp"
#include "utils/Utils.hpp"
#include "utils/Arrays.hpp"
#include "utils/Timer.hpp"
#include "utils/Logger.hpp"
#include "utils/Format.hpp"
#include "utils/File.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::MonteCarlo::MonteCarlo() : borrowers(), assets(), aggregators()
{
  maxseconds = 0L;
  maxiterations = 0L;
  antithetic = false;
  reversed = false;
  hash = 0;
  fpath = "path not set";
  bforce = false;
  copula = NULL;
  blcopulas = false;
  bldeftime = false;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::MonteCarlo::~MonteCarlo()
{
  release();
}

//===========================================================================
// release
//===========================================================================
void ccruncher::MonteCarlo::release()
{
  // deallocating copula
  if (copula != NULL) { 
    delete copula; 
    copula = NULL; 
  }

  // dropping aggregators elements
  for(unsigned int i=0;i<aggregators.size();i++) {
    if (aggregators[i] != NULL) {
      delete aggregators[i];
      aggregators[i] = NULL;
    }
  }
  aggregators.clear();

  // closing files
  if (blcopulas) fcopulas.close();
  if (bldeftime) fdeftime.close();
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::MonteCarlo::initialize(IData &idata) throw(Exception)
{
  if (maxiterations != 0L)
  {
    throw Exception("Monte Carlo reinitialization not allowed");
  }
  else if (fpath == "path not set")
  {
    throw Exception("error initializing Monte Carlo: output files path not established");
  }

  try
  {
    Logger::addBlankLine();
    Logger::trace("initialing procedure", '*');
    Logger::newIndentLevel();

    // initializing parameters
    initParams(idata);

    // initializing borrowers
    initBorrowers(idata);

    // initializing assets
    initAssets(idata);

    // initializing survival functions
    initSurvival(idata);

    // initializing copula
    initCopula(idata, idata.getParams().copula_seed);

    // initializing aggregators
    initAggregators(idata);

    // initializes debug bulk files 
    initAdditionalOutput();

    // exit function
    Logger::previousIndentLevel();
  }
  catch(Exception &e)
  {
    release();
    throw Exception(e, "error initializing Monte Carlo");
  }
}

//===========================================================================
// initParams
//===========================================================================
void ccruncher::MonteCarlo::initParams(IData &idata) throw(Exception)
{
  // setting logger header
  Logger::trace("setting parameters", '-');
  Logger::newIndentLevel();

  // max number of seconds
  maxseconds = idata.getParams().maxseconds;
  Logger::trace("maximum execution time (in seconds)", Format::long2string(maxseconds));

  // max number of iterations
  maxiterations = idata.getParams().maxiterations;
  Logger::trace("maximum number of iterations", Format::long2string(maxiterations));

  // printing initial date
  time0 = idata.getParams().time0;
  Logger::trace("initial date", Format::date2string(time0));
  // printing end date
  timeT = idata.getParams().timeT;
  Logger::trace("end date", Format::date2string(timeT));

  // fixing variance reduction method
  antithetic = idata.getParams().antithetic;
  Logger::trace("antithetic mode", Format::bool2string(antithetic));
  reversed = false;

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initBorrowers
//===========================================================================
void ccruncher::MonteCarlo::initBorrowers(IData &idata) throw(Exception)
{
  // doing assertions
  assert(borrowers.size() == 0);

  // setting logger header
  Logger::trace("setting borrowers to simulate", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("simulate only active borrowers", Format::bool2string(idata.getParams().onlyactive));
  Logger::trace("number of initial borrowers", Format::long2string(idata.getPortfolio().getBorrowers().size()));

  // determining the borrowers to simulate
  bool onlyactive = idata.getParams().onlyactive;
  vector<Borrower *> &vborrowers = idata.getPortfolio().getBorrowers();
  borrowers.reserve(vborrowers.size());
  for(unsigned int i=0; i<vborrowers.size(); i++)
  {
    if (onlyactive)
    {
      if (vborrowers[i]->isActive(time0, timeT)) 
      {
        borrowers.push_back(SimulatedBorrower(vborrowers[i]));
      }
    }
    else
    {
      borrowers.push_back(SimulatedBorrower(vborrowers[i]));
    }
  }
  
  // important: sorting borrowers list by sector and rating
  sort(borrowers.begin(), borrowers.end());
  Logger::trace("number of simulated borrowers", Format::long2string((long)borrowers.size()));

  // checking that exist borrowers to simulate
  if (borrowers.size() == 0)
  {
    throw Exception("error initializing borrowers: 0 borrowers to simulate");
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initAssets
//===========================================================================
void ccruncher::MonteCarlo::initAssets(IData &idata) throw(Exception)
{
  // doing assertions
  assert(assets.size() == 0);

  // setting logger header
  Logger::trace("setting assets to simulate", '-');
  Logger::newIndentLevel();

  // note: this is the place where it must have the asset losses precomputation.
  // Asset losses has been moved to Borrower:insertAsset() with the purpose of 
  // being able flush memory on asset events just after precomputation because 
  // in massive portfolios memory can be exhausted

  // determining the assets to simulate
  long numassets = 0L;
  assets.reserve(4*borrowers.size());
  for(unsigned int i=0; i<borrowers.size(); i++)
  {
    vector<Asset*> &vassets = borrowers[i].ref->getAssets();
    for(unsigned int j=0; j<vassets.size(); j++)
    {
      numassets++;
      if (vassets[j]->isActive(time0, timeT)) 
      {
        assets.push_back(SimulatedAsset(vassets[j], i));
      }
    }
  }
  Logger::trace("number of initial assets", Format::long2string(numassets));
  Logger::trace("number of simulated assets", Format::long2string((long)assets.size()));

  // checking that exist assets to simulate
  if (assets.size() == 0)
  {
    throw Exception("error initializing assets: 0 assets to simulate");
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// initSurvival
//===========================================================================
void ccruncher::MonteCarlo::initSurvival(IData &idata) throw(Exception)
{
  // doing assertions
  assert(survival.size() == 0);

  // setting logger header
  Logger::trace("setting survival function", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of ratings", Format::int2string(idata.getRatings().size()));

  if (idata.hasSurvival())
  {
    // setting survival function
    survival = idata.getSurvival();
    Logger::trace("survival function", string("user defined"));

    // checking that survival function is defined for t <= timeT
    int months = idata.getSurvival().getMinCommonTime();
    Date aux = addMonths(time0, months);
    if (aux < timeT)
    {
      throw Exception("survival function not defined at t=" + Format::date2string(timeT));
    }
  }
  else
  {
    // setting logger info
    string sval = Format::int2string(idata.getTransitionMatrix().size());
    Logger::trace("transition matrix dimension", sval + "x" + sval);
    Logger::trace("transition matrix period (in months)", Format::int2string(idata.getTransitionMatrix().getPeriod()));

    // computing survival function using transition matrix
    int months = (int) ceil(time0.getMonthsTo(timeT));
    survival = idata.getTransitionMatrix().getSurvival(1, months+1);
    Logger::trace("transition matrix -> survival function", string("computed"));
    TransitionMatrix tm1 = idata.getTransitionMatrix().scale(1);
    double rerror = tm1.getRegularizationError();
    Logger::trace("regularization error", Format::double2string(rerror));
  }

  // exit function
  Logger::previousIndentLevel();
}

//===========================================================================
// copula construction
//===========================================================================
void ccruncher::MonteCarlo::initCopula(IData &idata, long seed) throw(Exception)
{
  Timer timer(true);

  // doing assertions
  assert(copula == NULL);

  // setting logger header
  Logger::trace("initializing copula", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("number of sectors", Format::int2string(idata.getSectors().size()));

  // setting logger info
  Logger::trace("copula type", idata.getParams().copula_type);
  Logger::trace("copula dimension", Format::long2string(borrowers.size()));
  Logger::trace("seed used to initialize randomizer (0=none)", Format::long2string(seed));

  try
  {
    vector<int> tmp(idata.getCorrelationMatrix().size(),0);

    // computing the number of borrowers in each sector
    for(unsigned int i=0; i<borrowers.size(); i++)
    {
      tmp[borrowers[i].ref->isector]++;
    }

    // creating the copula object
    if (idata.getParams().getCopulaType() == "gaussian")
    {
      copula = new BlockGaussianCopula(idata.getCorrelationMatrix().getMatrix(), &tmp[0], idata.getCorrelationMatrix().size());
      double cnum = ((BlockGaussianCopula*)copula)->getConditionNumber();
      Logger::trace("cholesky condition number", Format::double2string(cnum));
    }
    else if (idata.getParams().getCopulaType() == "t")
    {
      double ndf = idata.getParams().getCopulaParam();
      copula = new BlockTStudentCopula(idata.getCorrelationMatrix().getMatrix(), &tmp[0], idata.getCorrelationMatrix().size(), ndf);
      double cnum = ((BlockTStudentCopula*)copula)->getConditionNumber();
      Logger::trace("cholesky condition number", Format::double2string(cnum));
    }
    else 
    {
      throw Exception("invalid copula type");
    }
  }
  catch(std::exception &e)
  {
    // copula deallocated by release method
    throw Exception(e, "error ocurred while initializing copula");
  }

  // if no seed is given /dev/urandom or time() will be used
  if (seed == 0L)
  {
    // use a seed based on clock
    seed = Utils::trand();
  }

  // seeding random number generators
  //TODO: threads requires distinct seed
  copula->setSeed(seed);

  // exit function
  Logger::trace("elapsed time initializing copula", timer);
  Logger::previousIndentLevel();
}

//===========================================================================
// initAggregators
//===========================================================================
void ccruncher::MonteCarlo::initAggregators(IData &idata) throw(Exception)
{
  Timer timer(true);
  segmentations = &(idata.getSegmentations());

  // assertions
  assert(fpath != "" && fpath != "path not set"); 
  assert(aggregators.size() == 0); 

  // setting logger header
  Logger::trace("initializing aggregators", '-');
  Logger::newIndentLevel();

  // setting logger info
  Logger::trace("output data directory", fpath);
  Logger::trace("number of segmentations", Format::int2string(segmentations->size()));

  // allocating and initializing aggregators
  aggregators.clear();
  for(int i=0;i<segmentations->size();i++)
  {
    string filename = File::normalizePath(fpath) + segmentations->getSegmentation(i).name + ".csv";
    Aggregator *aggregator = new Aggregator(i, segmentations->getSegmentation(i), assets);
    aggregator->setOutputProperties(filename, bforce);
    aggregators.push_back(aggregator);
  }

  // exit function
  Logger::trace("elapsed time initializing aggregators", timer);
  Logger::previousIndentLevel();
}

//===========================================================================
// execute
//===========================================================================
long ccruncher::MonteCarlo::execute() throw(Exception)
{
  long numiterations;

  // assertions
  assert(fpath != "" && fpath != "path not set"); 

  // setting logger header
  Logger::addBlankLine();
  Logger::trace("running Monte Carlo" + (hash==0?"": " [" + Format::int2string(hash) + " simulations per hash]"), '*');
  Logger::newIndentLevel();

  //TODO: implement threads here
  numiterations = executeWorker();

  // exit function
  Logger::addBlankLine();
  Logger::previousIndentLevel();
  return numiterations;
}

//===========================================================================
// executeWorker
//===========================================================================
long ccruncher::MonteCarlo::executeWorker() throw(Exception)
{
  long numiterations = 0L;
  bool aux=false, moreiterations=true;
  Timer timer(true), timer1, timer2, timer3, timer4;

  try
  {
    while(moreiterations)
    {
      // generating random numbers
      timer1.resume();
      randomize();
      timer1.stop();

      // simulating default time for each borrower
      timer2.resume();
      simulate();
      timer2.stop();

      // portfolio evaluation
      timer3.resume();
      evalue();
      timer3.stop();

      // aggregation of values
      timer4.resume();
      aux = aggregate();
      timer4.stop();

      // counter increment
      numiterations++;

      // printing hashes
      if (hash > 0 && numiterations%hash == 0)
      {
        Logger::append(".");
      }

      // checking stop criteria
      if (aux == false)
      {
        moreiterations = false;
      }

      // checking stop criteria
      if (maxiterations > 0L && numiterations >= maxiterations)
      {
        moreiterations = false;
      }

      // checking stop criteria
      if (maxseconds > 0L && timer.read() >  maxseconds)
      {
        moreiterations = false;
      }
    }

    // flushing aggregators
    for(unsigned int i=0;i<aggregators.size();i++) 
    {
      aggregators[i]->flush();
    }

    // printing traces
    Logger::trace("elapsed time creating random numbers", timer1);
    Logger::trace("elapsed time simulating default times", timer2);
    Logger::trace("elapsed time evaluating portfolio", timer3);
    Logger::trace("elapsed time aggregating values", timer4);
    Logger::trace("total simulation time", Timer::format(timer.read()));
  }
  catch(Exception &e)
  {
    throw Exception(e, "error ocurred while executing Monte Carlo");
  }

  // exit function
  return(numiterations);
}

//===========================================================================
// generate random numbers
//===========================================================================
void ccruncher::MonteCarlo::randomize()
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

  // trace copula values
  if (blcopulas) 
  {
    printCopulaValues();
  }
}

//===========================================================================
// simulate time-to-default for each borrower
// put result in rpaths[iborrower]
//===========================================================================
void ccruncher::MonteCarlo::simulate()
{
  // simulates default times
  for (unsigned int i=0; i<borrowers.size(); i++) 
  {
    borrowers[i].dtime = simTimeToDefault(i);
  }

  // trace default times
  if (bldeftime) 
  {
    printDefaultTimes();
  }
}

//===========================================================================
// getRandom. Returns requested copula value
// encapsules antithetic management
//===========================================================================
double ccruncher::MonteCarlo::getRandom(int iborrower)
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
// given a borrower, simule time to default
//===========================================================================
Date ccruncher::MonteCarlo::simTimeToDefault(int iborrower)
{
  // rating at t0 is initial rating
  int r = borrowers[iborrower].irating;

  // getting random number U[0,1] (correlated with rest of borrowers...)
  double u = getRandom(iborrower);

  // simulate time where this borrower defaults (in months)
  double t = survival.inverse(r, u);

  // return simulated default date
  // TODO: assumed no leap years (this can be improved)
  return time0 + (long)(t*365.0/12.0);
}

//===========================================================================
// portfolio evaluation using simulated default times
//===========================================================================
void ccruncher::MonteCarlo::evalue()
{
  for(unsigned int i=0; i<assets.size(); i++)
  {
    Date t = borrowers[assets[i].iborrower].dtime;
    
    if (assets[i].mindate <= t && t <= assets[i].maxdate)
    {
      assets[i].loss = assets[i].ref->getLoss(t);
    }
    else
    {
      assets[i].loss = 0.0;
    }
  }
}

//===========================================================================
// aggregate simulated losses
// return true=all ok, continue
// return false=stop montecarlo
//===========================================================================
bool ccruncher::MonteCarlo::aggregate() throw(Exception)
{
  bool ret=true;

  for(unsigned int i=0;i<aggregators.size();i++)
  {
    if (aggregators[i]->append(assets) == false)
    {
      ret = false;
    }
  }

  return ret;
}

//===========================================================================
// setHash
//===========================================================================
void ccruncher::MonteCarlo::setHash(int num)
{
  assert(num >= 0);
  hash = std::max(0, num);
}

//===========================================================================
// setFilePath
//===========================================================================
void ccruncher::MonteCarlo::setFilePath(string path, bool force)
{
  fpath = path;
  bforce = force;
}

//===========================================================================
// setAdditionalOutput
//===========================================================================
void ccruncher::MonteCarlo::setAdditionalOutput(bool copulas, bool deftimes)
{
  blcopulas = copulas;
  bldeftime = deftimes;
}

//===========================================================================
// initAdditionalOutput
//===========================================================================
void ccruncher::MonteCarlo::initAdditionalOutput() throw(Exception)
{
  assert(fpath != "" && fpath != "path not set"); 
  string dirpath = File::normalizePath(fpath);
  
  // initializes copula values file
  if (blcopulas)
  {
    string filename = dirpath + "copula.csv";
    try
    {
      fcopulas.open(filename.c_str(), ios::out|ios::trunc);
      for(unsigned int i=0; i<borrowers.size(); i++)
      {
        fcopulas << "\"" << borrowers[i].ref->name << "\"" << (i!=borrowers.size()-1?", ":"");
      }
      fcopulas << endl;
    }
    catch(...)
    {
      throw Exception("error writing file " + filename);
    }
  }

  // initializes simulated default times file
  if (bldeftime)
  {
    string filename = dirpath + "deftimes.csv";
    try
    {
      fdeftime.open(filename.c_str(), ios::out|ios::trunc);
      for(unsigned int i=0; i<borrowers.size(); i++)
      {
        fdeftime << "\"" << borrowers[i].ref->name << "\"" << (i!=borrowers.size()-1?", ":"");
      }
      fdeftime << endl;
    }
    catch(...)
    {
      throw Exception("error writing file " + filename);
    }
  }
}

//===========================================================================
// printCopulaValues
//===========================================================================
void ccruncher::MonteCarlo::printCopulaValues() throw(Exception)
{
  for(unsigned int i=0; i<borrowers.size(); i++) 
  {
    fcopulas << getRandom(i) << (i!=borrowers.size()-1?", ":"");
  }
  fcopulas << "\n";
}

//===========================================================================
// printDefaultTimes
//===========================================================================
void ccruncher::MonteCarlo::printDefaultTimes() throw(Exception)
{
  for(unsigned int i=0; i<borrowers.size(); i++) 
  {
    fdeftime << borrowers[i].dtime << (i!=borrowers.size()-1?", ":"");
  }
  fdeftime << "\n";
}

