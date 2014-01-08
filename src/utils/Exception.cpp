
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

#include "utils/Exception.hpp"

using namespace std;

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Exception::Exception() : exception(), msg("")
{
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const string &m) : exception(), msg(m)
{
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const std::exception &e) : exception(e), msg(e.what())
{
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const std::exception &e, const string &m) : exception(e)
{
  msg = e.what() + string("\n") + m;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Exception::~Exception() throw()
{
  // nothing to do
}

//===========================================================================
// toString
//===========================================================================
const string & ccruncher::Exception::toString() const
{
  return msg;
}

//===========================================================================
// toString
//===========================================================================
const char * ccruncher::Exception::what() const throw()
{
  return msg.c_str();
}

//===========================================================================
// output operator
//===========================================================================
ostream & ccruncher::operator << (ostream& os, const Exception &e)
{
  os << e.toString();
  return os;
}

