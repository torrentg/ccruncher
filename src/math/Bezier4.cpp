
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include "math/Bezier4.hpp"
#include <gsl/gsl_poly.h>
#include <cassert>

#define EPSILON 1e-12

//===========================================================================
// Description:
// ------------
// Quadratic Bezier curve that maps [0,1] to [0,1] defined by points:
// P0=(0,0), P1=(a,0), P2=(b,1), P3=(1,1)
// and that interpole point (p,p), where p in (0,1)
// see http://en.wikipedia.org/wiki/B%C3%A9zier_curve
//
// B(t) = P0·(1-t)^3 + P1·3·t·(1-t)^2 + P2·3·t^2·(1-t) + P3·t^3
// Bx(t) = 3a·t + 3(b-2a)·t^2 + (3a-3b+1)·t^3
// By(t) = -2·t^2 + 3·t^3
//
//===========================================================================

//=============================================================
// constructor
//=============================================================
ccruncher::Bezier4::Bezier4(double p_, double b_) throw(Exception) : p(p_), b(b_)
{
  int num_roots;
  double r[3];

  // checking if reversed case
  if (b > 1.0) {
    b = 2.0 - b;
    reversed = true;
  }
  else {
    reversed = false;
  }

  // checking feasebility
  if (p < 0.0 || p > 1.0) {
    throw Exception("p value out-of-range");
  }

  if (p < EPSILON || p > 1.0-EPSILON)
  {
    a = 0.0;
  }
  else
  {
    // searching t0 where By(t0)=p
    double t0 = -1.0;
    double r[3];
    int num_roots = gsl_poly_solve_cubic(-3.0/2.0, 0.0, p/2.0, r+0, r+1, r+2);
    for(int i=0; i<num_roots; i++)
    {
      if (r[i] > 0.0 && r[i] < 1.0) {
        t0 = r[i];
        break;
      }
    }
    assert(t0 > 0.0);

    // fixing a
    a = p/(3.0*t0*(1-t0)) - b*t0/(1-t0) - t0*t0/(3.0*(1-t0)*(1-t0));
  }

  // checking if well-defined, Bx'(t)!=0 in t=(0,1)
  num_roots = gsl_poly_solve_quadratic(3*(3*a-3*b+1), 6*(b-2*a), 3*a, r+0, r+1);
  for(int i=0; i<num_roots; i++)
  {
    if (r[i] >= 0.0 && r[i] <= 1.0)
    {
      throw Exception("non feasible curve");
    }
  }
}

//=============================================================
// return By(t) where Bx(t)=x
//=============================================================
double ccruncher::Bezier4::eval(double x)
{
  double r[3];
  double k = 3*a-3*b+1;
  double t=-1.0;

  // search t where Bx(t)=x
  int num_roots = gsl_poly_solve_cubic(3*(b-2*a)/k, 3*a/k, -x/k, r+0, r+1, r+2);
  for(int i=0; i<num_roots; i++)
  {
    if (r[i] >= 0.0 && r[i] <= 1.0) {
      t = r[i];
      break;
    }
  }
  assert(t >= 0.0);

  // eval By(t)
  double y = -2.0*t*t*t + 3.0*t*t;

  if (reversed) return 2.0*x-y;
  else return y;
}
