
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
#include "utils/Arrays.hpp"
#include "utils/Utils.hpp"
#include "utils/File.hpp"
#include "utils/Format.hpp"
#include "kernel/Aggregator.hpp"
#include "utils/ccmpi.h"
#include <cassert>

#define CVALUES(segment,simulation) cvalues[segment+simulation*numsegments]

//===========================================================================
// constructor
// note: we don't diferentiate between asset-segmentations or borrower-segmentations
// because borrower segments has been recoded as asset segments (see Borrower code)
//===========================================================================
ccruncher::Aggregator::Aggregator(int isegmentation_, Segmentation &segmentation_, 
  vector<SimulatedAsset> &assets) : segmentation(segmentation_), isegments(0)
{
  assert(isegmentation_ >= 0);
  isegmentation = isegmentation_;
  numsegments = segmentation.size();
  printRestSegment = false;
  isegments.assign(assets.size(), 0);
  
  for(unsigned int i=0; i<assets.size(); i++)
  {
    isegments[i] = assets[i].ref->getSegment(isegmentation);
    if (isegments[i] == 0) 
    {
      printRestSegment = true;
    }
  }

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
    flush();
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
bool ccruncher::Aggregator::append(vector<SimulatedAsset> &assets) throw(Exception)
{
  assert(assets.size() == isegments.size());

  // initializing values
  for(int i=0; i<numsegments; i++) 
  {
    CVALUES(i,icont) = 0.0;
  }

  // filling values
  for(unsigned int i=0; i<assets.size(); i++)
  {
    CVALUES(isegments[i],icont) += assets[i].loss;
  }

  // incrementing counters
  icont++;
  cont++;

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
    MPI::COMM_WORLD.Send(&isegmentation, 1, MPI::INT, 0, MPI_TAG_INFO);
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
        fout << "\"" << segmentation.getSegment(i) << "\"" << (i<numsegments-1?", ":"");
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
// getBufferSize
// returns buffersize in bytes
//===========================================================================
long ccruncher::Aggregator::getBufferSize()
{
  return bufferrows*numsegments*sizeof(double);
}

