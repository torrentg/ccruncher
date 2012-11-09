
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

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cerrno>
#include "utils/CsvFile.hpp"
#include "utils/Format.hpp"
#include <cassert>

//===========================================================================
// default constructor
//===========================================================================
ccruncher::CsvFile::CsvFile(const string &fname, const string sep) throw(Exception)
  : filename("")
{
  if (fname != "") {
      open(fname, sep);
  }
}

//===========================================================================
// open a file
//===========================================================================
void ccruncher::CsvFile::open(const string &fname, const string sep) throw(Exception)
{
  filename = fname;
  separators = sep;

  // resseting content
  if (file.is_open()) file.close();
  headers.clear();

  // opening file
  file.open(fname.c_str(), std::ios_base::in);
  if (!file.is_open()) {
    throw Exception("error opening file " + fname);
  }
  //file.exceptions(ios::failbit | ios::badbit);

  // parsing headers
  int rc;
  do
  {
    rc = read();
    char *str = trim(buffer);
    if (*str == '"') str++;
    size_t l = strlen(str);
    if (l>0 && *(str+l-1)=='"') *(str+l-1) = 0;
    str = trim(str);
    headers.push_back(string(str));
  }
  while(rc == 1);
}

//===========================================================================
// returns headers
//===========================================================================
const vector<string> ccruncher::CsvFile::getHeaders() const
{
  return headers;
}

//===========================================================================
// read a field to buffer
// return value is the right-hand character
//   1: field separator
//   2: new line
//   3: end-of-file
//===========================================================================
int ccruncher::CsvFile::read() throw(Exception)
{
  size_t pos = 0;

  do
  {
    file.get(buffer[pos]);

    if (file.eof()) {
      buffer[pos] = 0;
      return 3;
    }
    else if (buffer[pos] == '\n') {
      buffer[pos] = 0;
      return 2;
    }
    else if (strchr(separators.c_str(), buffer[pos]) != NULL)
    {
      buffer[pos] = 0;
      return 1;
    }
    else if (pos >= sizeof(buffer)-1) {
      throw Exception("field to long");
    }
    else {
      pos++;
    }
  }
  while(true);
}

//===========================================================================
// parse a value
//===========================================================================
double ccruncher::CsvFile::parse(const char *str) throw(Exception)
{
  char *endptr;
  errno = 0;
  double val = strtod(str, &endptr);
  if (errno != 0 || str == endptr || *endptr != 0) {
    throw Exception("invalid value");
  }
  else {
    return val;
  }
}

//===========================================================================
// trim a string
//===========================================================================
char* ccruncher::CsvFile::trim(char *ptr)
{
  if (ptr == NULL) return ptr;
  char *ret = ptr;
  while (isspace(*ret)) ret++;
  char *aux = ret;
  while (!isspace(*aux) && *aux!=0) aux++;
  *aux = 0;
  return ret;
}

//===========================================================================
// returns column values
//===========================================================================
void ccruncher::CsvFile::getValues(size_t col, vector<double> &ret) throw(Exception)
{
  ret.clear();

  if (col >= headers.size()) {
    throw Exception("invalid column index");
  }

  int rc;
  size_t numlines = 0;

  // rewind file
  file.clear();
  file.seekg(0, ios::beg);

  // skip headers
  while(read() == 1);
  numlines++;

  // read lines
  do
  {
    // read first field
    rc = read();
    if (rc != 1) {
      char *ptr = trim(buffer);
      if (*ptr == 0) {
        // skip blank line
        numlines++;
        continue;
      }
    }

    // read previous fields
    for(size_t i=1; i<=col; i++)
    {
      rc = read();
      if (rc != 1 && i<col) {
        throw Exception("line with invalid format");
      }
    }

    // read col-th field
    double val = parse(trim(buffer));
    ret.push_back(val);

    // reading the rest of fields
    while (rc == 1) rc = read();
    numlines++;
  }
  while(rc != 3);
}

