
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
//
// SegmentAggregator.hpp - SegmentAggregator header - $Rev$
// --------------------------------------------------------------------------
//
// 2005/05/20 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/07/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . added timer to control last flush time
//
// 2005/08/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . added segmentaggregator identifier support
//   . added appendRawData() method (used in MPI mode)
//
// 2005/09/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . added param montecarlo.simule
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . SegmentAggregator refactoring
//
// 2006/01/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed simule and method params
//
// 2007/07/31 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed interests arguments
//
// 2007/08/03 - Gerard Torrent [gerard@mail.generacio.com]
//   . Client class renamed to Borrower
//
//===========================================================================

#ifndef _SegmentAggregator_
#define _SegmentAggregator_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/Timer.hpp"
#include "portfolio/Borrower.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class SegmentAggregator
{

  private:

    // segmentaggregator identifier
    int iaggregator;
    // segmentation index
    int isegmentation;
    // segment name
    string name2;
    // segment index
    int isegment;
    // type of components (assets/borrowers) of segmentation
    components_t components;

    // output file stream
    ofstream fout;
    // file name
    string filename;
    // output file path
    string path;
    // force file overwriting (if exists)
    bool bforce;
    // buffer size
    int buffersize;

    // number of all borrowers considered
    long N;
    // number of time tranches
    int M;
    // num borrowers belonging this segment
    long nborrowers;
    // num asset belonging this segment
    long nassets;
    // number of realizations
    long cont;
    // buffer counter
    long icont;

    // related borrowers position index (size = nborrowers)
    vector<long> iborrowers;
    // vertexes of afected borrowers (size = nborrowers x M)
    vector<vector<double> > losses;
    // vertex values aggregation per tranch (size = buffersize)
    vector<double> cvalues;

    // internal timer (control time from last flush)
    Timer timer;

    // memory management
    void init();

    // internal functions
    long getANumBorrowers(vector<Borrower *> &, long, bool *);
    long getCNumBorrowers(vector<Borrower *> &, long, bool *);
    long getANumAssets(vector<Borrower *> &, long, bool *);
    long getCNumAssets(vector<Borrower *> &, long, bool *);
    void allocIBorrowers(long, bool *, long) throw(Exception);
    void allocLosses(vector<Date> &, vector<Borrower *> &) throw(Exception);

    // output functions
    string getFilePath() throw(Exception);
    void ofsopen() throw(Exception);
    void ofsclose() throw(Exception);

  public:

    // constructors & destructors
    SegmentAggregator();
    ~SegmentAggregator();

    // initialization methods
    void define(int, int, int, components_t);
    void setOutputProperties(const string &path, const string &filename, bool force, int buffersize) throw(Exception);
    void initialize(vector<Date> &, vector<Borrower *> &, long) throw(Exception);

    // other methods
    long getNumElements() const;
    bool append(int *defaulttimes) throw(Exception);
    bool appendRawData(double *data, int datasize) throw(Exception);
    bool flush() throw(Exception);
    void touch() throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
