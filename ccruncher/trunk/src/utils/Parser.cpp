
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
//===========================================================================

#include <cstdlib>
#include <cerrno>
#include <climits>
#include <iostream>
#include <sstream>
#include "Parser.hpp"

using namespace ccruncher;

//===========================================================================
// interpreta un int
//===========================================================================
int ccruncher::Parser::intValue(const string &str) throw(Exception)
{
  long aux = 0L;
  
  // parsejem el numero 
  try
  {
    aux = Parser::longValue(str);
  }
  catch(Exception &e)
  {
    throw Exception("Parser::intValue(): invalid number");
  }
  
  // comprovem si es tracta de un enter
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
// interpreta un long
//===========================================================================
long ccruncher::Parser::longValue(const string &str) throw(Exception)
{
  char *pstr = NULL;
  const char *pnum = str.c_str();

  // inicialitzem estat errors numerics 
  errno = 0;

  // parsejem el numero 
  long ret = strtol(pnum, &pstr, 10); //atol(str.c_str());

  // comprovem si es tracta de un enter
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
// interpreta un double
//===========================================================================
double ccruncher::Parser::doubleValue(const string &str) throw(Exception)
{
  char *pstr = NULL;
  const char *pnum = str.c_str();
  
  // inicialitzem estat errors numerics 
  errno = 0;
  
  // parsejem el numero 
  double ret = strtod(pnum, &pstr); //atof(str.c_str());
  
  // comprovem si es tracta de un double
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
// interpreta un date
//===========================================================================
Date ccruncher::Parser::dateValue(const string &str) throw(Exception)
{
  // parsejem la data
  Date ret = Date(str);
  
  // sortim
  return ret;
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
