
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#ifndef _CsvFile_
#define _CsvFile_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include <fstream>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class CsvFile
{

  private:

    // filed separator/s
    string separators;
    // file name
    string filename;
    // stream
    ifstream file;
    // column headers
    vector<string> headers;
    // buffer
    char buffer[256];

  private:

    // parse a field
    int read() throw(Exception);
    // parse a double
    static double parse(const char *) throw(Exception);
    // trim a string
    static char* trim(char *);

  public:

    // constructor
    CsvFile(const string &fname="", const string sep=",") throw(Exception);
    // open a file
    void open(const string &fname, const string sep=",") throw(Exception);
    // returns headers
    const vector<string> getHeaders() const;
    // returns column values
    void getValues(size_t col, vector<double> &ret) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
