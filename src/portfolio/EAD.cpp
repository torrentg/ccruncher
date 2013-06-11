
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
#include "portfolio/EAD.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include <cassert>

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

//===========================================================================
// constructor
//===========================================================================
ccruncher::EAD::EAD(const char *cstr) throw(Exception)
{
  assert(cstr != NULL);
  if (cstr == NULL) throw Exception("null ead value");

  // triming initial spaces
  while (isspace(*cstr)) cstr++;

  // parsing ead distribution
  for(size_t i=0; i<NUMDISTRS; i++)
  {
      if (strncmp(cstr, distrs[i].str, distrs[i].len) == 0)
    {
      size_t len = strlen(cstr);
      if (len-distrs[i].len-2 >= 256 || *(cstr+distrs[i].len) != '(' || *(cstr+len-1) != ')') {
        throw Exception("invalid ead value");
      }
      char aux[256];
      memcpy(aux, cstr+distrs[i].len+1, len-distrs[i].len-2);
      len -= distrs[i].len + 2;
      aux[len] = 0;
      if (distrs[i].nargs > 1) {
        char *p = aux-1;
        size_t num_pars = 0;
        while (*(++p) != 0) {
          if (*p == '(') { num_pars++; continue; }
          else if (*p == ')' && num_pars > 0) { num_pars--; continue; }
          else if (*p == ',' && num_pars == 0) { *p = 0; break; }
        }
        if (p == aux+len) {
          throw Exception("invalid number of arguments");
        }
        value2 = Parser::doubleValue(p+1);
      }
      value1 = Parser::doubleValue(aux);
      if (distrs[i].nargs == 1) {
        init(distrs[i].type, value1, NAN);
      }
      else {
        init(distrs[i].type, value1, value2);
      }
      return;
    }
  }

  // fixed value case
  value1 = Parser::doubleValue(cstr);
  init(Fixed, value1, NAN);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::EAD::EAD(const string &str) throw(Exception)
{
  *this = EAD(str.c_str());
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::EAD::EAD(Type t, double a, double b) throw(Exception)
{
  init(t, a, b);
}

//===========================================================================
// valid
//===========================================================================
bool ccruncher::EAD::valid(Type t, double a, double b)
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
void ccruncher::EAD::init(Type t, double a, double b) throw(Exception)
{
  if (t != Fixed || !isnan(a))
  {
    if (!valid(t, a, b)) throw Exception("invalid ead parameters");
  }

  type = t;
  value1 = a;
  value2 = b;
}

//===========================================================================
// retuns value1
//===========================================================================
double ccruncher::EAD::getValue1() const
{
  return value1;
}

//===========================================================================
// returns value2
//===========================================================================
double ccruncher::EAD::getValue2() const
{
  return value2;
}

//===========================================================================
// check if is a Non-A-EAD value
//===========================================================================
bool ccruncher::EAD::isvalid(const EAD &x)
{
  return valid(x.type, x.value1, x.value2);
}

//===========================================================================
// apply current net value factor to ead
//===========================================================================
void ccruncher::EAD::mult(double factor)
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

