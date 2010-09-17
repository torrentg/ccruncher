
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
#include "portfolio/Recovery.hpp"
#include "utils/Parser.hpp"
#include <cassert>

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Recovery::Recovery()
{
  type = Fixed;
  value1 = NAN;
  value2 = NAN;
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
    if (value1 <= 0.0 || value2 <= 0.0)
    {
      throw Exception("invalid beta arguments");
    }
    type = Beta;
  }
  else
  {
    value1 = Parser::doubleValue(cstr);
    type = Fixed;
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
ccruncher::Recovery::Recovery(RecoveryType t, double a, double b)
{
  type = t;
  value1 = a;
  value2 = b;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Recovery::Recovery(double val)
{
  type = Fixed;
  value1 = val;
  value2 = NAN;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Recovery::Recovery(double a, double b)
{
  type = Beta;
  value1 = a;
  value2 = b;
}

