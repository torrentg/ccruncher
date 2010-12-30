
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

#include "utils/LookupTable.hpp"
#include <cassert>

// --------------------------------------------------------------------------

//===========================================================================
// default constructor
//===========================================================================
ccruncher::LookupTable::LookupTable()
{
  vector<double> values_;
  values_.push_back(0.0);
  values_.push_back(0.0);
  init(0.0, 1.0, values_);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::LookupTable::LookupTable(double xmin_, double xmax_, const vector<double> &values_) throw(Exception)
{
  init(xmin_, xmax_, values_);
}

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::LookupTable::LookupTable(const LookupTable &x) throw(Exception)
{
  *this = x;
}

//===========================================================================
// init
//===========================================================================
void ccruncher::LookupTable::init(double xmin_, double xmax_, const vector<double> &values_) throw(Exception)
{
  if (xmin_ >= xmax_ || values_.size() <= 1)
  {
    throw Exception("invalid arguments at lookup table");
  }
  xmin = xmin_;
  xmax = xmax_;
  values = values_;
  // precompute slopes required by interpolation
  // interpolation formula: y = y0 + (x-x0) x (y1-y0)/(x1-x0)
  // precomputed slope: (y1-y0)/(x1-x0)
  int numsteps = values.size();
  steplength = (xmax-xmin)/(double)(numsteps-1);
  for(int i=0; i<numsteps-1; i++)
  {
    double aux = (values[i+1]-values[i])/steplength;
    slope.push_back(aux);
  }
}

//===========================================================================
// size
//===========================================================================
int ccruncher::LookupTable::size() const
{
  return values.size();
}

/*
  Notes for interpolation with derivatives (Mathematica):
  > f2[x_] := a + b*x + c*x^2 + d*x^3
  > Solve[{f2[x1] == y1, f2[x2] == y2, f2'[x1] == p1, f2'[x2] == p2}, {a, b, c, d}]
  > FullSimplify[f2[x] /. %]
  > FullSimplify[Coefficient[%4, x, 1]]
*/

