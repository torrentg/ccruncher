
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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
#include "utils/Arrays.hpp"
#include "utils/Utils.hpp"
#include "utils/File.hpp"
#include "utils/Format.hpp"
#include "kernel/Aggregator.hpp"
#include "utils/ccmpi.h"
#include <cassert>

#define CVALUES(segment,simulation) cvalues[segment+simulation*numsegments]

//===========================================================================
// void constructor
//===========================================================================
ccruncher::Aggregator::Aggregator(int id, Segmentation &segmentation_, 
  vector<Borrower *> &borrowers_, long n) : segmentation(segmentation_), 
  borrowers(borrowers_)
{
  assert(id >= 0);
  iaggregator = id;
  assert(n >= 0);
  numborrowers = n;
  numsegments = segmentation.size();
  printRestSegment = hasRestSegment();

  bufferrows = CCMAXBUFSIZE/(numsegments*sizeof(double));
  if (bufferrows <= 0) bufferrows = 1;
  cvalues.reserve(bufferrows*numsegments);
  cvalues.assign(bufferrows*numsegments, 0.0);

  cont = 0L;
  icont = 0L;
  timer.resume();
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Aggregator::~Aggregator()
{
  try
  {
    fout.close();
  }
  catch(...)
  {
    throw Exception("error closing file for aggregator " + segmentation.name);
  }
}

//===========================================================================
// append
// input vector has length numborrowers with the index time (in months) where borrower defaults
//===========================================================================
bool ccruncher::Aggregator::append(Date *defaulttimes, bool force) throw(Exception)
{
  assert(defaulttimes != NULL);
  assert(segmentation.components==borrower || segmentation.components==asset);

  if (segmentation.components == borrower)
  {
    append1(defaulttimes, force);
  }
  else 
  {
    append2(defaulttimes, force);
  }

  // flushing if buffer is full
  if (icont >= bufferrows || timer.read() > CCEFLUSHSECS)
  {
    return flush();
  }
  else
  {
    return true;
  }
}

//===========================================================================
// append1
// when segmentation components are borrowers
//===========================================================================
void ccruncher::Aggregator::append1(Date *defaulttimes, bool force)
{
  // initializing values
  int isegmentation = segmentation.order;
  for(unsigned int i=0; i<numsegments; i++) 
  {
    CVALUES(i,icont) = 0.0;
  }

  // filling values
  for(unsigned int i=0; i<numborrowers; i++)
  {
    int isegment = borrowers[i]->getSegment(isegmentation);
    vector<Asset> &assets = borrowers[i]->getAssets();
    for(unsigned int j=0; j<assets.size(); j++) 
    {
      CVALUES(isegment,icont) += assets[j].getLoss(defaulttimes[i], force);
    }
  }

  // incrementing counters
  icont++;
  cont++;
}

//===========================================================================
// append2
// when segmentation components are assets
//===========================================================================
void ccruncher::Aggregator::append2(Date *defaulttimes, bool force)
{
  // initializing values
  int isegmentation = segmentation.order;
  for(unsigned int i=0; i<numsegments; i++) 
  {
    CVALUES(i,icont) = 0.0;
  }

  // filling values
  for(unsigned int i=0; i<numborrowers; i++)
  {
    vector<Asset> &assets = borrowers[i]->getAssets();
    for(unsigned int j=0; j<assets.size(); j++) 
    {
      int isegment = assets[j].getSegment(isegmentation);
      CVALUES(isegment,icont) += assets[j].getLoss(defaulttimes[i], force);
    }
  }

  // incrementing counters
  icont++;
  cont++;
}

//===========================================================================
// appendRawData
// returns the number of simulations added
//===========================================================================
long ccruncher::Aggregator::appendRawData(double *data, int datasize) throw(Exception)
{
  long numsims = 0L;
  assert(data != NULL);
  assert(datasize >= 0);
  
  //checking if datasize is multiple of numsegments
  if (datasize % numsegments != 0)
  {
    throw Exception("trying to append invalid raw data in " + segmentation.name);
  }

  // appending data
  numsims = datasize/numsegments;
  for(int i=0; i<numsims; i++)
  {
    for(int j=0; j<numsegments; j++)
    {
      CVALUES(j,icont) = data[j+i*numsegments];
    }
    icont++;
    cont++;
    // flushing if buffer is full
    if (icont >= bufferrows || timer.read() > CCEFLUSHSECS)
    {
      flush();
    }
  }

  // exit function
  return numsims;
}

//===========================================================================
// print
//===========================================================================
bool ccruncher::Aggregator::flush() throw(Exception)
{
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
    MPI::COMM_WORLD.Send(&(cvalues[0]), icont*numsegments, MPI::DOUBLE, 0, MPI_TAG_DATA);
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
    // printing buffer content (cvalues)
    try
    {
      for(long i=0; i<icont; i++)
      {
        // printing simulation counter
        // fout << (cont-icont+i+1) << ", ";
        for (long j=(printRestSegment?0:1); j<numsegments; j++)
        {
          // printing simulated value
          fout << CVALUES(j,i) << (j<numsegments-1?", ":"");
        }
        fout << "\n"; //don't use endl because force to flush in disk
      }
      // reseting buffer counter
      icont = 0;
    }
    catch(Exception e)
    {
      throw Exception(e, "error flushing content for aggregator " + segmentation.name);
    }

    // reseting timer
    timer.start();

    // exit function
    return true;
  }
}

//===========================================================================
// setOutputProperties
//===========================================================================
void ccruncher::Aggregator::setOutputProperties(const string &filename, bool force) throw(Exception)
{
  if (!Utils::isMaster()) 
  {
    return;
  }
  if (force == false && access(filename.c_str(), W_OK) == 0)
  {
    throw Exception("file " + filename + " already exist");
  }
  try
  {
    fout.open(filename.c_str(), ios::out|ios::trunc); //ios.app
    fout.setf(ios::fixed);
    fout.setf(ios::showpoint);
    fout.precision(2);
    if (numsegments == 1)
    {
      fout << "\"" << segmentation.name << "\"";
    }
    else
    {
      for(int i=(printRestSegment?0:1); i<numsegments; i++)
      {
        fout << "\"" << segmentation[i].name << "\"" << (i<numsegments-1?", ":"");
      }
    }
    fout << endl;
  }
  catch(...)
  {
    throw Exception("error opening file " + filename);
  }
}

//===========================================================================
// hasRestSegment
// if exist a borrower/asset in the rest segment returns true, false otherwise
//===========================================================================
bool ccruncher::Aggregator::hasRestSegment()
{
  int isegmentation = segmentation.order;

  if (segmentation.components == borrower)
  {
    for(unsigned int i=0; i<numborrowers; i++)
    {
      if (borrowers[i]->getSegment(isegmentation) == 0) 
      {
        return true;
      }
    }
  }
  else 
  {
    for(unsigned int i=0; i<numborrowers; i++)
    {
      vector<Asset> &assets = borrowers[i]->getAssets();
      for(unsigned int j=0; j<assets.size(); j++) 
      {
        if (assets[j].getSegment(isegmentation) == 0) 
        {
          return true;
        }
      }
    }
  }

  return false;
}

//===========================================================================
// getBufferSize
// returns buffersize in bytes
//===========================================================================
long ccruncher::Aggregator::getBufferSize()
{
  return bufferrows*numsegments*sizeof(double);
}
