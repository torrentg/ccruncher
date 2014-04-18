
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

#include <cstring>
#include <cassert>
#include "utils/ExpatUserData.hpp"

#define MAX_STACK_SIZE 10

using namespace std;
using namespace ccruncher;

/**************************************************************************/
ccruncher::ExpatUserData::ExpatUserData(size_t buffersize) :
  pila(MAX_STACK_SIZE), pila_pos(-1), current_tag(nullptr), buffer(nullptr),
  buffer_size(buffersize), buffer_pos1(nullptr), buffer_pos2(nullptr),
  date1(nullptr), date2(nullptr), interest(nullptr), ratings(nullptr),
  factors(nullptr), segmentations(nullptr)
{
  if (buffer_size == 0) buffer_size = 1;
  buffer = new char[buffer_size];
  buffer_pos1 = buffer + buffer_size + 1;
  buffer_pos2 = buffer + buffer_size + 1;
}

/**************************************************************************//**
 * @see ExpatUserData#operator=
 * @param[in] o Object to replicate.
 */
ccruncher::ExpatUserData::ExpatUserData(const ExpatUserData &o) :
  pila(0), pila_pos(-1), current_tag(nullptr), buffer(nullptr),
  buffer_size(0), buffer_pos1(nullptr), buffer_pos2(nullptr)
{
  *this = o;
}

/**************************************************************************/
ccruncher::ExpatUserData::~ExpatUserData()
{
  if (buffer != nullptr) delete [] buffer;
}

/**************************************************************************//**
 * @param[in] o Object to assign.
 */
ExpatUserData & ccruncher::ExpatUserData::operator= (const ExpatUserData &o)
{
  if (this != &o)
  {
    pila = o.pila;
    pila_pos = o.pila_pos;

    current_tag = o.current_tag;
    defines = o.defines;

    if (buffer != nullptr) delete [] buffer;
    buffer_size = o.buffer_size;
    buffer = new char[buffer_size];
    buffer_pos1 = buffer + buffer_size + 1;
    buffer_pos2 = buffer + buffer_size + 1;
  }

  return *this;
}

/**************************************************************************//**
 * @details Establishes the handlers to be used in subsequent calls. These
 *          handlers will be removed when the end element tag be found.
 * @param[in] name Element name (length < 20).
 * @param[in] eh Element (and possibly sub-elements) handlers.
 */
void ccruncher::ExpatUserData::setCurrentHandlers(const char *name, ExpatHandlers *eh)
{
  pila_pos++;
  assert(pila_pos<MAX_STACK_SIZE);
  pila[pila_pos].handlers = eh;
  assert(strlen(name)<20);
  strncpy(pila[pila_pos].name, name, 19);
}

/**************************************************************************//**
 * @details Push content to buffer used to manage defines replacement.
 * @param[in] str Content to push.
 * @param[in] n Content length.
 * @throw Exception Buffer content bigger than buffer size.
 */
const char* ccruncher::ExpatUserData::bufferPush(const char *str, size_t n)
{
  if (buffer_pos2+n+1 < buffer+buffer_size)
  {
    buffer_pos1 = buffer_pos2+1;
  }
  else if (n < buffer_size)
  {
    buffer_pos1 = buffer;
  }
  else
  {
    throw Exception("buffer overflow");
  }

  memcpy(buffer_pos1, str, n);
  buffer_pos2 = buffer_pos1 + n;
  *buffer_pos2 = 0;

  return buffer_pos1;
}

/**************************************************************************//**
 * @details Append content to buffer used to manage defines replacement.
 * @param[in] str Content to push.
 * @param[in] n Content length.
 * @throw Exception Buffer content bigger than buffer size.
 */
const char* ccruncher::ExpatUserData::bufferAppend(const char *str, size_t n)
{
  if (buffer_pos2+n+1 < buffer+buffer_size)
  {
    memcpy(buffer_pos2, str, n);
    buffer_pos2 += n;
    *buffer_pos2 = 0;
  }
  else if (buffer_pos2-buffer_pos1+n < buffer_size)
  {
    memcpy(buffer, buffer_pos1, buffer_pos2-buffer_pos1);
    buffer_pos2 = buffer + (buffer_pos2-buffer_pos1);
    buffer_pos1 = buffer;
    memcpy(buffer_pos2, str, n);
    buffer_pos2 += n;
    *buffer_pos2 = 0;
  }
  else
  {
    throw Exception("buffer overflow");
  }

  return buffer_pos1;
}

/**************************************************************************//**
 * @details If the string contains defines, then make the replacement in
 *          a buffer and returns a pointer to this buffer. The content in
 *          the buffer is valid until another applyDefines call overwrites
 *          this content. If you experience problems (eg. a lot of parameters
 *          in the same element) increase the buffer size.
 * @note This method is defined here instead of ExpatParser because it is
 *       called from a static method: ExpatParser::startElement().
 * @param[in] str String to apply defines.
 * @return If not defines are found then returns the input string, otherwise
 *         returns a pointer to the buffer size.
 * @throw Exception Buffer content bigger than buffer size.
 * @throw Exception Define name not found.
 */
const char* ccruncher::ExpatUserData::applyDefines(const char *str)
{
  const char *ret = str;
  const char *p1 = str;
  const char *p2;

  while ((p2=strchr(p1,'$'))!=nullptr)
  {
    if (ret == str) bufferPush(p1, p2-p1);
    else bufferAppend(p1, p2-p1);

    p1 = p2 = p2+1; // skips '$'
    while(isalnum(*p2) || *p2=='_') p2++;
    string key(p1, p2-p1);

    auto it = defines.find(key);
    if (it == defines.end()) throw Exception("macro '" + key + "' not defined");
    ret = bufferAppend(it->second.c_str(), it->second.size());

    p1 = p2;
  }

  if (ret != str) ret = bufferAppend(p1, strlen(p1));
/*
cout << "buffer(" << buffer_size << ")='";
for(int i=0; i<buffer_size; i++) cout << (char)(buffer[i]==0?'.':buffer[i]);
cout << "'" << endl;
*/
  return ret;
}

