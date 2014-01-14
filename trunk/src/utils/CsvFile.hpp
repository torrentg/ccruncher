
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

#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief   Object to retrieve data from CSV a file.
 *
 * @details This class can open, get column headers, and read data by
 *          columns or as a whole from a CSV file. It is designed to deal
 *          with large files. Provides a way to stop a long data read
 *          changing a variable value and to retrieve the number of
 *          parsed lines (to compute the parsing progress). Observe
 *          that CSV can file simultaneously writed by another
 *          process/thread.
 *
 * @see http://en.wikipedia.org/wiki/Comma-separated_values
 */
class CsvFile
{

  private:

    //! File name
    std::string filename;
    //! File object
    std::FILE *file;
    //! Column headers
    std::vector<std::string> headers;
    //! Buffer (128KB)
    char buffer[128*1024];
    //! Current position in buffer
    char *ptr0;
    //! Next position in buffer
    char *ptr1;
    //! File size (in bytes)
    size_t filesize;

  private:

    //! Fills buffer
    size_t getChunk(char *ptr);
    //! Parse a field
    int next() throw(Exception);
    //! Returns row sums
    void getRowSums(std::vector<double> &ret, bool *stop=NULL) throw(Exception);
    //! Parse a double
    static double parse(const char *) throw(Exception);
    //! Trim a string
    static char* trim(char *);

  public:

    //! Constructor
    CsvFile(const std::string &fname="") throw(Exception);
    //! Destructor
    ~CsvFile();
    //! Open file
    void open(const std::string &fname) throw(Exception);
    //! Close file
    void close();
    //! Returns headers
    const std::vector<std::string>& getHeaders();
    //! Returns column values
    void getColumn(int col, std::vector<double> &ret, bool *stop=NULL) throw(Exception);
    //! Returns file values
    void getColumns(std::vector<std::vector<double> > &ret, bool *stop=NULL) throw(Exception);
    //! Returns file size (in bytes)
    size_t getFileSize() const;
    //! Returns readed bytes
    size_t getReadedSize() const;
    //! Returns the number of lines
    size_t getNumLines() throw(Exception);

};

} // namespace

#endif

