
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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
#include <cassert>
#include "utils/CsvFile.hpp"

using namespace std;

#define BUFFER_SIZE 128*1024
#define MAX_FIELD_SIZE 40
#define FIELD_SEPARATOR ','
#define COMMENT_CHAR '#'

/**************************************************************************//**
 * @param[in] fname CSV file path with read permission.
 * @throw Exception Error opening file.
 */
ccruncher::CsvFile::CsvFile(const string &fname)
    : filename(""), file(nullptr), filesize(0)
{
  if (fname != "") {
    open(fname);
  }
}

/**************************************************************************/
ccruncher::CsvFile::~CsvFile()
{
  close();
}

/**************************************************************************//**
 * @details Close previous file (if exist) and opens the given one.
 * @param[in] fname CSV file path with read permission.
 * @throw Exception Error opening file.
 */
void ccruncher::CsvFile::open(const string &fname)
{
  close();

  filename = fname;

  file = fopen(fname.c_str(), "r");
  if (file == nullptr) {
    throw Exception("error opening file " + fname);
  }

  ptr0 = nullptr;
  ptr1 = nullptr;
  buffer[0] = 0;

  // get current file size
  long pos0 = ftell(file);
  fseek(file, 0, SEEK_END);
  filesize = ftell(file);
  fseek(file, pos0, SEEK_SET);
}

/**************************************************************************/
void ccruncher::CsvFile::close()
{
  if (file != nullptr) {
    fclose(file);
    file = nullptr;
  }
  headers.clear();
  ptr0 = nullptr;
  ptr1 = nullptr;
  buffer[0] = 0;
  filesize = 0;
}

/**************************************************************************//**
 * @return List of columns headers.
 */
