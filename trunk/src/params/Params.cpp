
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
#include "utils/Strings.hpp"
#include "utils/Format.hpp"
#include "utils/Parser.hpp"
#include <cassert>

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
  onlyactive = false;
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
    throw Exception("unexpected tag " + string(name));
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
  else if (isEqual(name,"parameter")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name));
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
    throw Exception("invalid copula type: " + str + ". try 'gaussian' or 't(x)' with x>2");
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
  else if (name == "stopcriteria.maxiterations")
  {
    if (maxiterations >= 0L) throw Exception("parameter stopcriteria.maxiterations repeated");
    else maxiterations = getIntAttribute(attributes, "value");
    if (maxiterations < 0L) throw Exception("parameter stopcriteria.maxiterations out of range");
  }
  else if (name == "stopcriteria.maxseconds")
  {
    if (maxseconds >= 0L) throw Exception("parameter stopcriteria.maxseconds repeated");
    else maxseconds = getIntAttribute(attributes, "value");
    if (maxseconds < 0L) throw Exception("parameter stopcriteria.maxseconds out of range");
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
    rng_seed = getLongAttribute(attributes, "value");
  }
  else if (name == "montecarlo.antithetic")
  {
    antithetic = getBooleanAttribute(attributes, "value");
  }
  else if (name == "portfolio.onlyActiveObligors")
  {
    onlyactive = getBooleanAttribute(attributes, "value");
  }
  else
  {
    throw Exception("found unexpected parameter: " + name);
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
    throw Exception("parameter stopcriteria.maxiterations not defined");
  }

  if (maxseconds < 0L)
  {
    throw Exception("parameter stopcriteria.maxseconds not defined");
  }

  if (copula_type == "")
  {
    throw Exception("parameter copula.type not defined");
  }

  if (maxiterations == 0 && maxseconds == 0)
  {
    throw Exception("non finite stop criteria");
  }
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::Params::getXML(int ilevel) const throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<parameters>\n";
  ret += spc2 + "<parameter name='time.0' value='" + Format::toString(time0) + "'/>\n";
  ret += spc2 + "<parameter name='time.T' value='" + Format::toString(timeT) + "'/>\n";
  ret += spc2 + "<parameter name='stopcriteria.maxiterations' value='" + Format::toString(maxiterations) + "'/>\n";
  ret += spc2 + "<parameter name='stopcriteria.maxseconds' value='" + Format::toString(maxseconds) + "'/>\n";
  ret += spc2 + "<parameter name='copula.type' value='" + copula_type + "'/>\n";
  ret += spc2 + "<parameter name='rng.seed' value='" + Format::toString(rng_seed) + "'/>\n";
  ret += spc2 + "<parameter name='montecarlo.antithetic' value='" + Format::toString(antithetic) + "'/>\n";
  ret += spc2 + "<parameter name='portfolio.onlyActiveObligors' value='" + Format::toString(onlyactive) + "'/>\n";
  ret += spc1 + "</parameters>\n";

  return ret;
}
