
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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
#include <sstream>
#include "utils/Format.hpp"

//===========================================================================
// int
//===========================================================================
string ccruncher::Format::toString(const int val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// long
//===========================================================================
string ccruncher::Format::toString(const long val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// double
//===========================================================================
string ccruncher::Format::toString(const double val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// date
//===========================================================================
string ccruncher::Format::toString(const ccruncher::Date &val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// bool
//===========================================================================
string ccruncher::Format::toString(const bool val)
{
  ostringstream oss;
  oss << (val?"true":"false");
  return oss.str();
}

//===========================================================================
// unsigned int
//===========================================================================
string ccruncher::Format::toString(const unsigned int val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

#ifndef size_t
//===========================================================================
// size_t
//===========================================================================
string ccruncher::Format::toString(const size_t val)
{
  ostringstream oss;
  oss << (int)val;
  return oss.str();
}
#endif

