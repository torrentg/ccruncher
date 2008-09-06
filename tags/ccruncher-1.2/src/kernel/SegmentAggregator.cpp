
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
// 2005/05/20 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/06/26 - Gerard Torrent [gerard@mail.generacio.com]
//   . portfolio evaluation modified
//
// 2005/07/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . added timer to control last flush time
//
// 2005/07/09 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed exposure/recovery by netting
//
// 2005/07/30 - Gerard Torrent [gerard@mail.generacio.com]
//   . moved <cassert> include at last position
//
// 2005/08/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . implemented MPI support
//
// 2005/09/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . added param montecarlo.simule
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . Client refactoring
//
// 2006/01/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . SegmentAggregator refactoring
//
// 2006/01/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed simule and method params
//
// 2007/07/31 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed interests arguments
//
// 2007/08/03 - Gerard Torrent [gerard@mail.generacio.com]
//   . Client class renamed to Borrower
//
//===========================================================================

#include <cmath>
#include "utils/Arrays.hpp"
#include "utils/Utils.hpp"
#include "utils/File.hpp"
#include "utils/Format.hpp"
#include "kernel/SegmentAggregator.hpp"
#include "utils/ccmpi.h"
#include <cassert>

//===========================================================================
// void constructor
//===========================================================================
ccruncher::SegmentAggregator::SegmentAggregator() : iborrowers(0), losses(0), cvalues(0)
{
  init();
}

//===========================================================================
// init
//===========================================================================
void ccruncher::SegmentAggregator::init()
{
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
  nborrowers = 0L;
  nassets = 0L;
  cont = 0L;
  icont = 0L;

  timer.resume();
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
// destructor
//===========================================================================
ccruncher::SegmentAggregator::~SegmentAggregator()
{
  // nothing to do
}

//===========================================================================
// initialize
//===========================================================================
void ccruncher::SegmentAggregator::initialize(vector<Date> &dates, vector<Borrower *> &borrowers,
  long n) throw(Exception)
{
  bool *borrowerflag = NULL;

  // assertions
  assert(n >= 0);

  // initial validations
  if (isegmentation == -1)
  {
    throw Exception("aggregator initialized with main variables not fixed (call define method first)");
  }

  // setting vars values
  N = n;
  M = dates.size();

  // allocating borrowerflag
  borrowerflag = Arrays<bool>::allocVector(N, false);

  try
  {
    if (components == asset)
    {
      // counting borrowers + filling borrowerflag
      nborrowers = getANumBorrowers(borrowers, N, borrowerflag);
      // counting assets
      nassets = getANumAssets(borrowers, N, borrowerflag);
    }
    else // components == borrowers
    {
      // counting borrowers + filling borrowerflag
      nborrowers = getCNumBorrowers(borrowers, N, borrowerflag);
      // counting assets
      nassets = getCNumAssets(borrowers, N, borrowerflag);
    }

    // allocating & fixing borrowers
    allocIBorrowers(nborrowers, borrowerflag, N);

    // allocating & filling losses
    allocLosses(dates, borrowers);

    // allocating cvalues
    cvalues.reserve(buffersize);
    cvalues.assign(buffersize, 0.0);
  }
  catch(std::exception &e)
  {
    Arrays<bool>::deallocVector(borrowerflag);
    throw Exception(e, "error initializing aggregator");
  }

  Arrays<bool>::deallocVector(borrowerflag);
}

//===========================================================================
// getCNumBorrowers
//===========================================================================
long ccruncher::SegmentAggregator::getCNumBorrowers(vector<Borrower *> &borrowers, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;

    if (borrowers[i]->belongsTo(isegmentation, isegment))
    {
      flags[i] = true;
      ret++;
    }
  }

  return ret;
}

//===========================================================================
// getANumBorrowers
//===========================================================================
long ccruncher::SegmentAggregator::getANumBorrowers(vector<Borrower *> &borrowers, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    flags[i] = false;

    vector<Asset> &assets = borrowers[i]->getAssets();

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
long ccruncher::SegmentAggregator::getANumAssets(vector<Borrower *> &borrowers, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    vector<Asset> &assets = borrowers[i]->getAssets();

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
long ccruncher::SegmentAggregator::getCNumAssets(vector<Borrower *> &borrowers, long n, bool *flags)
{
  long ret = 0L;

  for(long i=0;i<n;i++)
  {
    if (flags[i] == true)
    {
      vector<Asset> &assets = borrowers[i]->getAssets();
      ret += assets.size();
    }
  }

  return ret;
}

//===========================================================================
// allocIBorrowers
//===========================================================================
void ccruncher::SegmentAggregator::allocIBorrowers(long len, bool *flags, long n) throw(Exception)
{
  long aux = 0L;

  // assertion
  assert(len <= n);

  iborrowers.reserve(len);
  iborrowers.assign(len, -1L);

  // initializing values
  for(long i=0;i<n;i++)
  {
    if(flags[i] == true)
    {
      iborrowers[aux] = i;
      aux++;
    }
  }

  // assertion
  assert(aux == len);
}

//===========================================================================
// allocLosses
//===========================================================================
void ccruncher::SegmentAggregator::allocLosses(vector<Date> &dates, vector<Borrower *> &borrowers) throw(Exception)
{
  losses.reserve(nborrowers);
  losses.assign(nborrowers, vector<double>(0));

  for(long i=0;i<nborrowers;i++)
  {
    losses[i].reserve(dates.size());
    losses[i].assign(dates.size(), 0.0);

    // finding borrower info
    long cpos = iborrowers[i];
    vector<Asset> &assets = borrowers[cpos]->getAssets();

    // filling row
    for(unsigned int j=0;j<assets.size();j++)
    {
      if (components==borrower || (components==asset && assets[j].belongsTo(isegmentation, isegment)))
      {
        for(unsigned int k=0; k<dates.size(); k++)
        {
          losses[i][k] += assets[j].getLoss(k);
        }
      }
    }
  }
}

//===========================================================================
// append
// input vector has length N with the index time (in months) where borrower defaults
//===========================================================================
bool ccruncher::SegmentAggregator::append(int *defaulttimes) throw(Exception)
{
  assert(defaulttimes != NULL);
  int cpos;
  int itime;

  // initializing segment value
  cvalues[icont] = 0.0;

  // filling values
  for(long i=0;i<nborrowers;i++)
  {
    cpos = iborrowers[i];
    itime = defaulttimes[cpos];

    // asserting that at time 0 borrower is alive
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
  if (nborrowers == 0 && nassets == 0)
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
    MPI::COMM_WORLD.Send(&(cvalues[0]), icont, MPI::DOUBLE, 0, MPI_TAG_DATA);
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
      throw Exception(e, "error flushing aggregator content");
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
    throw Exception("aggregator with invalid buffer size (" + Format::int2string(ibs) + " < 0)");
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
    throw Exception("error opening file " + getFilePath());
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
    throw Exception("error closing file" + getFilePath());
  }
}

//===========================================================================
// touch. test output file correctnes
//===========================================================================
void ccruncher::SegmentAggregator::touch() throw(Exception)
{
  if (bforce == false && access(getFilePath().c_str(), W_OK) == 0)
  {
    throw Exception("file " + getFilePath() + " already exist");
  }

  try
  {
    fout.open(getFilePath().c_str(), ios::out|ios::trunc);
    fout.close();
  }
  catch(...)
  {
    throw Exception("error initializing file " + getFilePath());
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
    return nborrowers;
  }
}
