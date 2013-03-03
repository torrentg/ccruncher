
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

#include <cfloat>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cmath>
#include "portfolio/Exposure.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include <cassert>

using namespace std;

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exposure::Exposure(const char *cstr) throw(Exception)
{
  if (cstr == NULL) throw Exception("null exposure value");

  // triming initial spaces
  while (isspace(*cstr)) cstr++;

  // parsing exposure value
  if (!isalpha(*cstr))
  {
    value1 = Parser::doubleValue(cstr);
    init(Fixed, value1, NAN);
  }
  else if (strncmp(cstr, "lognormal", 9) == 0)
  {
    int rc = sscanf(cstr, "lognormal(%lf,%lf)", &value1, &value2);
    if (rc != 2) {
      throw Exception("invalid exposure value");
    }
    init(Lognormal, value1, value2);
  } 
  else if (strncmp(cstr, "exponential", 11) == 0)
  {
    int rc = sscanf(cstr, "exponential(%lf)", &value1);
    if (rc != 1) {
      throw Exception("invalid exposure value");
    }
    init(Exponential, value1, NAN);
  }
  else if (strncmp(cstr, "uniform", 7) == 0)
  {
    int rc = sscanf(cstr, "uniform(%lf,%lf)", &value1, &value2);
    if (rc != 2) {
      throw Exception("invalid exposure value");
    }
    init(Uniform, value1, value2);
  }
  else if (strncmp(cstr, "gamma", 5) == 0)
  {
    int rc = sscanf(cstr, "gamma(%lf,%lf)", &value1, &value2);
    if (rc != 2) {
      throw Exception("invalid exposure value");
    }
    init(Gamma, value1, value2);
  }
  else if (strncmp(cstr, "normal", 6) == 0)
  {
    int rc = sscanf(cstr, "normal(%lf,%lf)", &value1, &value2);
    if (rc != 2) {
      throw Exception("invalid exposure value");
    }
    init(Normal, value1, value2);
  }
  else
  {
    throw Exception("invalid exposure value");
  }
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exposure::Exposure(const string &str) throw(Exception)
{
  *this = Exposure(str.c_str());
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exposure::Exposure(ExposureType t, double a, double b) throw(Exception)
{
  init(t, a, b);
}

//===========================================================================
// valid
//===========================================================================
bool ccruncher::Exposure::valid(ExposureType t, double a, double b)
{
  switch(t)
  {
    case Fixed:
      if (a < 0.0 || isnan(a)) return false;
      else return true;

    case Lognormal:
      if (a < 0.0 || b <= 0.0 || isnan(a) || isnan(b)) return false;
      else return true;

    case Exponential:
      if (a <= 0.0 || isnan(a)) return false;
      else return true;

    case Uniform:
      if (a < 0.0 || b <= a || isnan(a) || isnan(b)) return false;
      else return true;

    case Gamma:
      if (a <= 0.0 || b <= 0.0 || isnan(a) || isnan(b)) return false;
      else return true;

    case Normal:
      if (a <= 0.0 || b <= 0.0 || isnan(a) || isnan(b)) return false;
      else return true;

    default:
      assert(false);
      return false;
  }
}

//===========================================================================
// init
//===========================================================================
void ccruncher::Exposure::init(ExposureType t, double a, double b) throw(Exception)
{
  if (t != Fixed || !isnan(a))
  {
    if (!valid(t, a, b)) throw Exception("invalid exposure parameters");
  }

  type = t;
  value1 = a;
  value2 = b;
}

//===========================================================================
// retuns value1
//===========================================================================
double ccruncher::Exposure::getValue1() const
{
  return value1;
}

//===========================================================================
// returns value2
//===========================================================================
double ccruncher::Exposure::getValue2() const
{
  return value2;
}

//===========================================================================
// check if is a Non-A-Exposure value
//===========================================================================
bool ccruncher::Exposure::isvalid(const Exposure &x)
{
  return valid(x.type, x.value1, x.value2);
}

//===========================================================================
// apply current net value factor to exposure
//===========================================================================
void ccruncher::Exposure::mult(double factor)
{
  switch(type)
  {
    case Fixed:
      value1 *= factor;
      break;

    case Lognormal:
      // http://en.wikipedia.org/wiki/Log-normal_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html
      value1 += log(factor);
      break;

    case Exponential:
      // http://en.wikipedia.org/wiki/Exponential_distribution (<-not)
      // http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html (<-this)
      value1 *= factor;
      break;

    case Uniform:
      // http://en.wikipedia.org/wiki/Uniform_distribution_%28continuous%29
      // http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html
      value1 *= factor;
      value2 *= factor;
      break;

    case Gamma:
      // http://en.wikipedia.org/wiki/Gamma_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html
      value2 *= factor;
      break;

    case Normal:
      // http://en.wikipedia.org/wiki/Normal_distribution
      // http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html
      value1 *= factor;
      value2 *= factor*factor;
      break;

    default:
      assert(false);
      break;
  }
}

