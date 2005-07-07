
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
// Parser.cpp - Parser code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added support for char * type
//
//===========================================================================

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <climits>
#include <iostream>
#include <sstream>
#include "utils/Parser.hpp"

using namespace ccruncher;

//===========================================================================
// parse an integer
//===========================================================================
int ccruncher::Parser::intValue(const string &str) throw(Exception)
{
  return intValue(str.c_str());
}

//===========================================================================
// parse an integer
//===========================================================================
int ccruncher::Parser::intValue(const char *pnum) throw(Exception)
{
  long aux = 0L;

  // parsing number
  try
  {
    aux = Parser::longValue(pnum);
  }
  catch(Exception &e)
  {
    throw Exception("Parser::intValue(): invalid number");
  }

  // checking that is an integer
  if (aux < INT_MIN || INT_MAX < aux)
  {
    throw Exception("Parser::intValue(): value out of range");
  }
  else
  {
    return int(aux);
  }
}

//===========================================================================
// parse a long
//===========================================================================
long ccruncher::Parser::longValue(const string &str) throw(Exception)
{
  return longValue(str.c_str());
}

//===========================================================================
// parse a long
//===========================================================================
long ccruncher::Parser::longValue(const char *pnum) throw(Exception)
{
  char *pstr = NULL;

  // initializing numerical error status
  errno = 0;

  // parsing number
  long ret = strtol(pnum, &pstr, 10); //atol(str.c_str());

  // checking that is a long
  if (errno != 0 || pstr != pnum + strlen(pnum) || strlen(pnum) == 0)
  {
    throw Exception("Parser::longValue(): invalid long number");
  }
  else
  {
    return ret;
  }
}

//===========================================================================
// parse a double
//===========================================================================
double ccruncher::Parser::doubleValue(const string &str) throw(Exception)
{
  return doubleValue(str.c_str());
}

//===========================================================================
// parse a double
//===========================================================================
double ccruncher::Parser::doubleValue(const char *pnum) throw(Exception)
{
  char *pstr = NULL;

  // initializing numerical error status
  errno = 0;

  // parsing number
  double ret = strtod(pnum, &pstr); //atof(str.c_str());

  // checking that is a double
  if (errno != 0 || pstr != pnum + strlen(pnum) || strlen(pnum) == 0)
  {
    throw Exception("Parser::doubleValue(): invalid double value");
  }
  else
  {
    return ret;
  }
}

//===========================================================================
// parse a date
//===========================================================================
Date ccruncher::Parser::dateValue(const string &str) throw(Exception)
{
  // parsing date
  Date ret = Date(str);

  // exit
  return ret;
}

//===========================================================================
// parse a date
//===========================================================================
Date ccruncher::Parser::dateValue(const char *cstr) throw(Exception)
{
  return dateValue(string(cstr));
}

//===========================================================================
// parse a boolean
//===========================================================================
bool ccruncher::Parser::boolValue(const string &str) throw(Exception)
{
  if (str == "true")
  {
    return true;
  }
  else if (str == "false")
  {
    return false;
  }
  else
  {
    throw Exception("Parser::boolValue(): invalid boolean value");
  }
}

//===========================================================================
// parse a boolean
//===========================================================================
bool ccruncher::Parser::boolValue(const char *cstr) throw(Exception)
{
  if (strcmp(cstr,"true") == 0)
  {
    return true;
  }
  else if (strcmp(cstr,"false") == 0)
  {
    return false;
  }
  else
  {
    throw Exception("Parser::boolValue(): invalid boolean value");
  }
}

//===========================================================================
// int2string
//===========================================================================
string ccruncher::Parser::int2string(const int val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// long2string
//===========================================================================
string ccruncher::Parser::long2string(const long val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// double2string
//===========================================================================
string ccruncher::Parser::double2string(const double val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// date2string
//===========================================================================
string ccruncher::Parser::date2string(const ccruncher::Date &val)
{
  ostringstream oss;
  oss << val;
  return oss.str();
}

//===========================================================================
// bool2string
//===========================================================================
string ccruncher::Parser::bool2string(const bool &val)
{
  ostringstream oss;
  oss << (val?"true":"false");
  return oss.str();
}
