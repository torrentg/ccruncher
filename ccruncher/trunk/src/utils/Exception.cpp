
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
// Exception.cpp - Exception code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include "Exception.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const string &str)
{
   msg = str;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const std::exception &e)
{
   msg = e.what();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const std::exception &e, const string &str)
{
   msg = e.what();
   msg += "\n";
   msg += str;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Exception::~Exception() throw ()
{
  // nothing to do
}

//===========================================================================
// toString
//===========================================================================
string ccruncher::Exception::toString() const
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
ostream & ccruncher::operator << (ostream& os, Exception const &e)
{
  os << "Exception: " << e.toString() << endl;

  return os;
}
