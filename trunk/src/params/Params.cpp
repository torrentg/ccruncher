
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#include <limits>
#include <cassert>
#include "params/Params.hpp"
#include "utils/Exception.hpp"
#include "utils/Parser.hpp"

#define TIME0 "time.0"
#define TIMET "time.T"
#define MAXITERATIONS "maxiterations"
#define MAXSECONDS "maxseconds"
#define COPULA "copula"
#define RNGSEED "rng.seed"
#define ANTITHETIC "antithetic"
#define BLOCKSIZE "blocksize"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] name Parameter name.
 * @param[in] value Parameter value.
 * @exception Exception Invalid parameter.
 */
void ccruncher::Params::setParamValue(const string &name, const string &value)
{
  if (name == TIME0) {
    setTime0(Parser::dateValue(value));
  }
  else if (name == TIMET) {
    setTimeT(Parser::dateValue(value));
  }
  else if (name == MAXITERATIONS) {
    setMaxIterations(Parser::ulongValue(value));
  }
  else if (name == MAXSECONDS) {
    setMaxSeconds(Parser::ulongValue(value));
  }
  else if (name == RNGSEED) {
    setRngSeed(Parser::ulongValue(value));
  }
  else if (name == BLOCKSIZE) {
    int num = Parser::intValue(value);
    int max = numeric_limits<unsigned short>::max();
    if (num <= 0 || max < num) {
      throw Exception("parameter '" BLOCKSIZE "' out of range [1," + to_string(max) + "]");
    }
    setBlockSize(static_cast<unsigned short>(num));
  }
  else if (name == COPULA) {
    setCopula(value);
  }
  else if (name == ANTITHETIC) {
    setAntithetic(Parser::boolValue(value));
  }
  else {
    throw Exception("unexpected parameter '" + name + "'");
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
    if (time0 == NAD) {
      throw Exception(TIME0 " not set");
    }

    if (timeT == NAD) {
      throw Exception(TIMET " not set");
    }

    if (time0 >= timeT) {
      throw Exception(TIME0 " >= " TIMET);
    }

    if (timeT.getYear()-time0.getYear() > 101) {
      throw Exception("more than 100 years between " TIME0 " and " TIMET);
    }

    getNdf();

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
 * @details Allowed copulas are: 'gaussian' and 't(ndf)' where ndf is a number >=2.
 * @param[in] str Copula type.
 * @throw Exception Invalid copula type.
 */
void ccruncher::Params::setCopula(const string &str)
{
  copula = str;
  getNdf();
}

/**************************************************************************//**
 * @return Degrees of freedom of the t-copula (ndf>=2) or +INF if gaussian.
 * @throw Exception Invalid parameter value.
 */
double ccruncher::Params::getNdf() const
{
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

