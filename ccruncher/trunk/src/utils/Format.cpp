
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
//
// Format.cpp - Format code - $Rev$
// --------------------------------------------------------------------------
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <iostream>
#include <sstream>
#include "utils/Format.hpp"

//===========================================================================
// int2string
//===========================================================================
string ccruncher::Format::int2string(const int val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// long2string
//===========================================================================
string ccruncher::Format::long2string(const long val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// double2string
//===========================================================================
string ccruncher::Format::double2string(const double val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// date2string
//===========================================================================
string ccruncher::Format::date2string(const ccruncher::Date &val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// bool2string
//===========================================================================
string ccruncher::Format::bool2string(const bool &val)
{
  ostringstream oss;
  oss << (val?"true":"false");
  return oss.str();
}
