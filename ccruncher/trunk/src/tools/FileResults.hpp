
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
// FileResults.cpp - FileResults header
// --------------------------------------------------------------------------
//
// 2005/01/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _FileResults_
#define _FileResults_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <map>
#include <vector>
#include "utils/Date.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

#define MAXLINESIZE 32768

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class FileResults
{

  private:

    int ncolumns;
    char buffer[MAXLINESIZE];

    map<string,string> info;
    vector<double> *data;

    void parseFile(string &filename) throw(Exception);
    void parseLine(char *line) throw(Exception);

    char * ltrim(char *tira);
    bool isComment(char *tira);
    bool isMapValue(char *tira);
    string extractKey(char *tira) throw(Exception);
    string extractValue(char *tira) throw(Exception);
    void extractData(char *tira) throw(Exception);
    char* parseDouble(char *tira, double *val) throw(Exception);

  public:

    FileResults(string &) throw(Exception);
    ~FileResults();

    int getNumColumns() throw(Exception);
    string getInfo(string &) throw(Exception);
    vector<double>* getColumn(int) throw(Exception);
    vector<double>* getColumns();
    Date getDate(int icol) throw(Exception);

    double getXMin();
    double getXMax();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
