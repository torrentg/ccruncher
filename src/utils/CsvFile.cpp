
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

using namespace std;

#define BUFFER_SIZE 128*1024
#define MAX_FIELD_SIZE 40

//===========================================================================
// default constructor
//===========================================================================
ccruncher::CsvFile::CsvFile(const string &fname, const string &sep) throw(Exception)
    : filename(""), file(NULL), filesize(0)
{
  if (fname != "") {
    open(fname, sep);
  }
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::CsvFile::~CsvFile()
{
  close();
}

//===========================================================================
// returns file size
//===========================================================================
size_t ccruncher::CsvFile::getNumBytes()
{
  if (file == NULL) return 0;
  long pos0 = ftell(file);
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, pos0, SEEK_SET);
  return size;
}

//===========================================================================
// open a file
//===========================================================================
void ccruncher::CsvFile::open(const string &fname, const string &sep) throw(Exception)
{
  close();

  filename = fname;
  separators = sep;

  file = fopen(fname.c_str(), "r");
  if (file == NULL) {
    throw Exception("error opening file " + fname);
  }

  ptr0 = NULL;
  ptr1 = NULL;
  buffer[0] = 0;
  filesize = getNumBytes();
  //TODO: move getNumBytes() content here?
}

//===========================================================================
// close file
//===========================================================================
void ccruncher::CsvFile::close()
{
  if (file != NULL) {
    fclose(file);
    file = NULL;
  }
  headers.clear();
  ptr0 = NULL;
  ptr1 = NULL;
  buffer[0] = 0;
  filesize = 0;
}

//===========================================================================
// returns headers
//===========================================================================
const vector<string>& ccruncher::CsvFile::getHeaders()
{
  if (file != NULL && !headers.empty()) return headers;

  headers.clear();
  open(filename, separators);

  int rc;
  do
  {
    rc = read();
    char *str = trim(ptr0);
    if (*str == '"') str++;
    size_t l = strlen(str);
    if (l>0 && *(str+l-1)=='"') *(str+l-1) = 0;
    //str = trim(str);
    headers.push_back(string(str));
  }
  while(rc == 1);

  return headers;
}

