
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#include "utils/Exception.hpp"

using namespace std;

/**************************************************************************//**
 * @details Creates an Exception with the given message.
 * @param[in] str Exception message.
 */
ccruncher::Exception::Exception(const std::string &str) noexcept : exception(), msg(str)
{
}

/**************************************************************************//**
 * @details Creates an Exception from a std::exception.
 * @param[in] e Base exception.
 */
ccruncher::Exception::Exception(const std::exception &e) noexcept : exception(e), msg(e.what())
{
}

/**************************************************************************//**
 * @details Creates an Exception from a std::exception extending the message.
 * @param[in] e Base exception.
 * @param[in] str Message clarifying the exception.
 */
ccruncher::Exception::Exception(const std::exception &e, const std::string &str) noexcept : exception(e)
{
  msg = e.what() + string("\n") + str;
}

/**************************************************************************//**
 * @details Returns exception message as string.
 * @return Exception message as string.
 */
const string & ccruncher::Exception::toString() const noexcept
{
  return msg;
}

/**************************************************************************//**
 * @details Returns exception message as char array.
 * @see http://www.cplusplus.com/reference/exception/exception/what/
 * @return Exception message as char array.
 */
const char * ccruncher::Exception::what() const noexcept
{
  return msg.c_str();
}

/**************************************************************************//**
 * @details Prints the exception message in an output stream.
 * @param[out] os Output stream.
 * @param [in] e Exception to write.
 * @return Reference to the output stream.
 */
std::ostream & ccruncher::operator << (std::ostream& os, const Exception &e)
{
  os << e.toString();
  return os;
}

