
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

#ifndef _Aggregator_
#define _Aggregator_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "kernel/SimulatedData.hpp"
#include "utils/Exception.hpp"
#include "utils/Timer.hpp"
#include "segmentations/Segmentation.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Aggregator
{

  private:

    // segmentation object
    Segmentation &segmentation;
    // output file stream
    ofstream fout;
    // number of segments
    int numsegments;
    // number of simulations
    int cont;
    // buffer counter
    int icont;
    // buffer of simulated values (size = numsegments x buffersize)
    vector<double> buffer;
    // number of rows in buffer
    int bufferrows;
    // indicates if rest segment is used
    bool printRestSegment;
    // internal timer (control time from last flush)
    Timer timer;

  public:

    // constructor
    Aggregator(vector<SimulatedAsset> &, int, Segmentation &, const string &, bool) throw(Exception);
    // destructor
    ~Aggregator();
    // append data to aggregator
    bool append(vector<double> &) throw(Exception);
    // flush data to disk
    bool flush() throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
