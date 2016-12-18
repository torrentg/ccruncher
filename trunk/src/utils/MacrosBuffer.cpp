
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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
#include "utils/MacrosBuffer.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************/
ccruncher::MacrosBuffer::MacrosBuffer(size_t buffersize) :
  mBuffer(nullptr), mBufferSize(buffersize), mBufferPos1(nullptr), mBufferPos2(nullptr)
{
  if (mBufferSize == 0) mBufferSize = 1;
  mBuffer = new char[mBufferSize];
  mBufferPos1 = mBuffer + mBufferSize + 1;
  mBufferPos2 = mBuffer + mBufferSize + 1;
}

/**************************************************************************//**
 * @see MacrosBuffer#operator=
 * @param[in] o Object to replicate.
 */
ccruncher::MacrosBuffer::MacrosBuffer(const MacrosBuffer &o) :
  mBuffer(nullptr), mBufferSize(0), mBufferPos1(nullptr), mBufferPos2(nullptr)
{
  *this = o;
}

/**************************************************************************/
ccruncher::MacrosBuffer::~MacrosBuffer()
{
  delete [] mBuffer;
}

/**************************************************************************//**
 * @param[in] o Object to assign.
 */
MacrosBuffer & ccruncher::MacrosBuffer::operator= (const MacrosBuffer &o)
{
  if (this != &o)
  {
    values = o.values;
    delete [] mBuffer;
    mBufferSize = o.mBufferSize;
    mBuffer = new char[mBufferSize];
    mBufferPos1 = mBuffer + mBufferSize + 1;
    mBufferPos2 = mBuffer + mBufferSize + 1;
  }

  return *this;
}

/**************************************************************************//**
 * @details Push content to buffer used to manage macros replacement.
 * @param[in] str Content to push.
 * @param[in] n Content length.
 * @throw Exception Buffer content bigger than buffer size.
 */
const char* ccruncher::MacrosBuffer::bufferPush(const char *str, size_t n)
{
  if (mBufferPos2+n+1 < mBuffer+mBufferSize) {
    mBufferPos1 = mBufferPos2+1;
  }
  else if (n < mBufferSize) {
    mBufferPos1 = mBuffer;
  }
  else {
    throw Exception("macros buffer overflow");
  }

  memcpy(mBufferPos1, str, n);
  mBufferPos2 = mBufferPos1 + n;
  *mBufferPos2 = 0;

  return mBufferPos1;
}

/**************************************************************************//**
 * @details Append content to buffer used to manage macros replacement.
 * @param[in] str Content to push.
 * @param[in] n Content length.
 * @throw Exception Buffer content bigger than buffer size.
 */
const char* ccruncher::MacrosBuffer::bufferAppend(const char *str, size_t n)
{
  if (mBufferPos2+n+1 < mBuffer+mBufferSize) {
    memcpy(mBufferPos2, str, n);
    mBufferPos2 += n;
    *mBufferPos2 = 0;
  }
  else if (mBufferPos2-mBufferPos1+n < mBufferSize) {
    memcpy(mBuffer, mBufferPos1, mBufferPos2-mBufferPos1);
    mBufferPos2 = mBuffer + (mBufferPos2-mBufferPos1);
    mBufferPos1 = mBuffer;
    memcpy(mBufferPos2, str, n);
    mBufferPos2 += n;
    *mBufferPos2 = 0;
  }
  else {
    throw Exception("macros buffer overflow");
  }

  return mBufferPos1;
}

/**************************************************************************//**
 * @details If the string contains macros, then does the replacement in
 *          a buffer and returns a pointer to this buffer. The content in
 *          the buffer is valid until another apply call overwrites
 *          this content. If you experience problems (eg. a lot of parameters
 *          in the same element) increase the buffer size.
 * @param[in] str String to apply macros.
 * @return If not macros are found then returns the input string, otherwise
 *         returns a pointer to the buffer size.
 * @throw Exception Buffer content bigger than buffer size.
 * @throw Exception Macro name not found.
 */
const char* ccruncher::MacrosBuffer::apply(const char *str)
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

    auto it = values.find(key);
    if (it == values.end()) {
      throw Exception("macro '" + key + "' not defined");
    }
    ret = bufferAppend(it->second.c_str(), it->second.size());

    p1 = p2;
  }

  if (ret != str) ret = bufferAppend(p1, strlen(p1));
  return ret;
}

