
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

#ifndef _Aggregator_
#define _Aggregator_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include <fstream>
#include "utils/Exception.hpp"
#include "params/Segmentation.hpp"
#include "params/Segmentations.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Aggregator
{

  private:

    // segmentation name
    std::string filename;
    // output file stream
    std::ofstream fout;
    // number of segments
    int numsegments;

  private:

    // non-copyable class
    Aggregator(const Aggregator &);
    // non-copyable class
    Aggregator & operator=(const Aggregator &);

  public:

    // constructor
    Aggregator(int , const Segmentations &, const std::string &, char) throw(Exception);
    // destructor
    ~Aggregator();
    // append data to aggregator
    void append(const double *) throw(Exception);
    // force flush data to disk
    void flush() throw(Exception);
    // return the number of segments
    int size() const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