const vector<string>& ccruncher::CsvFile::getHeaders()
{
  if (file != nullptr && !headers.empty()) return headers;

  headers.clear();
  open(filename);

  int rc;
  do
  {
    rc = next();
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

/**************************************************************************//**
 * @details Internal method.
 *          Moves content pointed by ptr to the begining of buffer and
 *          appends new file data just to complete the buffer capacity.
 *          Returns the number of available chars in buffer.
 * @param[in] ptr Current position in buffer.
 * @return Number of available bytes in buffer.
 */
size_t ccruncher::CsvFile::getChunk(char *ptr)
{
  size_t len = 0;
  char *aux = nullptr;

  // move data pointed by ptr to the begining of buffer
  if (ptr != nullptr) {
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

/**************************************************************************//**
 * @details Move ptr0 and ptr1 to the next uncommented line.
 *          Comments starts by COMMENT_CHAR and ends at line-end.
 */
void ccruncher::CsvFile::skipComments()
{
  while (*ptr0 == COMMENT_CHAR)
  {
    while(*ptr0 != '\n' && *ptr0 != 0)
    {
      if (buffer+BUFFER_SIZE-1 <= ptr0) {
        getChunk(ptr0);
      }
      ptr0++;
    }
    if (*ptr0 != 0) {
      if (buffer+BUFFER_SIZE-1 <= ptr0) {
        getChunk(ptr0);
      }
      ptr0++;
    }
  }
  ptr1 = ptr0;
}

/**************************************************************************//**
 * @details Internal method.
 *          Parse next token in buffer. Returns the type of token found:
 *          - 1: field separator
 *          - 2: new line
 *          - 3: end-of-file
 *          ptr0 points to the identified token.
 * @return Type of token (1=sep, 2=eol, 3=eof)
 * @throw Exception Field length exceeds max size.
 */
int ccruncher::CsvFile::next()
{
  // first line
  if (ptr1 == nullptr) {
    getChunk(nullptr);
  }
  ptr0 = ptr1;
  assert(buffer <= ptr0 && ptr0 < buffer+BUFFER_SIZE);

  if (*ptr0 == COMMENT_CHAR) {
    skipComments();
  }
  if (*ptr0 == 0) return 3;

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
    else if (*ptr1 == FIELD_SEPARATOR) { //strchr(separators.c_str(), buffer[pos]) != nullptr
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

/**************************************************************************//**
 * @param[in] str String containing a number.
 * @return Number as floating point value.
 * @throw Exception Invalid number.
 */
double ccruncher::CsvFile::parse(const char *str)
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

/**************************************************************************//**
 * @details Move str pointer until a non-space is found. Removes ending
 *          spaces setting a 'o' just after the first ending non-space.
 *          This method can modify ptr content.
 * @param[in,out] ptr String to trim spaces.
 * @return Pointer to first non-space char.
 */
char* ccruncher::CsvFile::trim(char *ptr)
{
  if (ptr == nullptr) return ptr;
  char *ret = ptr;
  while (isspace(*ret)) ret++;
  if (*ret == 0) return ret;
  char *aux = ret;
  aux += strlen(aux)-1;
  while (aux >= ret && isspace(*aux)) aux--;
  *(aux+1) = 0;
  return ret;
}

/**************************************************************************//**
 * @details Returns the column value of col-th column. If col<0 then is
 *          equivalent to CsvFile::getRowSums(). User can stop current
 *          parsing modifying the variable stop.
 * @param[in] col Column index (0-based).
 * @param[out] ret Data readed.
 * @param[in] stop Stop flag.
 * @throw Exception Parser error.
 */
void ccruncher::CsvFile::getColumn(int col, vector<double> &ret, bool *stop)
{
  if (col < 0) {
    return getRowSums(ret, stop);
  }

  int rc;
  size_t numlines = 0;

  ret.clear();
  open(filename);

  try
  {
    // skip headers
    while(next() == 1);
    numlines++;

    // read lines
    do
    {
      // read first field
      rc = next();
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
        rc = next();
        if (rc != 1 && i<col) {
          throw Exception("line with invalid format");
        }
      }

      // read col-th field
      double val = parse(trim(ptr0));
      ret.push_back(val);

      // reading the rest of fields
      while (rc == 1) rc = next();
      numlines++;

      // check stop flag
      if (stop != nullptr && *stop) break;
    }
    while(rc != 3);
  }
  catch(Exception &e)
  {
    throw Exception(e.toString() + " at line " + to_string(numlines+1));
  }
}

/**************************************************************************//**
 * @details Compute column sums across rows. User can stop current parsing
 *          modifying the variable stop.
 * @param[out] ret Data readed.
 * @param[in] stop Stop flag.
 * @throw Exception Parser error.
 */
void ccruncher::CsvFile::getRowSums(vector<double> &ret, bool *stop)
{
  int rc;
  size_t numlines = 0;
  int numcols = 1;
  int curcol = 0;

  ret.clear();
  open(filename);

  try
  {
    // skip headers
    while(next() == 1) numcols++;
    numlines++;

    // read lines
    do
    {
      // read first field
      rc = next();
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
        rc = next();
        curcol++;
        val += parse(trim(ptr0));
      }
      if (curcol+1 != numcols) {
        throw Exception("invalid file format");
      }
      ret.push_back(val);
      numlines++;

      // check stop flag
      if (stop != nullptr && *stop) break;
    }
    while(rc != 3);
  }
  catch(Exception &e)
  {
    throw Exception(e.toString() + " at line " + to_string(numlines+1));
  }
}

/**************************************************************************//**
 * @details Read CSV table values. User can stop current parsing
 *          modifying the variable stop.
 * @param[out] ret Data readed.
 * @param[in] stop Stop flag.
 * @throw Exception Parser error.
 */
void ccruncher::CsvFile::getColumns(vector<vector<double>> &ret, bool *stop)
{
  int rc;
  size_t numlines = 0;
  int numcols = 1;
  int curcol = 0;

  ret.clear();
  open(filename);

  try
  {
    // skip headers
    while(next() == 1) numcols++;
    numlines++;

    ret.resize(numcols);

    // read lines
    do
    {
      // read first field
      rc = next();
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
        rc = next();
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
      if (stop != nullptr && *stop) break;
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
    throw Exception(e.toString() + " at line " + to_string(numlines+1));
  }
}

/**************************************************************************/
size_t ccruncher::CsvFile::getFileSize() const
{
  return filesize;
}

/**************************************************************************//**
 * @details This method is useful if a thread is reading the CSV file and
 *          another thread queries about the number of readed bytes to
 *          display the parsing progress.
 * @return Readed bytes.
 */
size_t ccruncher::CsvFile::getReadedSize() const
{
  if (file == nullptr) return 0;
  else return ftell(file);
}

/**************************************************************************//**
 * @details Count the number of eol in the file.
 * @return Number of lines
 * @throw Exception If file can not be readed.
 */
size_t ccruncher::CsvFile::getNumLines()
{
  size_t numlines = 0;
  size_t rc = 0;

  open(filename);

  do
  {
    rc = getChunk(nullptr);
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

