
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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
#include <limits>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>
#include "utils/Parser.hpp"
#include "utils/Expr.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] str String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
int ccruncher::Parser::intValue(const std::string &str)
{
  return intValue(str.c_str());
}

/**************************************************************************//**
 * @param[in] pnum String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
int ccruncher::Parser::intValue(const char *pnum)
{
  assert(pnum != nullptr);
  
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
  if (aux < numeric_limits<int>::min() || numeric_limits<int>::max() < aux)
  {
    throw Exception("error parsing integer value '" + string(pnum) + "': value out of range");
  }
  else
  {
    return int(aux);
  }
}

/**************************************************************************//**
 * @param[in] str String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
long ccruncher::Parser::longValue(const std::string &str)
{
  return longValue(str.c_str());
}

/**************************************************************************//**
 * @param[in] pnum String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
long ccruncher::Parser::longValue(const char *pnum)
{
  assert(pnum != nullptr);
  
  char *pstr = nullptr;

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

/**************************************************************************//**
 * @param[in] str String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
double ccruncher::Parser::doubleValue(const std::string &str)
{
  return doubleValue(str.c_str());
}

/**************************************************************************//**
 * @param[in] pnum String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
double ccruncher::Parser::doubleValue(const char *pnum)
{
  assert(pnum != nullptr);
  char *pstr = nullptr;

  // initializing numerical error status
  errno = 0;

  // parsing number
  double ret = strtod(pnum, &pstr); //atof(str.c_str());

  // checking that is a double
  if (errno != 0 || pstr == pnum)
  {
    return eval(pnum);
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
    return eval(pnum);
  }
}

/**************************************************************************//**
 * @param[in] pnum String to parse.
 * @return Expression value.
 * @throw Exception Invalid format.
 */
double ccruncher::Parser::eval(const char *pnum)
{
  try
  {
    vector<Expr::variable> variables;
    vector<Expr::token> tokens;
    Expr::compile(pnum, variables, tokens);
    if (!variables.empty()) throw Exception("found a variable: " + variables[0].id);
    int maxsize = Expr::link(tokens, variables);
    return Expr::eval(tokens, maxsize);
  }
  catch(Exception &e)
  {
    throw Exception(e, "error parsing double value '" + string(pnum) + "': not a number");
  }
}

/**************************************************************************//**
 * @param[in] str String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
Date ccruncher::Parser::dateValue(const std::string &str)
{
  return Date(str);
}

/**************************************************************************//**
 * @param[in] cstr String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
Date ccruncher::Parser::dateValue(const char *cstr)
{
  assert(cstr != nullptr);
  return Date(cstr);
}

/**************************************************************************//**
 * @param[in] str String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
bool ccruncher::Parser::boolValue(const std::string &str)
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

/**************************************************************************//**
 * @param[in] cstr String to parse.
 * @return Value parsed.
 * @throw Exception Invalid format.
 */
bool ccruncher::Parser::boolValue(const char *cstr)
{
  assert(cstr != nullptr);

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

