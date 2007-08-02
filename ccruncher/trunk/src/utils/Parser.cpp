
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
// Parser.cpp - Parser code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added support for char * type
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . format methods segregated to Format class
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <climits>
#include <iostream>
#include <sstream>
#include "utils/Parser.hpp"

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
  catch(Exception)
  {
    throw Exception("error parsing integer value " + string(pnum) + ": not a number");
  }

  // checking that is an integer
  if (aux < INT_MIN || INT_MAX < aux)
  {
    throw Exception("error parsing integer value " + string(pnum) + ": value out of range");
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
    throw Exception("error parsing long value " + string(pnum) + ": not a number");
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
    throw Exception("error parsing double value " + string(pnum) + ": not a number");
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
    throw Exception("error parsing boolean value " + str + " : distinct than 'true' or 'false'");
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
    throw Exception("error parsing boolean value " + string(cstr) + " : distinct than 'true' or 'false'");
  }
}

