
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

#include "kernel/Aggregator.hpp"
#include <cassert>

//===========================================================================
// constructor
// note: we don't diferentiate between asset-segmentations or borrower-segmentations
// because borrower segments has been recoded as asset segments (see Borrower code)
//===========================================================================
ccruncher::Aggregator::Aggregator(vector<SimulatedAsset> &assets, int isegmentation, 
    Segmentation &segmentation_, const string &filename, bool force) 
    throw(Exception) : segmentation(segmentation_), buffer(0)
{
  // initialization
  numsegments = segmentation.size();
  cont = 0L;
  icont = 0L;
  timer.resume();
  printRestSegment = false;
  for(unsigned int i=0; i<assets.size(); i++)
  {
    if (assets[i].ref->getSegment(isegmentation) == 0) 
    {
      printRestSegment = true;
      break;
    }
  }

  // buffer creation
  bufferrows = CCMAXBUFSIZE/(numsegments*sizeof(double));
  if (bufferrows <= 0) bufferrows = 1;
  buffer.assign(bufferrows*numsegments, 0.0);

  // file creation
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
//===========================================================================
bool ccruncher::Aggregator::append(vector<double> &losses) throw(Exception)
{
  assert((int)losses.size() == numsegments);

  // filling values
  double *p = &buffer[icont*numsegments];
  for(int i=0; i<numsegments; i++)
  {
    p[i] = losses[i];
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
// print
//===========================================================================
bool ccruncher::Aggregator::flush() throw(Exception)
{
  // if haven't values, exits
  if (icont <= 0)
  {
    return true;
  }

  // printing buffer content
  try
  {
    for(int i=0; i<icont; i++)
    {
      // printing simulation counter
      double *p = &buffer[i*numsegments];
      
      for (int j=(printRestSegment?0:1); j<numsegments-1; j++)
      {
        // printing simulated value
        fout << p[j] << ", ";
      }
      fout << p[numsegments-1] << "\n"; // endl not used because force to flush in disk
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

