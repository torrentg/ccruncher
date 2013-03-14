
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#include <unistd.h>
#include "kernel/Aggregator.hpp"
#include "kernel/SimulatedData.hpp"
#include "utils/File.hpp"
#include <cassert>

using namespace std;

//===========================================================================
// constructor
// note: we don't diferentiate between asset-segmentations or obligor-segmentations
// because obligor segments has been recoded as asset segments (see Obligor code)
//===========================================================================
ccruncher::Aggregator::Aggregator(const char *assets, size_t numassets, size_t assetsize,
    int isegmentation, const Segmentations &segmentations,
    const string &filename_, char mode) throw(Exception)
{
  assert(assets != NULL && assetsize > 0);
  assert(mode=='a' || mode=='w' || mode=='c');
  assert(0 <= isegmentation && isegmentation < segmentations.size());

  Segmentation segmentation = segmentations.getSegmentation(isegmentation);

  // initialization
  filename = filename_;
  numsegments = segmentations.getSegmentation(isegmentation).size();
  printUnassignedSegment = false;
  for(size_t i=0; i<numassets; i++)
  {
    SimulatedAsset *asset = (SimulatedAsset*) &(assets[i*assetsize]);
    if ((&(asset->segments))[isegmentation] == 0)
    {
      printUnassignedSegment = true;
      break;
    }
  }
  
  // checking file creation mode
  bool force_creation = (mode!='a' && mode!='w');
  if (force_creation == true && access(filename.c_str(), W_OK) == 0)
  {
    throw Exception("file " + filename + " already exist");
  }

  try
  {
    // opening/creating file
    fout.exceptions(ios::failbit | ios::badbit);
    fout.open(filename.c_str(), ios::out|(mode=='a'?(ios::app):(ios::trunc)));
    fout.setf(ios::fixed);
    fout.setf(ios::showpoint);
    fout.precision(2);

    // printing header
    if (mode != 'a' || (mode == 'a' && File::filesize(filename) == 0))
    {
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
  }
  catch(std::exception &e)
  {
    throw Exception(e, "error opening file '" + filename + "'");
  }

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
  catch(std::exception &)
  {
    // destructor can't throw exceptions
  }
}

//===========================================================================
// append
//===========================================================================
void ccruncher::Aggregator::append(const double *losses) throw(Exception)
{
  assert(losses != NULL);

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
  catch(std::exception &e)
  {
    throw Exception(e, "error writing in '" + filename + "'");
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
  catch(std::exception &e)
  {
    throw Exception(e, "error writing in '" + filename + "'");
  }
}

//===========================================================================
// return the number of segments
//===========================================================================
int ccruncher::Aggregator::size() const
{
  return numsegments;
}

