
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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
// note: we don't diferentiate between asset-segmentations or obligor-segmentations
// because obligor segments has been recoded as asset segments (see Obligor code)
//===========================================================================
ccruncher::Aggregator::Aggregator(vector<SimulatedAsset> &assets, int isegmentation, 
    Segmentation &segmentation_, const string &filename, bool force) 
    throw(Exception) : segmentation(segmentation_)
{
  // initialization
  numsegments = segmentation.size();
  printUnassignedSegment = false;
  for(unsigned int i=0; i<assets.size(); i++)
  {
    if (assets[i].ref->getSegment(isegmentation) == 0) 
    {
      printUnassignedSegment = true;
      break;
    }
  }

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
      for(int i=1; i<numsegments; i++)
      {
        fout << "\"" << segmentation.getSegment(i) << "\"" << (i<numsegments-1?", ":"");
      }
      if (printUnassignedSegment)
      {
        fout << ", \"" << segmentation.getSegment(0) << "\"";
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
void ccruncher::Aggregator::append(vector<double> &losses) throw(Exception)
{
  assert((int)losses.size() == numsegments);

  try
  {
    for (int i=1; i<numsegments; i++)
    {
      fout << losses[i] << (i<numsegments-1?", ":"");
    }
    if (printUnassignedSegment)
    {
      fout << (numsegments>1?", ":"") << losses[0];
    }
    fout << "\n"; // endl not used because force to flush in disk
  }
  catch(Exception e)
  {
    throw Exception(e, "error flushing content for aggregator " + segmentation.name);
  }
}

//===========================================================================
// flush
//===========================================================================
void ccruncher::Aggregator::flush() throw(Exception)
{
  try
  {
    fout.flush();
  }
  catch(Exception e)
  {
    throw Exception(e, "error flushing segmentation " + segmentation.name);
  }
}

