
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <climits>
#include <iostream>
#include <sstream>
#include <cmath>
#include "utils/Parser.hpp"
#include <cassert>

using namespace std;
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
  assert(pnum != NULL);
  
  long aux = 0L;

  // parsing number
  try
  {
    aux = Parser::longValue(pnum);
  }
  catch(Exception)
  {
    throw Exception("error parsing integer value '" + string(pnum) + "': not a number");
  }

  // checking that is an integer
  if (aux < INT_MIN || INT_MAX < aux)
  {
    throw Exception("error parsing integer value '" + string(pnum) + "': value out of range");
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
  assert(pnum != NULL);
  
  char *pstr = NULL;

  // initializing numerical error status
  errno = 0;

  // parsing number
  long ret = strtol(pnum, &pstr, 10); //atol(str.c_str());

  // checking that is a long
  if (errno != 0 || pstr == pnum || pstr != pnum+strlen(pnum))
  {
    throw Exception("error parsing long value '" + string(pnum) + "': not a number");
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
// if numbers ends with '%' returns the number divided by 100
//===========================================================================
double ccruncher::Parser::doubleValue(const char *pnum) throw(Exception)
{
  assert(pnum != NULL);
  char *pstr = NULL;

  // initializing numerical error status
  errno = 0;

  // parsing number
  double ret = strtod(pnum, &pstr); //atof(str.c_str());

  // checking that is a double
  if (errno != 0 || pstr == pnum)
  {
    throw Exception("error parsing double value '" + string(pnum) + "': not a number");
  }
  else if (*pstr == 0)
  {
    return ret;
  }
  else if (*pstr == '%' && *(pstr+1) == 0)
  {
    return ret/100.0;
  }
  else
  {
    throw Exception("error parsing double value '" + string(pnum) + "': not a number");
  }
}

//===========================================================================
// parse a date
//===========================================================================
Date ccruncher::Parser::dateValue(const string &str) throw(Exception)
{
  return Date(str);
}

//===========================================================================
// parse a date
//===========================================================================
Date ccruncher::Parser::dateValue(const char *cstr) throw(Exception)
{
  assert(cstr != NULL);
  return Date(cstr);
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
    throw Exception("error parsing boolean value '" + str + "': distinct than 'true' or 'false'");
  }
}

//===========================================================================
// parse a boolean
//===========================================================================
bool ccruncher::Parser::boolValue(const char *cstr) throw(Exception)
{
  assert(cstr != NULL);

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
    throw Exception("error parsing boolean value '" + string(cstr) + "': distinct than 'true' or 'false'");
  }
}

