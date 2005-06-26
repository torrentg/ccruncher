
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
// 2005/06/05 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added getStackTrace() method (link with -rdynamic flag)
//
//===========================================================================

#include "utils/Exception.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(char *str) : exception()
{
  msg = string(str);
  stacktrace = retrieveStackTrace();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const string &str) : exception()
{
  msg = str;
  stacktrace = retrieveStackTrace();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const ccruncher::Exception &e) : exception(e)
{
  msg = e.what();
  stacktrace = e.getStackTrace();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const std::exception &e) : exception(e)
{
  msg = e.what();
  stacktrace = retrieveStackTrace();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const ccruncher::Exception &e, const string &str) : exception(e)
{
  msg = e.what() + string("\n") + str;
  stacktrace = e.getStackTrace();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exception::Exception(const std::exception &e, const string &str) : exception(e)
{
  msg = e.what() + string("\n") + str;
  stacktrace = retrieveStackTrace();
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
  os << "\nException: " << e.toString() << "\n" << e.getStackTrace() << endl;

  return os;
}

#if defined(__linux__) && ( ! defined(NDEBUG) )
#include <execinfo.h>    // Needed for backtrace
#include <cxxabi.h>      // Needed for __cxa_demangle
#include <unistd.h>
//===========================================================================
// retrieveStackTrace
//===========================================================================
string ccruncher::Exception::retrieveStackTrace()
{
  int maxbt=100, numbt;
  void *btbuf[maxbt];
  char **symbuf;
  int status;
  char *demangledname;
  unsigned int pos1, pos2;
  string addr, func;
  string ret="\n";

  // retrieving backtrace
  numbt = backtrace(btbuf, maxbt);
  if (numbt < 0) {
    return string("backtrace could not be retrieved\n");
  }

  // retrieving symbols
  symbuf = backtrace_symbols(btbuf, numbt);
  if (symbuf == NULL) {
    return string("backtrace could not get symbols\n");
  }

  // printing backtrace lines (atention: first 2 frames skiped!)
  for (int i=2; i<numbt; ++i)
  {
    string cur = symbuf[i];

    // retrieving mem adress
    pos1 = cur.rfind ('[');
    pos2 = cur.rfind (']');
    if ((pos1 != string::npos) && (pos2 != string::npos))
    {
      addr = cur.substr(pos1 + 1, pos2 - pos1 - 1);
    }

    // retrieving function name (link with -rdynamic flag)
    pos1 = cur.rfind ("(_Z");
    pos2 = cur.rfind ('+');
    if (pos2 != string::npos)
    {
      if (pos1 != string::npos)
      {
        func = cur.substr(pos1 + 1, pos2 - pos1 - 1);
        demangledname = NULL;
        demangledname = abi::__cxa_demangle(func.c_str(), NULL, NULL, &status);
        if (status == 0) {
          func = string(demangledname);
        }
        else {
          func = "[" + addr + "]";
        }
        if (demangledname != NULL) {
          free(demangledname);
        }
      }
      else
      {
        pos1 = cur.rfind ('(');
        func = cur.substr (pos1 + 1, pos2 - pos1 - 1);
      }
    }

    ret += "in " + func + "\n";
    if (func == "main") break;
  }

  // exit function
  free(symbuf);
  return ret;
}
#else
//===========================================================================
// retrieveStackTrace
//===========================================================================
string ccruncher::Exception::retrieveStackTrace()
{
  return string("");
}
#endif

//===========================================================================
// retrieveStackTrace
//===========================================================================
string ccruncher::Exception::getStackTrace() const
{
  return stacktrace;
}
