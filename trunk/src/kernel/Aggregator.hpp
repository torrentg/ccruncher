
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

#ifndef _Aggregator_
#define _Aggregator_

#include <string>
#include <vector>
#include <fstream>
#include "params/Segmentations.hpp"
#include "params/Segmentation.hpp"
#include "utils/Exception.hpp"

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

    //! Segmentation name
    std::string filename;
    //! Output file stream
    std::ofstream fout;
    //! Number of segments
    int numsegments;

  private:

    //! Non-copyable class
    Aggregator(const Aggregator &);
    //! Non-copyable class
    Aggregator & operator=(const Aggregator &);

  public:

    //! Constructor
    Aggregator(int , const Segmentations &, const std::string &, char) throw(Exception);
    //! Destructor
    ~Aggregator();
    //! Append data to aggregator
    void append(const double *) throw(Exception);
    //! Force flush data to disk
    void flush() throw(Exception);
    //! Return the number of segments
    int size() const;

};

} // namespace

#endif

