
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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
#include "utils/Utils.hpp"
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
 * @param[in] filename Filename.
 * @param[in] mode a=append, w=overwrite, c=create
 * @param[in] numSegments Number of segments.
 * @throw Exception Error creating file.
 */
ccruncher::Aggregator::Aggregator(const std::string &filename, char mode, ushort numSegments)
{
  if (numSegments == 0) {
    throw Exception("trying to aggregate 0 segments");
  }
  mNumSegments = numSegments;

  if (mode != 'a' && mode != 'w' && mode != 'c') {
    throw Exception("invalid file mode");
  }
  mMode = mode;

  bool force_creation = (mMode!='a' && mMode!='w');
  if (force_creation == true && access(filename.c_str(), W_OK) == 0) {
    throw Exception("file '" + filename + "' already exist");
  }

  try
  {
    mFile.exceptions(ios::failbit | ios::badbit);
    mFile.open(filename.c_str(), ios::out|(mMode=='a'?(ios::app):(ios::trunc)));
    mFile.setf(ios::fixed);
    mFile.setf(ios::showpoint);
    mFile.precision(2);
    mFilename = filename;
  }
  catch(std::exception &e)
  {
    throw Exception(e, "error opening file '" + filename + "'");
  }
}

/**************************************************************************//**
 * @param[in] segmentation Segmentation of this aggregator.
 * @param[in] exposures Segment exposures.
 * @throw Exception Error printing header.
 */
void ccruncher::Aggregator::printHeader(const Segmentation &segmentation, const vector<double> &exposures)
{
  if (mNumSegments != segmentation.size() || mNumSegments != exposures.size()) {
    throw Exception("invalid aggregator header");
  }

  if (mMode != 'a' || (mMode == 'a' && Utils::filesize(mFilename) == 0))
  {
    mFile << "#==========================================================" << endl;
    mFile << "# file generated by ccruncher-" << PACKAGE_VERSION << endl;
    mFile << "# exposure: ";
    for(size_t i=0; i<exposures.size(); i++) {
      mFile << exposures[i] << (i<exposures.size()-1?", ":"");
    }
    mFile << endl;
    mFile << "#==========================================================" << endl;
    for(ushort i=0; i<segmentation.size(); i++) {
      mFile << "\"" << segmentation.getSegment(i) << "\"" << (i<segmentation.size()-1?", ":"");
    }
    mFile << endl;
  }
}

/**************************************************************************/
ccruncher::Aggregator::~Aggregator()
{
  try
  {
    mFile.close();
  }
  catch(std::exception &)
  {
    // destructor can't throw exceptions
  }
}

/**************************************************************************//**
 * @details Append a row to file.
 * @param[in] losses List of losses (length=mNumSegments).
 * @throw Exception Error writing data to file.
 */
void ccruncher::Aggregator::append(const double *losses)
{
  assert(losses != nullptr);
  assert(mNumSegments > 0);

  try
  {
    for(int i=0; i<mNumSegments-1; i++)
    {
      mFile << losses[i] << ", ";
    }
    // endl not used because force to flush in disk
    mFile << losses[mNumSegments-1] << "\n";
  }
  catch(std::exception &e)
  {
    throw Exception(e, "error writing in '" + mFilename + "'");
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
    mFile.flush();
  }
  catch(std::exception &e)
  {
    throw Exception(e, "error writing in '" + mFilename + "'");
  }
}