//===========================================================================
// moves content pointed by ptr to the begining of buffer
// and appends content to end
// returns the number of available chars in buffer
//===========================================================================
size_t ccruncher::CsvFile::getChunk(char *ptr)
{
  size_t len = 0;
  char *aux = NULL;

  // move data pointed by ptr to the begining of buffer
  if (ptr != NULL) {
    assert(buffer <= ptr && ptr < buffer+BUFFER_SIZE);
    len = buffer + BUFFER_SIZE - ptr;
    memcpy(buffer, ptr, len);
    aux = buffer + len;
  }
  else {
    aux = buffer;
  }

  // initializing pointers
  ptr0 = buffer;
  ptr1 = buffer;

  // read data from file preserving unreaded content
  len = buffer + BUFFER_SIZE - aux;
  size_t rc = fread(aux, sizeof(char), len, file);

  if (rc < len) {
    aux[rc] = 0;
    return (aux+rc) - buffer;
  }
  else {
    return BUFFER_SIZE;
  }
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
  if (ptr1 == NULL) getChunk(NULL);
  ptr0 = ptr1;
  assert(buffer <= ptr0 && ptr0 < buffer+BUFFER_SIZE);
  if (ptr0 == 0) return 3;

  do
  {
    if (buffer+BUFFER_SIZE-1 <= ptr1) {
      getChunk(ptr0);
    }
    else if (*ptr1 == 0) {
      return 3;
    }
    else if (*ptr1 == '\n') {
      *ptr1 = 0;
      ptr1++;
      return 2;
    }
    else if (*ptr1 == ',') { //strchr(separators.c_str(), buffer[pos]) != NULL
      *ptr1 = 0;
      ptr1++;
      return 1;
    }
    else if (ptr1-ptr0 > MAX_FIELD_SIZE) {
      throw Exception("field too long");
    }

    ptr1++;
    assert(buffer <= ptr1 && ptr1 <= buffer+BUFFER_SIZE);
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
  if (*ret == 0) return ret;
  char *aux = ret;
  aux += strlen(aux)-1;
  while (aux >= ret && isspace(*aux)) aux--;
  *(aux+1) = 0;
  return ret;
}

//===========================================================================
// returns column values
//===========================================================================
void ccruncher::CsvFile::getColumn(int col, vector<double> &ret, bool *stop) throw(Exception)
{
  if (col < 0) {
    return getRowSums(ret, stop);
  }

  int rc;
  size_t numlines = 0;

  ret.clear();
  open(filename, separators);

  try
  {
    // skip headers
    while(read() == 1);
    numlines++;

    // read lines
    do
    {
      // read first field
      rc = read();
      if (rc != 1) {
        char *ptr = trim(ptr0);
        if (*ptr == 0) {
          // skip blank line
          numlines++;
          continue;
        }
        if (col > 0) {
          throw Exception("value not found");
        }
      }

      // read previous fields
      for(int i=1; i<=col; i++)
      {
        rc = read();
        if (rc != 1 && i<col) {
          throw Exception("line with invalid format");
        }
      }

      // read col-th field
      double val = parse(trim(ptr0));
      ret.push_back(val);

      // reading the rest of fields
      while (rc == 1) rc = read();
      numlines++;

      // check stop flag
      if (stop != NULL && *stop) break;
    }
    while(rc != 3);
  }
  catch(Exception &e)
  {
    throw Exception(e.toString() + " at line " + Format::toString(numlines+1));
  }
}

//===========================================================================
// return row sums
//===========================================================================
void ccruncher::CsvFile::getRowSums(vector<double> &ret, bool *stop) throw(Exception)
{
  int rc;
  size_t numlines = 0;
  int numcols = 1;
  int curcol = 0;

  ret.clear();
  open(filename, separators);

  try
  {
    // skip headers
    while(read() == 1) numcols++;
    numlines++;

    // read lines
    do
    {
      // read first field
      curcol = -1;
      rc = read();
      curcol = 0;
      if (rc != 1) {
        char *ptr = trim(ptr0);
        if (*ptr == 0) {
          // skip blank line
          numlines++;
          continue;
        }
      }
      double val = parse(trim(ptr0));

      // reading the rest of fields
      while (rc == 1) {
        rc = read();
        curcol++;
        val += parse(trim(ptr0));
      }
      if (curcol+1 != numcols) {
        throw Exception("invalid file format");
      }
      ret.push_back(val);
      numlines++;

      // check stop flag
      if (stop != NULL && *stop) break;
    }
    while(rc != 3);
  }
  catch(Exception &e)
  {
    throw Exception(e.toString() + " at line " + Format::toString(numlines+1));
  }
}

//===========================================================================
// returns file content
//===========================================================================
void ccruncher::CsvFile::getColumns(vector<vector<double> > &ret, bool *stop) throw(Exception)
{
  int rc;
  size_t numlines = 0;
  int numcols = 1;
  int curcol = 0;

  ret.clear();
  open(filename, separators);

  try
  {
    // skip headers
    while(read() == 1) numcols++;
    numlines++;

    ret.resize(numcols);

    // read lines
    do
    {
      // read first field
      curcol = -1;
      rc = read();
      curcol = 0;
      if (rc != 1) {
        char *ptr = trim(ptr0);
        if (*ptr == 0) {
          // skip blank line
          numlines++;
          continue;
        }
      }
      ret[curcol].push_back(parse(trim(ptr0)));

      // reading the rest of fields
      while (rc == 1) {
        rc = read();
        curcol++;
        if (curcol >= numcols) {
          throw Exception("unexpected value");
        }
        ret[curcol].push_back(parse(trim(ptr0)));
      }

      if (curcol+1 != numcols) {
        throw Exception("expected value not found");
      }

      numlines++;

      // check stop flag
      if (stop != NULL && *stop) break;
    }
    while(rc != 3);
  }
  catch(Exception &e)
  {
    // remove last line
    if (curcol+1 != numcols && ret.size() > 0) {
      for(int i=0; i<curcol; i++) {
        if (ret[i].size() > 0) {
          ret[i].erase(ret[i].end()-1);
        }
      }
    }
    throw Exception(e.toString() + " at line " + Format::toString(numlines+1));
  }
}

//===========================================================================
// returns file size (in bytes)
//===========================================================================
size_t ccruncher::CsvFile::getFileSize() const
{
  return filesize;
}

//===========================================================================
// returns readed bytes
//===========================================================================
size_t ccruncher::CsvFile::getReadedSize() const
{
  if (file == NULL) return 0;
  else return ftell(file);
}

//===========================================================================
// returns the number of lines
//===========================================================================
size_t ccruncher::CsvFile::getNumLines()
{
  size_t numlines = 0;
  size_t rc = 0;

  open(filename, separators);

  do
  {
    rc = getChunk(NULL);
    for(size_t i=0; i<rc; i++) {
      if (buffer[i] == '\n') {
        numlines++;
      }
    }
  }
  while(rc == BUFFER_SIZE);

  numlines++;
  return numlines;
}

