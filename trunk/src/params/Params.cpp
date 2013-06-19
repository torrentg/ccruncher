
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

#include "params/Params.hpp"
#include "utils/Parser.hpp"
#include <climits>
#include <cassert>

using namespace std;
using namespace ccruncher;

//===========================================================================
// constructor
//===========================================================================
ccruncher::Params::Params()
{
  init();
}

//===========================================================================
// init
//===========================================================================
void ccruncher::Params::init()
{
  time0 = NAD;
  timeT = NAD;
  maxiterations = -1L;
  maxseconds = -1L;
  copula_type = "";
  rng_seed = 0L;
  antithetic = false;
  lhs_size = 1;
  onlyactive = false;
  blocksize = 128;
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Params::epstart(ExpatUserData &eu, const char *name, const char **atrs)
{
  if (isEqual(name,"parameters")) {
    // checking that don't have attributes
    if (getNumAttributes(atrs) > 0) {
      throw Exception("attributes are not allowed in tag parameters");
    }
  }
  else if (isEqual(name,"parameter")) {
    parseParameter(eu, atrs);
  }
  else {
    throw Exception("unexpected tag '" + string(name) + "'");
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Params::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"parameters")) {
    validate();
  }
}

//===========================================================================
// checks copula type string
//===========================================================================
string ccruncher::Params::getCopulaType(const string &str) throw(Exception)
{
  // gaussian case
  if (str == "gaussian") {
    return "gaussian";
  }
  // t-student case t(x)
  else if (str.length() >= 3 && str.substr(0,2) == "t(" && str.at(str.length()-1) == ')') {
    return "t";
  }
  // non-valid copula type
  else {
    throw Exception("invalid copula type: '" + str + "'");
  }
}

//===========================================================================
// checks copula type string
//===========================================================================
string ccruncher::Params::getCopulaType() const throw(Exception)
{
  return getCopulaType(copula_type);
}

//===========================================================================
// checks copula type string
//===========================================================================
double ccruncher::Params::getCopulaParam(const string &str) throw(Exception)
{
  if (getCopulaType(str) != "t") {
    throw Exception("copula without params");
  }
  // t-student case t(x), where x is a integer
  else {
    string val = str.substr(2, str.length()-3);
    double ndf = Parser::doubleValue(val);
    if (ndf < 2.0) 
    {
      throw Exception("t-student copula requires ndf >= 2");
    }
    return ndf;
  }
}

//===========================================================================
// checks copula type string
//===========================================================================
double ccruncher::Params::getCopulaParam() const throw(Exception)
{
  return getCopulaParam(copula_type);
}

//===========================================================================
// parse a XML parameter
//===========================================================================
void ccruncher::Params::parseParameter(ExpatUserData &, const char **attributes) throw(Exception)
{
  // reading attribute name
  string name = getStringAttribute(attributes, "name");

  if (name == "time.0")
  {
    if (time0 != NAD) throw Exception("parameter time.0 repeated");
    else time0 = getDateAttribute(attributes, "value");
  }
  else if (name == "time.T")
  {
    if (timeT != NAD) throw Exception("parameter time.T repeated");
    else timeT = getDateAttribute(attributes, "value");
  }
  else if (name == "maxiterations")
  {
    if (maxiterations >= 0L) throw Exception("parameter maxiterations repeated");
    else maxiterations = getIntAttribute(attributes, "value");
    if (maxiterations < 0L) throw Exception("parameter maxiterations out of range");
  }
  else if (name == "maxseconds")
  {
    if (maxseconds >= 0L) throw Exception("parameter maxseconds repeated");
    else maxseconds = getIntAttribute(attributes, "value");
    if (maxseconds < 0L) throw Exception("parameter maxseconds out of range");
  }
  else if (name == "copula.type")
  {
    if (copula_type != "") throw Exception("parameter copula.type repeated");
    else copula_type = getStringAttribute(attributes, "value");
    if (getCopulaType() == "t") {
      getCopulaParam(); //parse and validate param
    }
  }
  else if (name == "rng.seed")
  {
    long aux = getLongAttribute(attributes, "value");
    rng_seed = *((unsigned long *)(&aux));
  }
  else if (name == "antithetic")
  {
    antithetic = getBooleanAttribute(attributes, "value");
  }
  else if (name == "lhs")
  {
    try
    {
      if (getBooleanAttribute(attributes, "value") == false) {
        lhs_size = 1;
      }
      else {
        lhs_size = 1000;
      }
    }
    catch(Exception &e)
    {
      int aux = getIntAttribute(attributes, "value");
      if (aux <= 0 || USHRT_MAX < aux) {
        throw Exception("parameter lhs out of range");
      }
      else {
        lhs_size = aux;
      }
    }
  }
  else if (name == "onlyActiveObligors")
  {
    onlyactive = getBooleanAttribute(attributes, "value");
  }
  else if (name == "blocksize")
  {
    int aux = getIntAttribute(attributes, "value");
    if (aux <= 0 || USHRT_MAX < aux) {
      throw Exception("parameter blocksize out of range");
    }
    else {
      blocksize = (unsigned short)(aux);
    }
  }
  else
  {
    throw Exception("unexpected parameter: '" + name + "'");
  }
}

//===========================================================================
// check class content
//===========================================================================
void ccruncher::Params::validate() const throw(Exception)
{

  if (time0 == NAD)
  {
    throw Exception("parameter time.0 not defined");
  }

  if (timeT == NAD)
  {
    throw Exception("parameter time.T not defined");
  }

  if (time0 >= timeT)
  {
    throw Exception("time.0 >= time.T");
  }

  if (timeT.getYear()-time0.getYear() > 101) {
    throw Exception("more than 100 years between time0 and timeT");
  }

  if (maxiterations < 0L)
  {
    throw Exception("parameter maxiterations not defined");
  }

  if (maxseconds < 0L)
  {
    throw Exception("parameter maxseconds not defined");
  }

  if (copula_type == "")
  {
    throw Exception("parameter copula.type not defined");
  }

  if (maxiterations == 0 && maxseconds == 0)
  {
    throw Exception("non finite stop criteria");
  }

  if (lhs_size < 1)
  {
    throw Exception("parameter lhs not defined or less than 1");
  }

  if (blocksize < 1)
  {
    throw Exception("parameter blocksize not defined or less than 1");
  }

  if (antithetic && blocksize%2 != 0)
  {
    throw Exception("blocksize must be multiple of 2 when antithetic is enabled");
  }
}

