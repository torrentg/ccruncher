
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#include <cassert>
#include <unistd.h>
#include "kernel/Aggregator.hpp"
#include "kernel/SimulatedData.hpp"
#include "utils/File.hpp"
#include "utils/config.h"

using namespace std;

/**************************************************************************//**
 * @details We don't diferentiate between segmentation composed by assets
 *          or obligors because obligor segments has been recoded as asset
 *          segments (see Obligor code). File creation mode:
 *          - 'a': Append content to file.
 *          - 'c': Create. Fails if file exist.
 *          - 'w': overwrites previous file content (if exist)
 * @see http://ccruncher.net/ofileref.html#segmentation
 * @param[in] segmentation Segmentation of this aggregator.
 * @param[in] ofile Filename.
 * @param[in] mode a=append, w=overwrite, c=create
 * @param[in] ifile Input file name (to put in header)
 * @param[in] exposures Segment exposures (to put in header)
 * @throw Exception Error creating file.
 */
ccruncher::Aggregator::Aggregator(const Segmentation &segmentation,
    const std::string &ofile, char mode, const std::string &ifile,
    const std::vector<double> &exposures)
{
  assert(mode=='a' || mode=='w' || mode=='c');

  // initialization
  filename = ofile;
  numsegments = segmentation.size();
  assert(numsegments > 0);
  assert(exposures.empty() || (int)exposures.size()==numsegments);

  // checking file creation mode
  bool force_creation = (mode!='a' && mode!='w');
  if (force_creation == true && access(filename.c_str(), W_OK) == 0)
  {
    throw Exception("file '" + filename + "' already exist");
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
      fout << "#==========================================================" << endl;
      fout << "# file generated by ccruncher-" << PACKAGE_VERSION << endl;
      fout << "# input file: " << ifile << endl;
      if (!exposures.empty()) {
        fout << "# exposure: ";
        for(int i=0; i<numsegments; i++) {
          fout << exposures[i] << (i<numsegments-1?", ":"");
        }
        fout << endl;
      }
      fout << "#==========================================================" << endl;
      for(int i=0; i<numsegments; i++) {
        fout << "\"" << segmentation.getSegment(i) << "\"" << (i<numsegments-1?", ":"");
      }
      fout << endl;
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, "error opening file '" + filename + "'");
  }

}

/**************************************************************************/
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

/**************************************************************************//**
 * @details Append a row to file.
 * @param[in] losses List of losses (length=numsegments).
 * @throw Exception Error writing data to file.
 */
void ccruncher::Aggregator::append(const double *losses)
{
  assert(losses != nullptr);
  assert(numsegments > 0);

  try
  {
    for(int i=0; i<numsegments-1; i++)
    {
      fout << losses[i] << ", ";
    }
    // endl not used because force to flush in disk
    fout << losses[numsegments-1] << "\n";
  }
  catch(std::exception &e)
  {
    throw Exception(e, "error writing in '" + filename + "'");
  }
}

/**************************************************************************//**
 * @details Synchronizes disk file and the associated stream buffer.
 * @throw Exception Error writing data to file.
 */
void ccruncher::Aggregator::flush()
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

/**************************************************************************//**
 * @return The number of segments.
 */
int ccruncher::Aggregator::size() const
{
  return numsegments;
}

