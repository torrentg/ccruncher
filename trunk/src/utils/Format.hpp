
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#ifndef _Format_
#define _Format_

#include <string>
#include <sstream>
#include <iostream>
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief   Functions to format usual types (int, double, date, etc.)
 */
class Format
{

  public:

    //! Converts int to string
    template <typename T>
    static std::string toString(const T &val);
    //! Returns bytes as string (B, KB, MB)
    static std::string bytes(const size_t val);

};

/**************************************************************************//**
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
template <typename T>
std::string ccruncher::Format::toString(const T &val)
{
  std::ostringstream oss;
  oss << std::boolalpha << val;
  return oss.str();
}

} // namespace

#endif

