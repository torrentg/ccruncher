
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// Normal.cpp - Normal code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/06/28 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added a new cdf function more acurate (18 digits) extracted from
//     R code. Comented because is more expensive (in computation time)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2007/08/06 - Gerard Torrent [gerard@fobos.generacio.com]
//   . cdf(x) and pdf(x) converted to inline
//
//===========================================================================

#include <cmath>
#include <cstdlib>
#include "math/Normal.hpp"

//---------------------------------------------------------------------------

#define  A1  (-3.969683028665376e+01)
#define  A2  (+2.209460984245205e+02)
#define  A3  (-2.759285104469687e+02)
#define  A4  (+1.383577518672690e+02)
#define  A5  (-3.066479806614716e+01)
#define  A6  (+2.506628277459239e+00)

#define  B1  (-5.447609879822406e+01)
#define  B2  (+1.615858368580409e+02)
#define  B3  (-1.556989798598866e+02)
#define  B4  (+6.680131188771972e+01)
#define  B5  (-1.328068155288572e+01)

#define  C1  (-7.784894002430293e-03)
#define  C2  (-3.223964580411365e-01)
#define  C3  (-2.400758277161838e+00)
#define  C4  (-2.549732539343734e+00)
#define  C5  (+4.374664141464968e+00)
#define  C6  (+2.938163982698783e+00)

#define  D1  (+7.784695709041462e-03)
#define  D2  (+3.224671290700398e-01)
#define  D3  (+2.445134137142996e+00)
#define  D4  (+3.754408661907416e+00)

#define P_LOW   0.02425
#define P_HIGH  0.97575

//===========================================================================
// probability density function Normal(mu, sigma)
//===========================================================================
double ccruncher::Normal::pdf(double x, double mu, double sigma)
{
  return pdf((x-mu)/sigma);
}

//===========================================================================
// cumulative distribution function Normal(mu, sigma)
//===========================================================================
double ccruncher::Normal::cdf(double x, double mu, double sigma)
{
  return cdf((x-mu)/sigma);
}

//===========================================================================
// cdfinv.
// precision digits = 9
// extracted from http://home.online.no/~pjacklam/notes/invnorm/
//===========================================================================
double ccruncher::Normal::cdfinv(double p)
{
  double x=0.0;
  double q, r;

  if ((0 < p )  && (p < P_LOW))
  {
    q = sqrt(-2.0*log(p));
    x = (((((C1*q+C2)*q+C3)*q+C4)*q+C5)*q+C6) / ((((D1*q+D2)*q+D3)*q+D4)*q+1.0);
  }
  else
  {
    if ((P_LOW <= p) && (p <= P_HIGH))
    {
      q = p - 0.5;
      r = q*q;
      x = (((((A1*r+A2)*r+A3)*r+A4)*r+A5)*r+A6)*q /(((((B1*r+B2)*r+B3)*r+B4)*r+B5)*r+1.0);
    }
    else
    {
      if ((P_HIGH < p)&&(p < 1.0))
      {
        q = sqrt(-2.0*log(1.0-p));
        x = -(((((C1*q+C2)*q+C3)*q+C4)*q+C5)*q+C6) / ((((D1*q+D2)*q+D3)*q+D4)*q+1.0);
      }
      else
      {
        x = NAN;
      }
    }
  }

  return x;
}

/*===========================================================================
 *  R : A Computer Langage for Statistical Data Analysis
 *  Copyright (C) 1995, 1996  Robert Gentleman and Ross Ihaka
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Reference:
 * Cody, W.D. (1993). ALGORITHM 715: SPECFUN - A Portable FORTRAN
 * Package of Special Function Routines and Test Drivers"
 * ACM Transactions on Mathematical Software. 19, 22-32.
 *
 * This function evaluates the normal distribution function:
 * The main computation evaluates near-minimax approximations
 * derived from those in "Rational Chebyshev approximations for
 * the error function" by W. J. Cody, Math. Comp., 1969, 631-637.
 * This transportable program uses rational functions that
 * theoretically approximate the normal distribution function to
 * at least 18 significant decimal digits.  The accuracy achieved
 * depends on the arithmetic system, the compiler, the intrinsic
 * functions, and proper selection of the machine-dependent
 * constants.
 *
 * Mathematical Constants:
 *
 * sqrpi = 1 / sqrt(2*pi),
 * root32 = sqrt(32),
 * thrsh = the argument for which pnorm(thrsh,0,1) = 0.75.
  //===========================================================================*/
