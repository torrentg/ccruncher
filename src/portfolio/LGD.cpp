
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

#include <cfloat>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cassert>
#include "portfolio/LGD.hpp"
#include "utils/Parser.hpp"

using namespace std;
using namespace ccruncher;

//=============================================================
// supported LGD distributions
//=============================================================
const ccruncher::LGD::Distr ccruncher::LGD::distrs[] = {
  {"uniform", 7, Uniform},
  {"beta", 4, Beta}
};

#define NUMDISTRS (sizeof(distrs)/sizeof(Distr))

/**************************************************************************//**
 * @param[in] cstr String with lgd value/distribution.
 * @throw Exception Invalid exposure.
 */
ccruncher::LGD::LGD(const char *cstr)
{
  assert(cstr != nullptr);
  if (cstr == nullptr) throw Exception("null lgd value");
  
  // triming initial spaces
  while (isspace(*cstr)) cstr++;

  size_t len = strlen(cstr);
  if (strchr(cstr,'(') != nullptr && cstr[len-1] == ')')
  {
    // parsing lgd distribution
    for(size_t i=0; i<NUMDISTRS; i++)
    {
      if (strncmp(cstr, distrs[i].str, distrs[i].len) == 0)
      {
        size_t len_pars = len-distrs[i].len-2;
        if (len_pars >= 256 || *(cstr+distrs[i].len) != '(' || *(cstr+len-1) != ')') {
          throw Exception("invalid lgd value");
        }
        char aux[256];
        memcpy(aux, cstr+distrs[i].len+1, len_pars);
        aux[len_pars] = 0;
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
        value2 = Parser::doubleValue(p+1);
        value1 = Parser::doubleValue(aux);
        init(distrs[i].type, value1, value2);
        return;
      }
    }
  }

  // fixed value case
  value1 = Parser::doubleValue(cstr);
  init(Fixed, value1, NAN);
}

/**************************************************************************//**
 * @param[in] str String with lgd value/distribution.
 * @throw Exception Invalid exposure.
 */
ccruncher::LGD::LGD(const std::string &str)
{
  *this = LGD(str.c_str());
}

/**************************************************************************//**
 * @param[in] t Type of lgd.
 * @param[in] a Distribution parameter.
 * @param[in] b Distribution parameter.
 * @throw Exception Invalid lgd.
 */
ccruncher::LGD::LGD(Type t, double a, double b)
{
  init(t, a, b);
}

/**************************************************************************//**
 * @param[in] t Type of lgd.
 * @param[in] a Distribution parameter.
 * @param[in] b Distribution parameter.
 * @return true=valid, false=invalid.
 */
bool ccruncher::LGD::valid(Type t, double a, double b)
{
  switch(t)
  {
    case Fixed:
      if (a < 0.0 || a > 1.0 || std::isnan(a)) return false;
      else return true;

    case Beta:
      if (a <= 0.0 || b <= 0.0 || std::isnan(a) || std::isnan(b)) return false;
      else return true;

    case Uniform:
      if (a < 0.0 || 1.0 < b || b <= a || std::isnan(a) || std::isnan(b)) return false;
      else return true;

    default:
      assert(false);
      return false;
  }
}

/**************************************************************************//**
 * @param[in] t Type of lgd.
 * @param[in] a Distribution parameter.
 * @param[in] b Distribution parameter.
 * @throw Exception Invalid lgd.
 */
void ccruncher::LGD::init(Type t, double a, double b)
{
  if (t != Fixed || !std::isnan(a))
  {
    if (!valid(t, a, b)) throw Exception("invalid lgd");
  }
  
  type = t;
  value1 = a;
  value2 = b;
}

/**************************************************************************//**
 * @return Exposure type.
 */
ccruncher::LGD::Type ccruncher::LGD::getType() const
{
  return type;
}

/**************************************************************************//**
 * @return Distribution parameter.
 */
double ccruncher::LGD::getValue1() const
{
  return value1;
}

/**************************************************************************//**
 * @return Distribution parameter.
 */
double ccruncher::LGD::getValue2() const
{
  return value2;
}

/**************************************************************************//**
 * @param[in] x EADLGD check.
 * @return true=valid, false=invalid.
 */
bool ccruncher::LGD::isvalid(const LGD &x)
{
  return valid(x.type, x.value1, x.value2);
}

