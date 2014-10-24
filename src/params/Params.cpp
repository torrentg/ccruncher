
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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
#include <limits>
#include <cassert>
#include "params/Params.hpp"
#include "utils/Exception.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include "utils/Utils.hpp"

#define TIME0 "time.0"
#define TIMET "time.T"
#define MAXITERATIONS "maxiterations"
#define MAXSECONDS "maxseconds"
#define COPULA "copula"
#define RNGSEED "rng.seed"
#define ANTITHETIC "antithetic"
#define BLOCKSIZE "blocksize"

#define DEFAULT_RNGSEED "0"
#define DEFAULT_ANTITHETIC "false"
#define DEFAULT_BLOCKSIZE "128"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] atrs Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Params::epstart(ExpatUserData &, const char *tag, const char **atrs)
{
  if (isEqual(tag,"parameters")) {
    if (getNumAttributes(atrs) > 0) {
      throw Exception("unexpected attributes in tag parameters");
    }
  }
  else if (isEqual(tag,"parameter"))
  {
    if (getNumAttributes(atrs) > 2) {
      throw Exception("unexpected attribute in tag parameter");
    }

    string name = getStringAttribute(atrs, "name");
    if (name != TIME0 && name != TIMET && name != MAXITERATIONS &&
        name != MAXSECONDS && name != COPULA && name != RNGSEED &&
        name != ANTITHETIC && name != BLOCKSIZE) {
      throw Exception("unexpected parameter '" + name + "'");
    }

    string value = getStringAttribute(atrs, "value");
    if (this->find(name) != this->end()) {
      throw Exception("parameter '" + name + "' already defined");
    }
    else {
      (*this)[name] = value;
    }
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 * @exception Exception Parameters validations failed.
 */
void ccruncher::Params::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"parameters")) {
    isValid(true);
  }
}

/**************************************************************************//**
 * @details Check that all variables are defined and have a valid value.
 * @throw Exception Error validating parameters.
 */
bool ccruncher::Params::isValid(bool throwException) const
{
  try
  {
    if (getTime0() >= getTimeT()) {
      throw Exception(TIME0 " >= " TIMET);
    }

    if (getTimeT().getYear()-getTime0().getYear() > 101) {
      throw Exception("more than 100 years between " TIME0 " and " TIMET);
    }

    getNdf();
    getRngSeed();

    if (getAntithetic() && getBlockSize()%2 != 0) {
      throw Exception(BLOCKSIZE " must be multiple of 2 when " ANTITHETIC " is enabled");
    }

    return true;
  }
  catch(Exception &e)
  {
    if (throwException) throw;
    else return false;
  }
}

/**************************************************************************//**
 * @param[in] key Parameter key.
 * @param[in] defaultValue Value to return if key not found.
 * @return Parameter value.
 */
string ccruncher::Params::getParamValue(const std::string &key, const std::string &defaultValue) const
{
  auto it = this->find(key);
  if (it == this->end()) {
    return defaultValue;
  }
  else {
    return it->second;
  }
}

/**************************************************************************//**
 * @param[in] key Parameter key.
 * @return Parameter value.
 * @exception Exception Parameter not found.
 */
string ccruncher::Params::getParamValue(const std::string &key) const
{
  auto it = this->find(key);
  if (it == this->end()) {
    throw Exception("parameter '" + key + "' not found");
  }
  return it->second;
}

/**************************************************************************//**
 * @return Starting date.
 * @exception Exception Invalid parameter value.
 */
Date ccruncher::Params::getTime0() const
{
  string value = getParamValue(TIME0);
  return Parser::dateValue(value);
}

/**************************************************************************//**
 * @return Ending date.
 * @exception Exception Invalid parameter value.
 */
Date ccruncher::Params::getTimeT() const
{
  string value = getParamValue(TIMET);
  return Parser::dateValue(value);
}

/**************************************************************************//**
 * @return Number of Monte Carlo iterations.
 * @exception Exception Invalid parameter value.
 */
size_t ccruncher::Params::getMaxIterations() const
{
  string value = getParamValue(MAXITERATIONS);
  long num = Parser::longValue(value);
  if (num < 0) {
    throw Exception("parameter '" MAXITERATIONS "' < 0");
  }
  return (size_t) num;
}

/**************************************************************************//**
 * @return Maximum execution time (in seconds).
 * @exception Exception Invalid parameter value.
 */
size_t ccruncher::Params::getMaxSeconds() const
{
  string value = getParamValue(MAXSECONDS);
  long num = Parser::longValue(value);
  if (num < 0) {
    throw Exception("parameter '" MAXSECONDS "' < 0");
  }
  return (size_t) num;
}

/**************************************************************************//**
 * @return Copula, gaussian ot t(ndf).
 * @throw Exception Invalid parameter value.
 */
std::string ccruncher::Params::getCopula() const
{
  return getParamValue(COPULA);
}

/**************************************************************************//**
 * @details If gaussian copula returns a non-valid ndf (<= 0).
 * @return Degrees of freedom of the t-copula (ndf>=2).
 * @throw Exception Invalid parameter value.
 */
double ccruncher::Params::getNdf() const
{
  string copula = getCopula();
  if (copula == "gaussian") {
    return -1.0;
  }
  else if (copula.length() >= 3 &&
           copula.substr(0,2) == "t(" &&
           copula.at(copula.length()-1) == ')') {
    string val = copula.substr(2, copula.length()-3);
    double ndf = Parser::doubleValue(val);
    if (ndf < 2.0) {
      throw Exception("t-student copula requires ndf >= 2");
    }
    return ndf;
  }
  else {
    throw Exception("invalid copula type: '" + copula + "'");
  }
}

/**************************************************************************//**
 * @details If seed not defined by user or equals to 0, then returns a
 *          value based on clock.
 * @return Rng seed.
 * @throw Exception Invalid parameter value.
 */
ulong ccruncher::Params::getRngSeed() const
{
  ulong seed = 0UL;
  string value = getParamValue(RNGSEED, DEFAULT_RNGSEED);
  long aux = Parser::longValue(value);
  seed = *(reinterpret_cast<ulong *>(&aux));
  if (seed == 0UL) {
    // seed based on clock
    seed = Utils::trand();
  }
  return seed;
}

/**************************************************************************//**
 * @return Use antithetic method in Monte Carlo.
 * @throw Exception Invalid parameter value.
 */
bool ccruncher::Params::getAntithetic() const
{
  string value = getParamValue(ANTITHETIC, DEFAULT_ANTITHETIC);
  return Parser::boolValue(value);
}

/**************************************************************************//**
 * @return Number of simultaneous simulations does by thread.
 * @throw Exception Invalid parameter value.
 */
ushort ccruncher::Params::getBlockSize() const
{
  string value = getParamValue(BLOCKSIZE, DEFAULT_BLOCKSIZE);
  int aux = Parser::intValue(value);
  if (aux <= 0 || USHRT_MAX < aux) {
    throw Exception("parameter '" BLOCKSIZE "' out of range [1," +
                    Format::toString((int)USHRT_MAX) + "]");
  }
  else {
    return (unsigned short) aux;
  }
}

