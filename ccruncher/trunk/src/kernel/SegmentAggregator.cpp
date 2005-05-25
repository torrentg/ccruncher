
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
// SegmentAggregator.cpp - SegmentAggregator code
// --------------------------------------------------------------------------
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cassert>
#include "utils/Arrays.hpp"
#include "kernel/SegmentAggregator.hpp"

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
void ccruncher::SegmentAggregator::define(int isegs, int iseg, components_t comps)
{
  assert(isegs >= 0);
  assert(iseg >= 0);

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
  vertexes = NULL;

  isegmentation = -1;
  isegment = -1;
  components = asset;

  filename = "";
  path = "UNASSIGNED";
  bforce = false;
  buffersize = MAXSIZE;

  N = 0L;
  M = 0;
  nclients = 0L;
  nassets = 0L;
  cont = 0L;
  icont = 0L;
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

  // deleting vertexes
  if (vertexes != NULL) {
    Arrays<DateValues>::deallocMatrix(vertexes, nclients);
    vertexes = NULL;
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
void ccruncher::SegmentAggregator::initialize(Date *dates, int m, vector<Client *> *clients,
  long n, Interests *interests_) throw(Exception)
{
  bool *clientflag = NULL;

  // assertions
  assert(dates != NULL);
  assert(m >= 0);
  assert(clients != NULL);
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

    // allocating & filling vertexes
    vertexes = allocVertexes(dates, M, clients, interests_);

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
long ccruncher::SegmentAggregator::getCNumClients(vector<Client *> *clients, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;

    if ((*clients)[i]->belongsTo(isegmentation, isegment))
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
long ccruncher::SegmentAggregator::getANumClients(vector<Client *> *clients, long n, bool *flags)
{
  vector<Asset> *assets;
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;

    assets = (*clients)[i]->getAssets();

    for(unsigned int j=0;j<assets->size();j++)
    {
      if ((*assets)[j].belongsTo(isegmentation, isegment))
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
long ccruncher::SegmentAggregator::getANumAssets(vector<Client *> *clients, long n, bool *flags)
{
  vector<Asset> *assets;
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    assets = (*clients)[i]->getAssets();

    for(unsigned int j=0;j<assets->size();j++)
    {
      if ((*assets)[j].belongsTo(isegmentation, isegment))
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
long ccruncher::SegmentAggregator::getCNumAssets(vector<Client *> *clients, long n, bool *flags)
{
  vector<Asset> *assets;
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    if (flags[i] == true)
    {
      assets = (*clients)[i]->getAssets();
      ret += assets->size();
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
// allocVertexes
//===========================================================================
DateValues** ccruncher::SegmentAggregator::allocVertexes(Date *dates, int m, vector<Client *> *clients, 
                Interests *interests_) throw(Exception)
{
  DateValues **ret = NULL;
  DateValues *aux = NULL;
  vector<Asset> *assets;

  //TODO: reconstruct this method
  
  ret = Arrays<DateValues>::allocMatrix(nclients, m);
  aux = Arrays<DateValues>::allocVector(m);

  for(long i=0;i<nclients;i++)
  {
    // initializing row
    for(int k=0;k<m;k++)
    {
      ret[i][k].date = dates[k];
      ret[i][k].cashflow = 0.0;
      ret[i][k].exposure = 0.0;
      ret[i][k].recovery = 0.0;
    }

    // finding client info
    long cpos = iclients[i];
    assets = (*clients)[cpos]->getAssets();

    // filling row
    for(unsigned int j=0;j<assets->size();j++)
    {
      if (components==client || (components==asset && (*assets)[j].belongsTo(isegmentation, isegment)))
      {
        (*assets)[j].getVertexes(dates, m, interests_, aux);

        for(int k=0;k<m;k++)
        {
          ret[i][k].cashflow += aux[k].cashflow;
          ret[i][k].exposure += aux[k].exposure;
          ret[i][k].recovery += aux[k].recovery;
        }
      }
    }
  }

  Arrays<DateValues>::deallocVector(aux);
  return ret;
}

//===========================================================================
// append
// input vector has length N with the index time (in months) where client defaults
//===========================================================================
void ccruncher::SegmentAggregator::append(int *defaulttimes) throw(Exception)
{
  assert(defaulttimes != NULL);
  int cpos;
  int itime;

  // initializing segment value
  cvalues[icont] = 0.0;

  // filling values
  for(long i=0;i<nclients;i++)
  {
    cpos = iclients[i];
    itime = defaulttimes[cpos];

    // if client non default in [0, M] time range
    if (itime >= M) {
      cvalues[icont] += vertexes[i][M-1].cashflow;
    }
    // if client defaults in [0, M] time range
    else {
      cvalues[icont] += vertexes[i][itime].cashflow;
      cvalues[icont] += vertexes[i][itime].recovery;
      cvalues[icont] -= vertexes[i][itime].exposure;
    }
  }
  
  // incrementing counters
  icont++;
  cont++;

  // flushing if buffer is full
  if (icont >= buffersize-1)
  {
    flush();
    icont = 0;
  }

  //TODO: flush if more than x sec. without flushing
}

//===========================================================================
// print
//===========================================================================
void ccruncher::SegmentAggregator::flush() throw(Exception)
{
  // opening output stream
  ofsopen();

  // printing buffer content (cvalues)
  try
  {
    if (nclients != 0 || nassets != 0)
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
  }
  catch(Exception e)
  {
    ofsclose();
    throw Exception(e, "SegmentAggregator::flush()");
  }

  // closing output stream
  ofsclose();
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
    buffersize = MAXSIZE;
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
long ccruncher::SegmentAggregator::getNumElements()
{
  if (components == asset) {
    return nassets;
  } else {
    return nclients;
  }
}
