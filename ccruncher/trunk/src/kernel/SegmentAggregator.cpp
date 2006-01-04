
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
// SegmentAggregator.cpp - SegmentAggregator code - $Rev$
// --------------------------------------------------------------------------
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/06/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . portfolio evaluation modified
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added timer to control last flush time
//
// 2005/07/09 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed exposure/recovery by netting
//
// 2005/07/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . moved <cassert> include at last position
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented MPI support
//
// 2005/09/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added param montecarlo.simule
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Client refactoring
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . SegmentAggregator refactoring
//
// 2006/01/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed simule and method params
//
//===========================================================================

#include <cmath>
#include "utils/Arrays.hpp"
#include "utils/Utils.hpp"
#include "kernel/SegmentAggregator.hpp"
#include <cassert>

#ifdef USE_MPI
  #include <mpi.h>
  #define MPI_VAL_WORK 1025   // work tag (see task variable)
  #define MPI_VAL_STOP 1026   // stop tag (see task variable)
  #define MPI_TAG_DATA 1027   // data tag (used to send results)
  #define MPI_TAG_INFO 1028   // info tag (used to send results)
  #define MPI_TAG_TASK 1029   // task tag (used to send results)
  #define MPI_TAG_EXIT 1030   // task tag (used to send results)
#endif

//===========================================================================
// void constructor
//===========================================================================
ccruncher::SegmentAggregator::SegmentAggregator()
{
  init();
}

//===========================================================================
// define
//===========================================================================
void ccruncher::SegmentAggregator::define(int iaggre, int isegs, int iseg, components_t comps)
{
  assert(iaggre >= 0);
  assert(isegs >= 0);
  assert(iseg >= 0);

  iaggregator = iaggre;
  isegmentation = isegs;
  isegment = iseg;
  components = comps;
}

//===========================================================================
// init
//===========================================================================
void ccruncher::SegmentAggregator::init()
{
  iclients = NULL;
  cvalues = NULL;
  losses = NULL;

  iaggregator = -1;
  isegmentation = -1;
  isegment = -1;
  components = asset;

  filename = "";
  path = "UNASSIGNED";
  bforce = false;
  buffersize = CCMAXBUFSIZE;

  N = 0L;
  M = 0;
  nclients = 0L;
  nassets = 0L;
  cont = 0L;
  icont = 0L;

  timer.resume();
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::SegmentAggregator::~SegmentAggregator()
{
  release();
}

//===========================================================================
// release
//===========================================================================
void ccruncher::SegmentAggregator::release()
{
  // deleting clients array
  if (iclients != NULL) {
    Arrays<long>::deallocVector(iclients);
    iclients = NULL;
  }

  // deleting losses arrays
  if (losses != NULL) {
    Arrays<double>::deallocMatrix(losses, nclients);
    losses = NULL;
  }

  // deleting cvalues
  if (cvalues != NULL) {
    Arrays<double>::deallocVector(cvalues);
    cvalues = NULL;
  }
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::SegmentAggregator::initialize(Date *dates, int m, vector<Client *> &clients,
  long n, Interests &interests_) throw(Exception)
{
  bool *clientflag = NULL;

  // assertions
  assert(dates != NULL);
  assert(m >= 0);
  assert(n >= 0);

  // initial validations
  if (isegmentation == -1)
  {
    throw Exception("SegmentAggregator::initialize(): main variables not fixed. Call define method first.");
  }

  // setting vars values
  N = n;
  M = m;

  // allocating clientflag
  clientflag = Arrays<bool>::allocVector(N, false);

  try
  {
    if (components == asset)
    {
      // counting clients + filling clientflag
      nclients = getANumClients(clients, N, clientflag);
      // counting assets
      nassets = getANumAssets(clients, N, clientflag);
    }
    else // components == clients
    {
      // counting clients + filling clientflag
      nclients = getCNumClients(clients, N, clientflag);
      // counting assets
      nassets = getCNumAssets(clients, N, clientflag);
    }

    // allocating & fixing clients
    iclients = allocIClients(nclients, clientflag, N);

    // allocating & filling losses
    losses = allocLosses(dates, M, clients, interests_);

    // allocating cvalues
    cvalues = Arrays<double>::allocVector(buffersize);

    // see MonteCarlo::init() for vertex computation
  }
  catch(std::exception &e)
  {
    Arrays<bool>::deallocVector(clientflag);
    throw Exception(e, "SegmentAggregator::initialize()");
  }

  Arrays<bool>::deallocVector(clientflag);
}

//===========================================================================
// getCNumClients
//===========================================================================
long ccruncher::SegmentAggregator::getCNumClients(vector<Client *> &clients, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;

    if (clients[i]->belongsTo(isegmentation, isegment))
    {
      flags[i] = true;
      ret++;
    }
  }

  return ret;
}

//===========================================================================
// getANumClients
//===========================================================================
long ccruncher::SegmentAggregator::getANumClients(vector<Client *> &clients, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;

    vector<Asset> &assets = clients[i]->getAssets();

    for(unsigned int j=0;j<assets.size();j++)
    {
      if (assets[j].belongsTo(isegmentation, isegment))
      {
        flags[i] = true;
        ret++;
        break;
      }
    }
  }

  return ret;
}

