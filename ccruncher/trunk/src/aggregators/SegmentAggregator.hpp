
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
// SegmentAggregator.hpp - SegmentAggregator header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _SegmentAggregator_
#define _SegmentAggregator_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/File.hpp"
#include "finances/DateValues.hpp"
#include "ratings/DateRatings.hpp"
#include "ratings/Ratings.hpp"
#include "portfolio/Client.hpp"
#include "segmentations/Segmentations.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

#define MAXSIZE 500

//---------------------------------------------------------------------------

class SegmentAggregator
{

  private:

    // output file stream
    ofstream fout;

    // aggregator name
    string name0;
    // segmentation name
    string name1;
    // segment name
    string name2;
    // ratings reference (used to print ratings names)
    Ratings *ratings;

    int isegmentation;
    int isegment;
    components_t components;
    bool bvalues;
    bool bfull;

    // output file path
    string path;
    // force file overwriting (if exists)
    bool bforce;
    // buffer size
    int buffersize;
    // header set flag
    bool existheader;

    // number of all clients considered
    long N;
    // number of time tranches
    int M;
    // buffer size
    long K;

    // num clients belonging this segment (components=client) or having an asset
    // belonging this segment (components=asset)
    long nclients;
    // num asset belonging this segment
    long nassets;
    // number of realizations
    long cont;
    // internal number of realizations
    long icont;
    // index of default rating
    int idefault;

    // related clients position index (size = nclients)
    long *iclients;

    // vertexes of afected clients & assets (size = nclients)
    DateValues **vertexes;

    // vertex values aggregation per tranch (only if bvalues=true)
    DateValues **cvalues; // size=1 (if bfull=false), size=MAXSIZE (if bfull=true)

    // ratings counter per tranch (only if bvalues=false)
    DateRatings **cratings; // size=1 (if bfull=false), size=MAXSIZE (if bfull=true)

    // memory management
    void reset();
    void release();

    // internal functions
    long getANumClients(vector<Client *> *, long, bool *);
    long getCNumClients(vector<Client *> *, long, bool *);
    long getANumAssets(vector<Client *> *, long, bool *);
    long getCNumAssets(vector<Client *> *, long, bool *);
    bool* allocClientFlag(long n) throw(Exception);
    long* allocIClients(bool *, long) throw(Exception);
    DateValues** allocCValues(Date *, int, long) throw(Exception);
    DateRatings** allocCRatings(Date *, int, long, int) throw(Exception);
    DateValues** allocVertexes(Date *, int, vector<Client *> *, long, Interests *) throw(Exception);
    void appendA(int **rpaths);
    void appendR(int **rpaths);
    void resetCValue(int);
    void resetCRating(int);

    void printHeader() throw(Exception);
    void printFooter() throw(Exception);
    void printItems(bool finalize) throw(Exception);
    void printItem(int) throw(Exception);
//    void printTranch(int, int) throw(Exception);

    string getFilePath() throw(Exception);
    void ofsopen() throw(Exception);
    void ofsclose() throw(Exception);

  public:

    SegmentAggregator();
    ~SegmentAggregator();

    void define(int, int, components_t, bool , bool);
    void setOutputProperties(string path, bool force, int buffersize) throw(Exception);
    void setNames(string, string, string) throw(Exception);
    void initialize(Date *, int, vector<Client *> *, long, int, Ratings *, Interests *) throw(Exception);
    void append(int **rpaths, int, long, vector<Client *> *) throw(Exception);
    void next();
    void flush(bool finalize) throw(Exception);
    void touch() throw(Exception);
    void finalize() throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
