
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "params/Segmentation.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Simulated values of a segmentation.
 *
 * @details Manages the CSV file containing the simulated values for a
 *          given segmentation. First line contains columns header (segment
 *          names). Every row corresponds to the simulated losses of a
 *          segmentation.
 *
 * @see http://ccruncher.net/ofileref.html#segmentation
 */
class Aggregator
{

  private:

    //! File name
    std::string mFilename;
    //! Output file stream
    std::ofstream mFile;
    //! Number of segments
    unsigned short mNumSegments;
    //! Open file mode
    char mMode;

  public:

    //! Constructor
    Aggregator(const std::string &mFilename, char mode, unsigned short numSegments);
    //! Non-copyable class
    Aggregator(const Aggregator &) = delete;
    //! Non-copyable class
    Aggregator & operator=(const Aggregator &) = delete;
    //! Destructor
    ~Aggregator();
    //! Set header info
    void printHeader(const Segmentation &segmentation, const std::vector<double> &exposures);
    //! Append data to aggregator
    void append(const double *);
    //! Force flush data to disk
    void flush();
    //! Return file name
    const std::string &getFilename() const { return mFilename; }

};

} // namespace
