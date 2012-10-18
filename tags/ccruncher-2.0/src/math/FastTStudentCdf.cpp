
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

#include <cmath>
#include <cstring>
#include "math/FastTStudentCdf.hpp"
#include <gsl/gsl_randist.h>
#include <gsl/gsl_linalg.h>
#include <cassert>

//---------------------------------------------------------------------------

#define EPSILON 1e-7
#define DELTA 0.0001

//===========================================================================
// constructor
// approximates t-Student CDF by cubic splines.
// interpolation points are selected to grant that error is less 
// than EPSILON approx.
// Previous approximation using a lookup-table gives a similar performance
// with more accuracy (~1e-9 using 10000 breaks) but with a lot more of memory 
// requirements
//===========================================================================
FastTStudentCdf::FastTStudentCdf(double nu) throw(Exception) : ndf(nu)
{
  if (nu < 2.0) throw Exception("ndf out-of-range (< 2)");

  double xtop = gsl_cdf_tdist_Pinv(1.0-DELTA, ndf);
  double x0 = 0.0;

  do
  {
    int n = breaks.size();
    coeffs.insert(coeffs.end(), 4, 0.0);
    double *a = &coeffs[n*4];
    double x1 = bisection(x0, a);
    breaks.push_back(x1);
    x0 = x1;
  }
  while(x0 < xtop);
}

//===========================================================================
// compute interpolating polynomial of degree 3
// returns an estimation of the maximum error
//===========================================================================
double FastTStudentCdf::getCoeffs3(double x1, double x2, double *ret) const throw(Exception)
{
  assert(ret != NULL);

  double Mv[16];
  double Bv[4];

  // P(x1) = F(x1)
  Mv[0] = 1.0;
  Mv[1] = x1;
  Mv[2] = x1*x1;
  Mv[3] = x1*x1*x1;
  Bv[0] = gsl_cdf_tdist_P(x1, ndf);

  // P'(x1) = F'(x1) = f(x1)
  Mv[4] = 0.0;
  Mv[5] = 1.0;
  Mv[6] = 2.0*x1;
  Mv[7] = 3.0*x1*x1;
  Bv[1] = gsl_ran_tdist_pdf(x1, ndf);

  // P(x2) = F(x2)
  Mv[8] = 1.0;
  Mv[9] = x2;
  Mv[10] = x2*x2;
  Mv[11] = x2*x2*x2;
  Bv[2] = gsl_cdf_tdist_P(x2, ndf);

  // P'(x2) = F'(x2) = f(x2)
  Mv[12] = 0.0;
  Mv[13] = 1.0;
  Mv[14] = 2.0*x2;
  Mv[15] = 3.0*x2*x2;
  Bv[3] = gsl_ran_tdist_pdf(x2, ndf);

  int s;
  gsl_matrix_view M = gsl_matrix_view_array(Mv, 4, 4);
  gsl_vector_view B = gsl_vector_view_array(Bv, 4);
  gsl_vector *X = gsl_vector_alloc(4);
  gsl_permutation *P = gsl_permutation_alloc(4);
  int rc1 = gsl_linalg_LU_decomp(&M.matrix, P, &s);
  if (rc1 != 0) throw Exception("error solving interpolating polynomial coefficients");
  int rc2 = gsl_linalg_LU_solve(&M.matrix, P, &B.vector, X);
  if (rc2 != 0) throw Exception("error solving interpolating polynomial coefficients");
  for(int i=0; i<4; i++) ret[i] = gsl_vector_get(X,i);
  gsl_permutation_free(P);
  gsl_vector_free(X);

  // estimating maximum error
  double max_err = 0.0;
  double x = x1;
  double delta = (x2-x1)/1000.0;
  if (delta < 0.001) delta = 0.001;

  while(x < x2)
  {
    double approx = ret[0]+x*(ret[1]+x*(ret[2]+x*ret[3]));
    double exact = gsl_cdf_tdist_P(x, ndf);
    double err = fabs(approx-exact);
    if (max_err < err) max_err = err;
    x += delta;
  }

  return max_err;
}

//===========================================================================
// sign
//===========================================================================
inline int FastTStudentCdf::sign(double x) const
{
  if (x < 0.0) return -1;
  else return +1;
}

//===========================================================================
// bisection
//===========================================================================
double FastTStudentCdf::bisection(double x0, double *ret) const throw(Exception)
{
  assert(ret != NULL);

  double a, b, c;
  double fa, fb, fc;
  double xa[4], xb[4], xc[4];
  int iter=0;

  a = x0 + DELTA;
  fa = getCoeffs3(x0, a, xa) - EPSILON;
  if (fa > 0.0) throw Exception("DELTA too high");

  b = x0;
  fb = -1.0;
  while (fb < 0.0)
  {
    b += 1.0;
    fb = getCoeffs3(x0, b, xb) - EPSILON;
  }

  assert(sign(fa) != sign(fb));

  do
  {
    c = (a+b)/2.0;
    fc = getCoeffs3(x0, c, xc) - EPSILON;

    if(sign(fa) != sign(fc)) 
    {
       b = c;
       memcpy(xb, xc, sizeof(double)*4);
    }
    else
    {
       a = c;
       memcpy(xa, xc, sizeof(double)*4);
    }

    iter++;
    assert(sign(fa) != sign(fb));
  }
  while((fabs(fc)>1e-10) && (iter<100));

  if (iter >= 100) throw Exception("convergence not achieved");
  
  memcpy(ret, xc, sizeof(double)*4);
  return c;
}

