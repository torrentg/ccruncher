
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

#include <climits>
#include <cassert>
#include "params/Params.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @details Set parameters to non-valid values.
 */
ccruncher::Params::Params()
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

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] name Element name.
 * @param[in] atrs Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Params::epstart(ExpatUserData &eu, const char *name, const char **atrs)
{
  if (isEqual(name,"parameters")) {
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

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] name Element name.
 */
void ccruncher::Params::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"parameters")) {
    validate();
  }
}

/**************************************************************************//**
 * @return Copula type ('gaussian' or 't').
 * @throw Exception Invalid copula identifier.
 */
string ccruncher::Params::getCopulaType() const throw(Exception)
{
  // gaussian case
  if (copula_type == "gaussian") {
    return "gaussian";
  }
  // t-student case t(x)
  else if (copula_type.length() >= 3 &&
           copula_type.substr(0,2) == "t(" &&
           copula_type.at(copula_type.length()-1) == ')') {
    return "t";
  }
  // non-valid copula type
  else {
    throw Exception("invalid copula type: '" + copula_type + "'");
  }
}


/**************************************************************************//**
 * @return Degrees of freedom of the t-copula (ndf>=2).
 * @throw Exception Invalid copula or copula params.
 */
double ccruncher::Params::getCopulaParam() const throw(Exception)
{
  if (getCopulaType() != "t") {
    throw Exception("copula without params");
  }
  // t-student case t(x), where x is a integer
  else {
    string val = copula_type.substr(2, copula_type.length()-3);
    double ndf = Parser::doubleValue(val);
    if (ndf < 2.0)
    {
      throw Exception("t-student copula requires ndf >= 2");
    }
    return ndf;
  }
}

/**************************************************************************//**
 * @param[in] attributes Xml element attributes.
 * @throw Exception Error processing xml data.
 */
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
    rng_seed = *(reinterpret_cast<unsigned long *>(&aux));
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
    catch(Exception &)
    {
      int aux = getIntAttribute(attributes, "value");
      if (aux <= 0 || USHRT_MAX < aux) {
        throw Exception("parameter lhs out of range [1," +
                        Format::toString((int)USHRT_MAX) + "]");
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
      throw Exception("parameter blocksize out of range [1," +
                      Format::toString((int)USHRT_MAX) + "]");
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

/**************************************************************************//**
 * @details Check that all variables are defined and have a valid value.
 * @throw Exception Error validating parameters.
 */
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

