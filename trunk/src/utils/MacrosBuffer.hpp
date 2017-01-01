
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#ifndef _MacrosBuffer_
#define _MacrosBuffer_

#include <map>
#include <string>

namespace ccruncher {

/**************************************************************************//**
 * @brief   Buffer where macros are applied.
 *
 * @details This class contains the map of macros. Also, given a string
 *          returns the string with the macros keys replaced by its values.
 *          Returned strings are temporary. Its content can be overwritted
 *          on succesive calls (depending on buffer size).
 */
class MacrosBuffer
{

  private:

    //! Buffer used to apply macros
    char *mBuffer;
    //! Buffer size
    size_t mBufferSize;
    //! Points to first char in current buffer
    char *mBufferPos1;
    //! Points to last char in current buffer
    char *mBufferPos2;

  private:

    //! Push str to buffer
    const char* bufferPush(const char *str, size_t n);
    //! Append str to buffer
    const char* bufferAppend(const char *str, size_t n);

  public:

    //! List of macros
    std::map<std::string,std::string> values;

  public:

    //! Constructor
    MacrosBuffer(size_t buffersize=4096);
    //! Copy constructor
    MacrosBuffer(const MacrosBuffer &);
    //! Destructor
    ~MacrosBuffer();
    //! Assignment operator
    MacrosBuffer & operator= (const MacrosBuffer &);
    //! Apply macros to the given string
    const char* apply(const char *str);

};

} // namespace

#endif