/*
#include <cfloat>

static double c[9] = {
  0.39894151208813466764,
  8.8831497943883759412,
  93.506656132177855979,
  597.27027639480026226,
  2494.5375852903726711,
  6848.1904505362823326,
  11602.651437647350124,
  9842.7148383839780218,
  1.0765576773720192317e-8
};

static double d[8] = {
  22.266688044328115691,
  235.38790178262499861,
  1519.377599407554805,
  6485.558298266760755,
  18615.571640885098091,
  34900.952721145977266,
  38912.003286093271411,
  19685.429676859990727
};

static double p[6] = {
  0.21589853405795699,
  0.1274011611602473639,
  0.022235277870649807,
  0.001421619193227893466,
  2.9112874951168792e-5,
  0.02307344176494017303
};

static double q[5] = {
  1.28426009614491121,
  0.468238212480865118,
  0.0659881378689285515,
  0.00378239633202758244,
  7.29751555083966205e-5
};

static double a[5] = {
  2.2352520354606839287,
  161.02823106855587881,
  1067.6894854603709582,
  18154.981253343561249,
  0.065682337918207449113
};

static double b[4] = {
  47.20258190468824187,
  976.09855173777669322,
  10260.932208618978205,
  45507.789335026729956
};

inline double fint(double x)
{
  return (x >= 0.0) ? floor(x) : -floor(-x);
}

double ccruncher::Normal::cdf(double x)
{
  double xden, temp, xnum, result, ccum;
  double del, min, eps, xsq;
  double y;
  int i;

  //  if(sd <= 0.0)
  //  DOMAIN_ERROR;
  //x = (x - mean) / sd;
  eps = DBL_EPSILON * .5;
  min = DBL_MIN;
  y = fabs(x);
  if (y <= 0.66291) {
    // Evaluate pnorm for |z| <= 0.66291
    xsq = 0.0;
    if (y > eps) {
      xsq = x * x;
    }
    xnum = a[4] * xsq;
    xden = xsq;
    for (i = 1; i <= 3; ++i) {
      xnum = (xnum + a[i - 1]) * xsq;
      xden = (xden + b[i - 1]) * xsq;
    }
    result = x * (xnum + a[3]) / (xden + b[3]);
    temp = result;
    result = 0.5 + temp;
    ccum = 0.5 - temp;
  }
  else if (y <= 5.656854248) {
    // Evaluate pnorm for 0.66291 <= |z| <= sqrt(32)
    xnum = c[8] * y;
    xden = y;
    for (i = 1; i <= 7; ++i) {
      xnum = (xnum + c[i - 1]) * y;
      xden = (xden + d[i - 1]) * y;
    }
    result = (xnum + c[7]) / (xden + d[7]);
    xsq = fint(y * 1.6) / 1.6;
    del = (y - xsq) * (y + xsq);
    result = exp(-xsq * xsq * 0.5) * exp(-del * 0.5) * result;
    ccum = 1.0 - result;
    if (x > 0.0) {
      temp = result;
      result = ccum;
      ccum = temp;
    }
  }
  else {
    // Evaluate pnorm for |z| > sqrt(32)
    result = 0.0;
    xsq = 1.0 / (x * x);
    xnum = p[5] * xsq;
    xden = xsq;
    for (i = 1; i <= 4; ++i) {
      xnum = (xnum + p[i - 1]) * xsq;
      xden = (xden + q[i - 1]) * xsq;
    }
    result = xsq * (xnum + p[4]) / (xden + q[4]);
    result = (0.39894228040143267794 - result) / y;
    xsq = fint(x * 1.6) / 1.6;
    del = (x - xsq) * (x + xsq);
    result = exp(-xsq * xsq * 0.5) * exp(-del * 0.5) * result;
    ccum = 1.0 - result;
    if (x > 0.0) {
      temp = result;
      result = ccum;
      ccum = temp;
    }
  }
  if (result < min) {
    result = 0.0;
  }
  if (ccum < min) {
    ccum = 0.0;
  }
  return result;
}
*/
