
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

#include <cfloat>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <gsl/gsl_cdf.h>
#include "portfolio/EAD.hpp"
#include "utils/Parser.hpp"
#include "utils/Exception.hpp"

#define EPSILON 1e-14

using namespace std;
using namespace ccruncher;

//=============================================================
// supported EAD distributions
//=============================================================
const ccruncher::EAD::Distr ccruncher::EAD::distrs[] = {
  {"lognormal", 9, 2, Lognormal},
  {"exponential", 11, 1, Exponential},
  {"uniform", 7, 2, Uniform},
  {"gamma", 5, 2, Gamma},
  {"normal", 6, 2, Normal}
};

#define NUMDISTRS (sizeof(distrs)/sizeof(Distr))

/**************************************************************************//**
 * @param[in] cstr String with exposure value/distribution.
 * @throw Exception Invalid exposure.
 */
ccruncher::EAD::EAD(const char *cstr)
{
  assert(cstr != nullptr);
  if (cstr == nullptr) throw Exception("null ead value");

  // triming initial spaces
  while (isspace(*cstr)) cstr++;

  size_t len = strlen(cstr);
  if (strchr(cstr,'(') != nullptr && cstr[len-1] == ')')
  {
    // parsing ead distribution
    for(size_t i=0; i<NUMDISTRS; i++)
    {
      if (strncmp(cstr, distrs[i].str, distrs[i].len) == 0)
      {
        size_t len_pars = len-distrs[i].len-2;
        if (len_pars >= 256 || *(cstr+distrs[i].len) != '(' || *(cstr+len-1) != ')') {
          throw Exception("invalid ead value");
        }
        char aux[256];
        memcpy(aux, cstr+distrs[i].len+1, len_pars);
        aux[len_pars] = 0;
        if (distrs[i].nargs > 1) {
          char *p = aux-1;
          size_t num_pars = 0;
          while (*(++p) != 0) {
            if (*p == '(') { num_pars++; continue; }
            else if (*p == ')' && num_pars > 0) { num_pars--; continue; }
            else if (*p == ',' && num_pars == 0) { *p = 0; break; }
          }
          if (p == aux+len_pars) {
            throw Exception("invalid number of arguments");
          }
          mValue2 = Parser::doubleValue(p+1);
        }
        mValue1 = Parser::doubleValue(aux);
        if (distrs[i].nargs == 1) {
          init(distrs[i].type, mValue1, NAN);
        }
        else {
          init(distrs[i].type, mValue1, mValue2);
        }
        return;
      }
    }
  }

  // fixed value case
  mValue1 = Parser::doubleValue(cstr);
  init(Fixed, mValue1, NAN);
}

/**************************************************************************//**
 * @param[in] str String with exposure value/distribution.
 * @throw Exception Invalid exposure.
 */
ccruncher::EAD::EAD(const std::string &str)
{
  *this = EAD(str.c_str());
}

/**************************************************************************//**
 * @param[in] t Type of exposure.
 * @param[in] a Distribution parameter.
 * @param[in] b Distribution parameter.
 * @throw Exception Invalid exposure.
 */
ccruncher::EAD::EAD(Type t, double a, double b)
{
  init(t, a, b);
}

/**************************************************************************//**
 * @param[in] o EAD instance to compare.
 * @return true=are equal, false=otherwise.
 */
bool ccruncher::EAD::operator==(const EAD &o) const
{
  if (mType != o.mType) return false;
  else if (!std::isnan(mValue1) && std::abs(mValue1-o.mValue1) > EPSILON) return false;
  else if (!std::isnan(mValue2) && std::abs(mValue2-o.mValue2) > EPSILON) return false;
  else return true;
}

/**************************************************************************//**
 * @param[in] o EAD instance to compare.
 * @return true=are distinct, false=otherwise.
 */
bool ccruncher::EAD::operator!=(const EAD &o) const
{
  return !(*this == o);
}

/**************************************************************************//**
 * @param[in] t Type of exposure.
 * @param[in] a Distribution parameter.
 * @param[in] b Distribution parameter.
 * @return true=valid, false=invalid.
 */