//===========================================================================
// getANumAssets
//===========================================================================
long ccruncher::SegmentAggregator::getANumAssets(vector<Client *> &clients, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    vector<Asset> &assets = clients[i]->getAssets();

    for(unsigned int j=0;j<assets.size();j++)
    {
      if (assets[j].belongsTo(isegmentation, isegment))
      {
        ret++;
      }
    }
  }

  return ret;
}

//===========================================================================
// getCNumAssets
//===========================================================================
long ccruncher::SegmentAggregator::getCNumAssets(vector<Client *> &clients, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    if (flags[i] == true)
    {
      vector<Asset> &assets = clients[i]->getAssets();
      ret += assets.size();
    }
  }

  return ret;
}

//===========================================================================
// allocIClients
//===========================================================================
long* ccruncher::SegmentAggregator::allocIClients(long len, bool *flags, long n) throw(Exception)
{
  long *ret = NULL;
  long aux = 0L;

  // assertion
  assert(len <= n);

  // allocating space
  ret = Arrays<long>::allocVector(len, -1);

  // initializing values
  for(long i=0;i<n;i++)
  {
    if(flags[i] == true)
    {
      ret[aux] = i;
      aux++;
    }
  }

  // assertion
  assert(aux == len);

  return ret;
}

//===========================================================================
// allocLosses
//===========================================================================
double** ccruncher::SegmentAggregator::allocLosses(Date *dates, int m, vector<Client *> &clients,
                Interests &interests_) throw(Exception)
{
  double **ret = NULL;

  ret = Arrays<double>::allocMatrix(nclients, m, 0.0);

  for(long i=0;i<nclients;i++)
  {
    // finding client info
    long cpos = iclients[i];
    vector<Asset> &assets = clients[cpos]->getAssets();

    // filling row
    for(unsigned int j=0;j<assets.size();j++)
    {
      if (components==client || (components==asset && assets[j].belongsTo(isegmentation, isegment)))
      {
        assets[j].getLosses(dates, m, interests_, ret[i]);
      }
    }
  }

  return ret;
}

//===========================================================================
// append
// input vector has length N with the index time (in months) where client defaults
//===========================================================================
bool ccruncher::SegmentAggregator::append(int *defaulttimes) throw(Exception)
{
  assert(defaulttimes != NULL);
  int cpos;
  int itime;
  double val=0.0;

  // initializing segment value
  cvalues[icont] = 0.0;

  // filling values
  for(long i=0;i<nclients;i++)
  {
    cpos = iclients[i];
    itime = defaulttimes[cpos];

    // asserting that at time 0 client is alive
    assert(itime > 0);

    if (itime < M) {
      cvalues[icont] += losses[i][itime];
    }
  }

  // incrementing counters
  icont++;
  cont++;

  // flushing if buffer is full
  if (icont >= buffersize || timer.read() > CCEFLUSHSECS)
  {
    return flush();
  }
  else
  {
    // exit function
    return true;
  }
}

