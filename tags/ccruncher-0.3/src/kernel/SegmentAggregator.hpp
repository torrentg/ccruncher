
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
// SegmentAggregator.hpp - SegmentAggregator header
// --------------------------------------------------------------------------
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
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
#include "portfolio/Client.hpp"
#include "portfolio/DateValues.hpp"
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

    // segmentation index
    int isegmentation;
    // segment name
    string name2;
    // segment index
    int isegment;
    // type of components (assets/clients) of segmentation
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

    // number of all clients considered
    long N;
    // number of time tranches
    int M;
    // num clients belonging this segment
    long nclients;
    // num asset belonging this segment
    long nassets;
    // number of realizations
    long cont;
    // buffer counter
    long icont;

    // related clients position index (size = nclients)
    long *iclients;
    // vertexes of afected clients (size = nclients x M)
    DateValues **vertexes;
    // vertex values aggregation per tranch (size = buffersize)
    double *cvalues; 

    // memory management
    void init();
    void release();

    // internal functions
    long getANumClients(vector<Client *> *, long, bool *);
    long getCNumClients(vector<Client *> *, long, bool *);
    long getANumAssets(vector<Client *> *, long, bool *);
    long getCNumAssets(vector<Client *> *, long, bool *);
    long* allocIClients(long, bool *, long) throw(Exception);
    DateValues** allocVertexes(Date *, int, vector<Client *> *, Interests *) throw(Exception);

    // output functions
    string getFilePath() throw(Exception);
    void ofsopen() throw(Exception);
    void ofsclose() throw(Exception);

  public:

    SegmentAggregator();
    ~SegmentAggregator();

    // initialization methods
    void define(int, int, components_t);
    void setOutputProperties(const string &path, const string &filename, bool force, int buffersize) throw(Exception);
    void initialize(Date *, int, vector<Client *> *, long, Interests *) throw(Exception);
    
    // other methods
    long getNumElements();
    void append(int *defaulttimes) throw(Exception);
    void flush() throw(Exception);
    void touch() throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
