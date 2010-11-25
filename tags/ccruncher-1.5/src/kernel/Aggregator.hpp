
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
#include "utils/Date.hpp"
#include "utils/Timer.hpp"
#include "portfolio/Borrower.hpp"
#include "segmentations/Segmentation.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Aggregator
{

  private:

    // segmentation index
    int isegmentation;
    // segmentation object
    Segmentation &segmentation;
    // segment by each simulated asset
    vector<int> isegments;
    // output file stream
    ofstream fout;
    // number of borrowers considered
    long numborrowers;
    // number of segments
    long numsegments;
    // number of simulations
    long cont;
    // buffer counter
    long icont;
    // cumulated values (size = numsegments x buffersize)
    vector<double> cvalues;
    // number of rows in buffer (1 row = numsegments values)
    int bufferrows;
    // indicates if rest segment has borrowers/assets
    bool printRestSegment;
    // internal timer (control time from last flush)
    Timer timer;

  public:

    // constructor
    Aggregator(int, Segmentation&, vector<SimulatedAsset> &);
    // destructor
    ~Aggregator();
    // set properties
    void setOutputProperties(const string &filename, bool force) throw(Exception);
    // append data to aggregator
    bool append(vector<SimulatedAsset> &assets) throw(Exception);
    // append raw data
    long appendRawData(double *data, int datasize) throw(Exception);
    // flush data to disk
    bool flush() throw(Exception);
    // returns buffer size
    long getBufferSize();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------