//===========================================================================
// appendRawData
//===========================================================================
bool ccruncher::SegmentAggregator::appendRawData(double *data, int datasize) throw(Exception)
{
  assert(data != NULL);
  assert(datasize >= 0);

  // appending data
  for(int i=0;i<datasize;i++)
  {
    cvalues[icont] = data[i];
    icont++;
    cont++;

    // flushing if buffer is full
    if (icont >= buffersize || timer.read() > CCEFLUSHSECS)
    {
      flush();
    }
  }

  // exit function
  return true;
}

//===========================================================================
// print
//===========================================================================
bool ccruncher::SegmentAggregator::flush() throw(Exception)
{
  // if haven't elements to aggregate, exits
  if (nclients == 0 && nassets == 0)
  {
    return true;
  }

  // if haven't values, exits
  if (icont <= 0)
  {
    return true;
  }

#ifdef USE_MPI
  if (!Utils::isMaster())
  {
    int task=0;

    // sending info
    MPI::COMM_WORLD.Send(&iaggregator, 1, MPI::INT, 0, MPI_TAG_INFO);
    // sending data
    MPI::COMM_WORLD.Send(cvalues, icont, MPI::DOUBLE, 0, MPI_TAG_DATA);
    // receiving task
    MPI::COMM_WORLD.Recv(&task, 1, MPI::INT, 0, MPI_TAG_TASK);
    // reseting timer
    timer.start();
    // reseting buffer counter
    icont = 0;
    // return function
    return (task==MPI_VAL_WORK?true:false);
  }
  else
#endif
  {
    // opening output stream
    ofsopen();

    // printing buffer content (cvalues)
    try
    {
      for(long i=0;i<icont;i++)
      {
        // printing simulation counter
        fout << (cont-icont+i+1) << "\t";
        // printing simulated value
        fout << cvalues[i] << "\n";
      }

      // reseting buffer counter
      icont = 0;
    }
    catch(Exception e)
    {
      ofsclose();
      throw Exception(e, "SegmentAggregator::flush()");
    }

    // closing output stream
    ofsclose();

    // reseting timer
    timer.start();

    // exit function
    return true;
  }
}

//===========================================================================
// setOutputProperties
//===========================================================================
void ccruncher::SegmentAggregator::setOutputProperties(const string &spath, const string &sfilename, bool force, int ibs) throw(Exception)
{
  filename = sfilename;
  path = File::normalizePath(spath);
  bforce = force;

  // setting buffersize value
  if (ibs > 0) {
    buffersize = ibs;
  }
  else if (ibs == 0) {
    buffersize = CCMAXBUFSIZE;
  }
  else {
    throw Exception("SegmentAggregator::setOutputProperties(): invalid buffer size");
  }
}

//===========================================================================
// ofsopen
//===========================================================================
void ccruncher::SegmentAggregator::ofsopen() throw(Exception)
{
  try
  {
    fout.open(getFilePath().c_str(), ios::out|ios::app);
    fout.setf(ios::fixed);
    fout.setf(ios::showpoint);
    fout.precision(2);
  }
  catch(...)
  {
    throw Exception("SegmentAggregator::open(): error opening file " + getFilePath());
  }
}

//===========================================================================
// close
//===========================================================================
void ccruncher::SegmentAggregator::ofsclose() throw(Exception)
{
  try
  {
    fout.close();
  }
  catch(...)
  {
    throw Exception("SegmentAggregator::open(): error closing file" + getFilePath());
  }
}

//===========================================================================
// touch. test output file correctnes
//===========================================================================
void ccruncher::SegmentAggregator::touch() throw(Exception)
{
  if (bforce == false && access(getFilePath().c_str(), W_OK) == 0)
  {
    throw Exception("SegmentAggregator::touch(): file " + getFilePath() + " already exist");
  }

  try
  {
    fout.open(getFilePath().c_str(), ios::out|ios::trunc);
    fout.close();
  }
  catch(...)
  {
    throw Exception("SegmentAggregator::touch(): error initializing file " + getFilePath());
  }
}

//===========================================================================
// getFilePath
//===========================================================================
string ccruncher::SegmentAggregator::getFilePath() throw(Exception)
{
  return path + filename;
}

//===========================================================================
// getNumElements
//===========================================================================
long ccruncher::SegmentAggregator::getNumElements() const
{
  if (components == asset) {
    return nassets;
  } else {
    return nclients;
  }
}
