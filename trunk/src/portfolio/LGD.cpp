
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
#include "portfolio/LGD.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

//===========================================================================
// constructor
//===========================================================================
ccruncher::LGD::LGD(const char *cstr) throw(Exception)
{
  if (cstr == NULL) throw Exception("null lgd value");
  
  // triming initial spaces
  while (isspace(*cstr)) cstr++;

  // parsing lgd value
  if (!isalpha(*cstr))
  {
    value1 = Parser::doubleValue(cstr);
    init(Fixed, value1, NAN);
  }
  else if (strncmp(cstr, "beta", 4) == 0)
  {
    int rc = sscanf(cstr, "beta(%lf,%lf)", &value1, &value2);
    if (rc != 2) 
    {
      throw Exception("invalid lgd value");
    }
    init(Beta, value1, value2);
  }
  else if (strncmp(cstr, "uniform", 7) == 0)
  {
    int rc = sscanf(cstr, "uniform(%lf,%lf)", &value1, &value2);
    if (rc != 2) {
      throw Exception("invalid lgd value");
    }
    init(Uniform, value1, value2);
  }
  else
  {
    throw Exception("invalid lgd value");
  }
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::LGD::LGD(const string &str) throw(Exception)
{
  *this = LGD(str.c_str());
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::LGD::LGD(Type t, double a, double b) throw(Exception)
{
  init(t, a, b);
}

//===========================================================================
// valid
//===========================================================================
bool ccruncher::LGD::valid(Type t, double a, double b)
{
  switch(t)
  {
    case Fixed:
      if (a < 0.0 || a > 1.0 || isnan(a)) return false;
      else return true;

    case Beta:
      if (a <= 0.0 || b <= 0.0 || isnan(a) || isnan(b)) return false;
      else return true;

    case Uniform:
      if (a < 0.0 || 1.0 < b || b <= a || isnan(a) || isnan(b)) return false;
      else return true;

    default:
      assert(false);
      return false;
  }
}

//===========================================================================
// init
//===========================================================================
void ccruncher::LGD::init(Type t, double a, double b) throw(Exception)
{
  if (t != Fixed || !isnan(a))
  {
    if (!valid(t, a, b)) throw Exception("invalid lgd parameters");
  }
  
  type = t;
  value1 = a;
  value2 = b;
}

//===========================================================================
// retuns value1
//===========================================================================
double ccruncher::LGD::getValue1() const
{
  return value1;
}

//===========================================================================
// returns value2
//===========================================================================
double ccruncher::LGD::getValue2() const
{
  return value2;
}

//===========================================================================
// check if is a Non-A-LGD value
//===========================================================================
bool ccruncher::LGD::isvalid(const LGD &x)
{
  return valid(x.type, x.value1, x.value2);
}
