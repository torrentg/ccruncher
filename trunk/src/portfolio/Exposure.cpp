
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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
#include "portfolio/Exposure.hpp"
#include "utils/Parser.hpp"
#include "utils/Format.hpp"
#include <cassert>

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Exposure::Exposure()
{
  type = Fixed;
  value1 = NAN;
  value2 = NAN;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Exposure::Exposure(const char *cstr) throw(Exception)
{
  assert(cstr != NULL);
  
  // triming initial spaces
  while (isspace(*cstr)) cstr++;

  // parsing exposure value  
  if (strncmp(cstr, "lognormal", 9) == 0)
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
  else
  {
    value1 = Parser::doubleValue(cstr);
    init(Fixed, value1, NAN);
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
// init
//===========================================================================
void ccruncher::Exposure::init(ExposureType t, double a, double b) throw(Exception)
{
  if (t == Fixed && (a < 0.0 || isnan(a))) {
    throw Exception("exposure value out of range");
  }
  else if (t == Lognormal && (a < 0.0 || b <= 0.0 || isnan(a) || isnan(b))) {
    throw Exception("lognormal parameters out of range");
  }
  else if (t == Exponential && (a <= 0.0 || isnan(a))) {
    throw Exception("exponential parameter out of range");
  }
  else if (t == Uniform && (a < 0.0 || 1.0 < b || b <= a || isnan(a) || isnan(b))) {
    throw Exception("uniform parameters out of range");
  }
  else if (t == Gamma && (a <= 0.0 || b <= 0.0 || isnan(a) || isnan(b))) {
    throw Exception("gamma parameters out of range");
  }
  else if (type < 1 || type > 5) {
    throw Exception("unknow exposure type");
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
// returns a Non-A-Exposure value (Fixed & NAN)
//===========================================================================
Exposure ccruncher::Exposure::getNAN()
{
  return Exposure();
}

//===========================================================================
// check if is a Non-A-Exposure value
//===========================================================================
bool ccruncher::Exposure::valid(const Exposure &x)
{
  if (x.type == Fixed && isnan(x.value1)) return true;
  else return false;
}

//===========================================================================
// to string
//===========================================================================
string ccruncher::Exposure::toString() const
{
  switch(type)
  {
    case Fixed:
      return Format::toString(value1);
    case Lognormal:
      return "lognormal(" + Format::toString(value1) + "," + Format::toString(value2) + ")";
    case Exponential:
      return "exponential(" + Format::toString(value1) + ")";
    case Uniform:
      return "uniform(" + Format::toString(value1) + "," + Format::toString(value2) + ")";
    case Gamma:
      return "gamma(" + Format::toString(value1) + "," + Format::toString(value2) + ")";
    default:
      return "NAN";
  }
}