bool ccruncher::EAD::valid(Type t, double a, double b)
{
  switch(t)
  {
    case Fixed:
      if (std::isnan(a) || a < 0.0) return false;
      else return true;

    case Lognormal:
      if (std::isnan(a) || std::isnan(b) || a < 0.0 || b <= 0.0) return false;
      else return true;

    case Exponential:
      if (std::isnan(a) || a <= 0.0) return false;
      else return true;

    case Uniform:
      if (std::isnan(a) || std::isnan(b) || a < 0.0 || b <= a) return false;
      else return true;

    case Gamma:
      if (std::isnan(a) || std::isnan(b) || a <= 0.0 || b <= 0.0) return false;
      else return true;

    case Normal:
      if (std::isnan(a) || std::isnan(b) || a <= 0.0 || b <= 0.0) return false;
      else return true;

    default:
      assert(false);
      return false;
  }
}

/**************************************************************************//**
 * @param[in] t Type of exposure.
 * @param[in] a Distribution parameter.
 * @param[in] b Distribution parameter.
 * @throw Exception Invalid exposure.
 */
void ccruncher::EAD::init(Type t, double a, double b)
{
  if (t != Fixed || !std::isnan(a))
  {
    if (!valid(t, a, b)) throw Exception("invalid ead");
  }

  mType = t;
  mValue1 = a;
  mValue2 = b;
}

/**************************************************************************//**
 * @return Exposure type.
 */
ccruncher::EAD::Type ccruncher::EAD::getType() const
{
  return mType;
}

/**************************************************************************//**
 * @return Distribution parameter.
 */
double ccruncher::EAD::getValue1() const
{
  return mValue1;
}

/**************************************************************************//**
 * @return Distribution parameter.
 */
double ccruncher::EAD::getValue2() const
{
  return mValue2;
}

/**************************************************************************//**
 * @param[in] x EAD to check.
 * @return true=valid, false=invalid.
 */
bool ccruncher::EAD::isvalid(const EAD &x)
{
  return valid(x.mType, x.mValue1, x.mValue2);
}

/**************************************************************************//**
 * @details Apply multiplicative factor directly if ead is of fixed type,
 *          if it is a distribution, modifies parameters in order to achieve
 *          the same effect.
 * @param[in] factor Multiplicative factor.
 */
void ccruncher::EAD::mult(double factor)
{
  switch(mType)
  {
    case Fixed:
      mValue1 *= factor;
      break;

    case Lognormal:
      // http://en.wikipedia.org/wiki/Log-normal_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html
      mValue1 += log(factor);
      break;

    case Exponential:
      // http://en.wikipedia.org/wiki/Exponential_distribution (<-not)
      // http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html (<-this)
      mValue1 *= factor;
      break;

    case Uniform:
      // http://en.wikipedia.org/wiki/Uniform_distribution_%28continuous%29
      // http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html
      mValue1 *= factor;
      mValue2 *= factor;
      break;

    case Gamma:
      // http://en.wikipedia.org/wiki/Gamma_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html
      mValue2 *= factor;
      break;

    case Normal:
      // http://en.wikipedia.org/wiki/Normal_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html
      mValue1 *= factor;
      mValue2 *= factor*factor;
      break;

    default:
      assert(false);
      break;
  }
}

/**************************************************************************//**
 * @details If exposure is a fixed value, mean coincides with this value,
 *          but if exposure is a distribution, then it return the mean
 *          of this distribution.
 * @return Exposure expected value.
 */
double ccruncher::EAD::getExpected() const
{
  switch(mType)
  {
    case Fixed:
      return mValue1;

    case Lognormal:
      // http://en.wikipedia.org/wiki/Log-normal_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html
      return exp(mValue1+mValue2*mValue2/2.0);

    case Exponential:
      // http://en.wikipedia.org/wiki/Exponential_distribution (<-not)
      // http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html (<-this)
      return mValue1;

    case Uniform:
      // http://en.wikipedia.org/wiki/Uniform_distribution_%28continuous%29
      // http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html
      return (mValue1+mValue2)/2.0;

    case Gamma:
      // http://en.wikipedia.org/wiki/Gamma_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html
      return mValue1*mValue2;

    case Normal:
      // http://en.wikipedia.org/wiki/Normal_distribution
      // http://en.wikipedia.org/wiki/Truncated_normal_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html
      {
        double x = -mValue1/mValue2;
        double v1 = gsl_ran_ugaussian_pdf(x);
        double v2 = gsl_cdf_ugaussian_P(x);
        return mValue1 + mValue2*v1/(1.0-v2);
      }

    default:
      assert(false);
      return NAN;
  }
}

