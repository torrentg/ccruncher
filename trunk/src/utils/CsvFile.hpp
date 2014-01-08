
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

#ifndef _CsvFile_
#define _CsvFile_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class CsvFile
{

  private:

    // filed separator/s
    std::string separators;
    // file name
    std::string filename;
    // stream
    std::FILE *file;
    // column headers
    std::vector<std::string> headers;
    // buffer (128KB)
    char buffer[128*1024];
    // current position in buffer
    char *ptr0;
    // next position in buffer
    char *ptr1;
    // file size (in bytes)
    size_t filesize;

  private:

    // returns file size
    size_t getNumBytes();
    // fills buffer
    size_t getChunk(char *ptr);
    // parse a field
    int read() throw(Exception);
    // returns row sums
    void getRowSums(std::vector<double> &ret, bool *stop=NULL) throw(Exception);
    // parse a double
    static double parse(const char *) throw(Exception);
    // trim a string
    static char* trim(char *);

  public:

    // constructor
    CsvFile(const std::string &fname="", const std::string &sep=",") throw(Exception);
    // destructor
    ~CsvFile();
    // open a file
    void open(const std::string &fname, const std::string &sep=",") throw(Exception);
    // close file
    void close();
    // returns headers
    const std::vector<std::string>& getHeaders();
    // returns column values (if col<0 returns rowSum)
    void getColumn(int col, std::vector<double> &ret, bool *stop=NULL) throw(Exception);
    // returns file values
    void getColumns(std::vector<std::vector<double> > &ret, bool *stop=NULL) throw(Exception);
    // returns file size (in bytes)
    size_t getFileSize() const;
    // returns readed bytes
    size_t getReadedSize() const;
    // returns the number of lines
    size_t getNumLines();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
