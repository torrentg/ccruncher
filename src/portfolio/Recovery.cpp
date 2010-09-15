
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
#include "portfolio/Recovery.hpp"

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
ccruncher::Recovery::Recovery(const string &str) throw(Exception)
{
  //TODO: parse beta(x,y) or double value
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

