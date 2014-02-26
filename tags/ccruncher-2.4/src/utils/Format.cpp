
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

#include <iostream>
#include <iomanip>
#include <sstream>
#include "utils/Format.hpp"

using namespace std;

/**************************************************************************//**
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
string ccruncher::Format::toString(const int val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

/**************************************************************************//**
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
string ccruncher::Format::toString(const long val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

/**************************************************************************//**
 * @param[in] val Input value.
 * @param[in] n Decimal precision to be used to format floating-point.
 * @return Value serialized to string.
 */
string ccruncher::Format::toString(const double val, int n)
{
  ostringstream oss;
  if (n >= 0) {
    oss << fixed << setprecision(n);
  }
  oss << val;
  return oss.str();
}

/**************************************************************************//**
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
string ccruncher::Format::toString(const ccruncher::Date &val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

/**************************************************************************//**
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
string ccruncher::Format::toString(const bool val)
{
  ostringstream oss;
  oss << (val?"true":"false");
  return oss.str();
}

/**************************************************************************//**
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
string ccruncher::Format::toString(const size_t val)
{
  ostringstream oss;
  oss << (int)val;
  return oss.str();
}

/**************************************************************************//**
 * @details Returns bytes as string (B, KB, MB, GB)
 * @param[in] val Input value.
 * @return Value serialized to string.
 */
string ccruncher::Format::bytes(const size_t val)
{
  if (val < 1024) {
    return toString(val) + " B";
  }
  else if (val < 1024*1024) {
    return toString(double(val)/double(1024), 2) + " KB";
  }
  else {
    return toString(double(val)/double(1024*1024), 2) + " MB";
  }
}

