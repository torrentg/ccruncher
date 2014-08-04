
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

#define TIME0 "time.0"
#define TIMET "time.T"
#define MAXITERATIONS "maxiterations"
#define MAXSECONDS "maxseconds"
#define COPULA "copula.type"
#define RNG_SEED "rng.seed"
#define ANTITHETIC "antithetic"
#define LHS "lhs"
#define BLOCKSIZE "blocksize"

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
        name != MAXSECONDS && name != COPULA && name != RNG_SEED &&
        name != ANTITHETIC && name != LHS && name != BLOCKSIZE) {
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

    if (getMaxIterations() == 0 && getMaxSeconds() == 0) {
      throw Exception("non finite stop criteria");
    }

    getCopulaType();
    getCopulaParam();
    getRngSeed();
    getLhsSize();

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
 * @return Starting date.
 * @exception Exception Invalid parameter value.
 */
Date ccruncher::Params::getTime0() const
{
  auto it = this->find(TIME0);
  if (it == this->end()) {
    throw Exception("parameter '" TIME0 "' not found");
  }
  return Parser::dateValue(it->second);
}

/**************************************************************************//**
 * @return Ending date.
 * @exception Exception Invalid parameter value.
 */
Date ccruncher::Params::getTimeT() const
{
  auto it = this->find(TIMET);
  if (it == this->end()) {
    throw Exception("parameter '" TIMET "' not found");
  }
  return Parser::dateValue(it->second);
}

/**************************************************************************//**
 * @return Number of Monte Carlo iterations.
 * @exception Exception Invalid parameter value.
 */
size_t ccruncher::Params::getMaxIterations() const
{
  auto it = this->find(MAXITERATIONS);
  if (it == this->end()) {
    throw Exception("parameter '" MAXITERATIONS "' not found");
  }
  long num = Parser::longValue(it->second);
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
  auto it = this->find(MAXSECONDS);
  if (it == this->end()) {
    throw Exception("parameter '" MAXSECONDS "' not found");
  }
  long num = Parser::longValue(it->second);
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
  auto it = this->find(COPULA);
  if (it == this->end()) {
    throw Exception("parameter '" COPULA "' not found");
  }
  return it->second;
}

/**************************************************************************//**
 * @return Copula type ('gaussian' or 't').
 * @throw Exception Invalid parameter value.
 */
string ccruncher::Params::getCopulaType() const
{
  auto it = this->find(COPULA);
  if (it == this->end()) {
    throw Exception("parameter '" COPULA "' not found");
  }
  string copula = it->second;
  if (copula == "gaussian") {
    return "gaussian";
  }
  else if (copula.length() >= 3 &&
           copula.substr(0,2) == "t(" &&
           copula.at(copula.length()-1) == ')') {
    return "t";
  }
  else {
    throw Exception("invalid copula type: '" + copula + "'");
  }
}

/**************************************************************************//**
 * @return Degrees of freedom of the t-copula (ndf>=2).
 * @throw Exception Invalid parameter value.
 */
double ccruncher::Params::getCopulaParam() const
{
  auto it = this->find(COPULA);
  if (it == this->end()) {
    throw Exception("parameter '" COPULA "' not found");
  }
  string copula = it->second;
  if (copula == "gaussian") {
    return std::numeric_limits<double>::infinity();
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
 * @return Rng seed.
 * @throw Exception Invalid parameter value.
 */
unsigned long ccruncher::Params::getRngSeed() const
{
  auto it = this->find(RNG_SEED);
  if (it == this->end()) {
    return 0ul; // default value = '0'
  }
  long aux = Parser::longValue(it->second);
  unsigned long seed = *(reinterpret_cast<unsigned long *>(&aux));
  return seed;
}

/**************************************************************************//**
 * @return Use antithetic method in Monte Carlo.
 * @throw Exception Invalid parameter value.
 */
bool ccruncher::Params::getAntithetic() const
{
  auto it = this->find(ANTITHETIC);
  if (it == this->end()) {
    return false; // default value = 'false'
  }
  return Parser::boolValue(it->second);
}

/**************************************************************************//**
 * @return Number of bins in the LHS method.
 * @throw Exception Invalid parameter value.
 */
unsigned short ccruncher::Params::getLhsSize() const
{
  auto it = this->find(LHS);
  if (it == this->end()) {
    return 1; // default value = 'false'
  }
  string value = it->second;
  try {
    if (Parser::boolValue(value) == false) {
      return 1;
    }
    else {
      return 1000;
    }
  }
  catch(Exception &) {
    int aux = Parser::intValue(value);
    if (aux <= 0 || USHRT_MAX < aux) {
      throw Exception("parameter '" LHS "' out of range [1," +
                      Format::toString((int)USHRT_MAX) + "]");
    }
    else {
      return (unsigned short) aux;
    }
  }
}

/**************************************************************************//**
 * @return Number of simultaneous simulations does by thread.
 * @throw Exception Invalid parameter value.
 */
unsigned short ccruncher::Params::getBlockSize() const
{
  auto it = this->find(BLOCKSIZE);
  if (it == this->end()) {
    return 128; // default value = '128'
  }
  string value = it->second;
  int aux = Parser::intValue(value);
  if (aux <= 0 || USHRT_MAX < aux) {
    throw Exception("parameter '" BLOCKSIZE "' out of range [1," +
                    Format::toString((int)USHRT_MAX) + "]");
  }
  else {
    return (unsigned short) aux;
  }
}

