
//***************************************************************************
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// Montecarlo.cpp - MonteCarlo code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#include <cfloat>
#include "MonteCarlo.hpp"
#include "utils/Utils.hpp"
#include "utils/Timer.hpp"

//***************************************************************************
// constructor
//***************************************************************************
ccruncher::MonteCarlo::MonteCarlo()
{
  // setting default values
  reset();
}

//***************************************************************************
// destructor
//***************************************************************************
ccruncher::MonteCarlo::~MonteCarlo()
{
  release();
}

//***************************************************************************
// reset
//***************************************************************************
void ccruncher::MonteCarlo::reset()
{
  MAXSECONDS = 0L;
  MAXITERATIONS = 0L;
  STEPS = 0;
  STEPLENGTH = 0;
  N = 0L;
  CONT = 0L;
  antithetic = false;
  reversed = false;

  verbosity = 0;
  usempi = false;
  hash = 0;
  fpath = "path not set";
  bforce = false;

  interests = NULL;
  ratings = NULL;
  sectors = NULL;
  segmentations = NULL;
  aggregators = NULL;
  clients = NULL;

  mtrans = NULL;
  cmatrix = NULL;
  copulas = NULL;
  dates = NULL;
  rpaths = NULL;
}

//***************************************************************************
// release
//***************************************************************************
void ccruncher::MonteCarlo::release()
{
  // deallocating transition matrix object
  if (mtrans != NULL) { delete mtrans; mtrans = NULL; }
  
  // client correlation matrix deallocated by copulas[0] object

  // deallocating copula vector
  if (copulas != NULL) 
  {
    for (int i=0;i<=STEPS;i++)
    {
      delete copulas[i]; 
      copulas[i] = NULL; 
    }
    delete [] copulas;
    copulas = NULL;
  }

  // deallocating dates vector
  if (dates != NULL) { delete [] dates; dates = NULL; }
  
  // deallocating rpaths matrix
  if (rpaths != NULL)
  {
    for (int i=0;i<N;i++)
    {
      delete [] rpaths[i]; 
      rpaths[i] = NULL; 
    }
    delete [] rpaths;
    rpaths = NULL;
  }
}

//***************************************************************************
// initialize
//***************************************************************************
void ccruncher::MonteCarlo::initialize(const IData *idata) throw(Exception)
{
  if (idata == NULL)
  {
    throw Exception("MonteCarlo::initialize(): NULL idata");
  }
  else if (MAXITERATIONS != 0L)
  {
    throw Exception("MonteCarlo::initialize(): reinitialization not allowed");
  }
  else if (fpath == "path not set")
  {
    throw Exception("MonteCarlo::initialize(): output files path not established");
  }

  try
  {
    init(idata);
  }
  catch(Exception &e)
  {
    release();
    throw Exception(e, "MonteCarlo::initialize()");
  }
}

//***************************************************************************
// init
//***************************************************************************
void ccruncher::MonteCarlo::init(const IData *idata) throw(Exception)
{
  // max number of seconds
  MAXSECONDS = idata->params->maxseconds;

  // max number of iterations
  MAXITERATIONS = idata->params->maxiterations;

  // fixing step number
  STEPS = idata->params->steps;

  // fixing steplength
  STEPLENGTH = idata->params->steplength;

  // fixing begin date
  begindate = idata->params->begindate;

  // determinem les dates de cada tall temporal
  dates = idata->params->getDates();

  // fixing number of clients
  if (idata->params->onlyactive) {
    idata->portfolio->sortClients(dates[0], dates[STEPS]);
    N = idata->portfolio->getNumActiveClients(dates[0], dates[STEPS]);
  } else {
    N = idata->portfolio->getClients()->size();
  }

  // fixing variance reduction method
  antithetic = idata->params->antithetic;

  // initializing internal variables  
  CONT = 0L;
  reversed = false;

  // filling objects
  ratings = idata->ratings;
  sectors = idata->sectors;
  segmentations = idata->segmentations;
  aggregators = idata->aggregators;
  clients = idata->portfolio->getClients();

  // trobem la matriu de transicio usada
  mtrans = translate(idata->transitions, (double)(STEPLENGTH)/12.0);

  // definim la matrix de correlacions entre clients
  cmatrix = initCorrelationMatrix(idata->correlations->getMatrix(), clients, N);

  // definim les copules a emprar
  copulas = initCopulas(cmatrix, N, STEPS, idata->params->copula_seed);

  // ratings paths allocation
  rpaths = initRatingsPaths(N, STEPS, clients);

  // inicialitzem els aggregators
  initAggregators();
}

//***************************************************************************
// construeix la matriu de correlacio entre clients
//***************************************************************************
double ** ccruncher::MonteCarlo::initCorrelationMatrix(double **sectorcorrels, 
                               vector<Client *> *vclients, int n) throw(Exception)
{
  double **ret = Utils::allocMatrix(n, n);

  // definim la matriu de correlacions entre clients
  for (int i=0;i<n;i++)
  {
    int isector1 = (*vclients)[i]->isector;

    for (int j=0;j<n;j++)
    {
      int isector2 = (*vclients)[j]->isector;

      if (i == j)
      {
        ret[i][j] = 1.0;
      }
      else
      {
        ret[i][j] = sectorcorrels[isector1][isector2];
      }
    }
  }
  
  return ret;
}

