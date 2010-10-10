
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#include <cmath>
#include <cfloat>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <gsl/gsl_randist.h>
#include "portfolio/Recovery.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include <cassert>

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Recovery::Recovery()
{
  *this = getNAN();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Recovery::Recovery(const char *cstr) throw(Exception)
{
  assert(cstr != NULL);
  
  // triming initial spaces
  while (isspace(*cstr)) cstr++;

  // parsing recovery value  
  if (strncmp(cstr, "beta", 4) == 0)
  {
    int rc = sscanf(cstr, "beta(%lf,%lf)", &value1, &value2);
    if (rc != 2) 
    {
      throw Exception("invalid recovery value");
    }
    init(Beta, value1, value2);
  }
  else
  {
    value1 = Parser::doubleValue(cstr);
    init(Fixed, value1, NAN);
  }
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Recovery::Recovery(const string &str) throw(Exception)
{
  *this = Recovery(str.c_str());
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Recovery::Recovery(RecoveryType t, double a, double b) throw(Exception)
{
  init(t, a, b);
}

//===========================================================================
// constructor
// caution: not checked that 0<= val <= 1
//===========================================================================
ccruncher::Recovery::Recovery(double val) throw(Exception)
{
  init(Fixed, val, NAN);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Recovery::Recovery(double a, double b) throw(Exception)
{
  init(Beta, a, b);
}

//===========================================================================
// init
//===========================================================================
void ccruncher::Recovery::init(RecoveryType t, double a, double b) throw(Exception)
{
  if (t == Fixed)
  {
    if (a < 0.0 || 1.0 < a) {
      throw Exception("recovery value out of range [0%,100%]");
    }
    else {
      type = t;
      value1 = a;
      value2 = NAN;
    }
  }
  else if (t == Beta) {
    if (a <= 0.0 || b <= 0.0) {
      throw Exception("beta arguments out of range (0,inf)");
    }
    else {
      type = t;
      value1 = a;
      value2 = b;
    }
  }
  else {
    throw Exception("unknow recovery type");
  }
}

//===========================================================================
// getValue
//===========================================================================
double ccruncher::Recovery::getValue() const
{
  if (type == Beta) return NAN;
  else return value1;
}

//===========================================================================
// getValue
//===========================================================================
double ccruncher::Recovery::getValue(const gsl_rng *rng) const
{
  if (type == Fixed) return value1;
  else return gsl_ran_beta(rng, value1, value2);
}

//===========================================================================
// returns type
//===========================================================================
RecoveryType ccruncher::Recovery::getType() const
{
  return type;
}

//===========================================================================
// retuns value1
//===========================================================================
double ccruncher::Recovery::getValue1() const
{
  return value1;
}

//===========================================================================
// returns value2
//===========================================================================
double ccruncher::Recovery::getValue2() const
{
  return value2;
}

//===========================================================================
// returns a Non-A-Recovery value (Fixed & NAN)
//===========================================================================
Recovery ccruncher::Recovery::getNAN()
{
  Recovery ret(0.0);
  ret.value1 = NAN;
  ret.value2 = NAN;
  return ret;
}

//===========================================================================
// check if is a Non-A-Recovery value
//===========================================================================
bool ccruncher::Recovery::isnan(Recovery x)
{
  if (x.type == Fixed && std::isnan(x.value1)) return true;
  else return false;
}

//===========================================================================
// to string
//===========================================================================
string ccruncher::Recovery::toString() const
{
  if (type == Fixed) return Format::toString(value1*100.0) + "%";
  else return "beta("+Format::toString(value1)+","+Format::toString(value2)+")";
}