//***************************************************************************
// construeix les copules per cada tall temporal
//***************************************************************************
CopulaNormal** ccruncher::MonteCarlo::initCopulas(double **ccm, int n, int k, long seed) throw(Exception)
{
  CopulaNormal **ret = new CopulaNormal*[k+1];
  for (int i=0;i<=k;i++) ret[i] = NULL;

  // creem l'objecte per la generacio de copules normals
  ret[0] = new CopulaNormal(n, ccm);

  // el repliquem per tots els talls temporals
  for(int i=1;i<=k;i++)
  {
    ret[i] = new CopulaNormal(*(ret[0]));
  }

  // seeding random number generators
  // if no seed is given /dev/urandom or time() will be used
  if (seed != 0L)
  {
    for(int i=0;i<=k;i++)
    {
      ret[i]->setSeed(seed+(long)i);
    }
  }

  return ret;
}

//***************************************************************************
// initRatingsPaths
//***************************************************************************
int** ccruncher::MonteCarlo::initRatingsPaths(int n, int k, vector<Client *> *vclients)
{
  int **ret = new int*[n];
  
  for (int i=0;i<n;i++)
  {
    // allocating memory
    ret[i] = new int[k+1];
    
    // setting initial client rating
    ret[i][0] = (*vclients)[i]->irating;

    // setting a default rating
    for (int j=1;j<=k;j++)
    {
      ret[i][j] = 0;
    }
  }

  return ret;
}

//***************************************************************************
// initAggregators
//***************************************************************************
void ccruncher::MonteCarlo::initAggregators() throw(Exception)
{
  int indexdefault = mtrans->getIndexDefault();
  aggregators->initialize(segmentations, dates, STEPS+1, clients, N, indexdefault, ratings);
  aggregators->setOutputProperties(fpath, bforce, 0);
}

//***************************************************************************
// execute
//***************************************************************************
void ccruncher::MonteCarlo::execute() throw(Exception)
{
  bool moreiterations = true;
  Timer sw1, sw2;

  try
  {
    // touching output
    aggregators->touch();

    while(moreiterations)
    {
      // generem les copules
      sw1.resume();
      generateRatingsPaths();
      sw1.stop();

      // evaluem la cartera
      sw2.resume();
      evalueAggregators();
      sw2.stop();

      // increment contadors
      CONT++;

      // printing hashes
      if (hash > 0 && CONT%hash == 0)
      {
        cout << "#" << flush;
      }
      
      // checking stop criteria
      if (CONT >= MAXITERATIONS)
      {
        moreiterations = false;
      }

      // checking stop criteria
      if (MAXSECONDS > 0L && (sw1.read()+sw2.read())> MAXSECONDS)
      {
        moreiterations = false;
      }
    }

    // closing aggregators
    aggregators->flush(true);
    
    // sortim
    showInfo();
  }
  catch(Exception &e)
  {
    throw Exception(e, "MonteCarlo::execute()");
  }
}

//***************************************************************************
// genera les copules (una per cada tall temporal)
// @exception en cas d'error
//***************************************************************************
void ccruncher::MonteCarlo::generateRatingsPaths()
{
  // sense contemplar tecnologia antithetic
  if (!antithetic)
  {
    for(int i=1;i<=STEPS;i++)
    {
      copulas[i]->next();
    }
  }
  // amb tecnologia antithetic (antithetic == true)
  else
  {
    if (!reversed)
    {
      for(int i=1;i<=STEPS;i++)
      {
        copulas[i]->next();
      }
      reversed = true;
    } 
    else
    {
      reversed = false;
    }
  }
  
  // for each client we compute ratings path
  for (int i=0;i<N;i++)
  {
    generateRatingsPath(i);
  }
}

//***************************************************************************
// determina l'evolucio del rating del client per cada pas temporal
// deixa el resultat en el vector work0
//***************************************************************************
void ccruncher::MonteCarlo::generateRatingsPath(int iclient)
{
  for (int t=1;t<=STEPS;t++)
  {
    if (antithetic)
    {
      if (reversed)
      {
        rpaths[iclient][t] = mtrans->evalue(rpaths[iclient][t-1], copulas[t]->get(iclient));
      }
      else
      {
        rpaths[iclient][t] = mtrans->evalue(rpaths[iclient][t-1], 1.0-copulas[t]->get(iclient));
      }
    }
    else
    {
      rpaths[iclient][t] = mtrans->evalue(rpaths[iclient][t-1], copulas[t]->get(iclient));
    }
  }
}

//***************************************************************************
// valora el portfolio emprant copula en curs
// @exception en cas d'error
//***************************************************************************
void ccruncher::MonteCarlo::evalueAggregators() throw(Exception)
{
  // adding one simulation
  aggregators->append(rpaths, STEPS+1, N, clients);
}

//***************************************************************************
// showInfo
//***************************************************************************
void ccruncher::MonteCarlo::showInfo()
{
  cout << endl;
  cout << "begindate   = " << begindate << endl;
  cout << "steplength  = " << STEPLENGTH << endl;
  cout << "steps       = " << STEPS << endl;
  cout << "clients     = " << N << endl;
  cout << "simulations = " << MAXITERATIONS << endl;    
  cout << endl;

//  cerr << "generacio copules = " << sw1.read() << endl;
//  cerr << "evaluacio cartera = " << sw2.read() << endl;
}

//***************************************************************************
// setVerbosity
//***************************************************************************
void ccruncher::MonteCarlo::setVerbosity(int level)
{
  verbosity = level;
}

//***************************************************************************
// useMPI
//***************************************************************************
void ccruncher::MonteCarlo::useMPI(bool val)
{
  usempi = val;
}

//***************************************************************************
// setHash
//***************************************************************************
void ccruncher::MonteCarlo::setHash(int num)
{
  hash = num;
}

//***************************************************************************
// setFilePath
//***************************************************************************
void ccruncher::MonteCarlo::setFilePath(string path, bool force)
{
  fpath = path;
  bforce = force;
}
