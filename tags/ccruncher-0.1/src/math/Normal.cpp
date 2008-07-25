
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
// Normal.cpp - Normal code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <ctime>
#include <cstdlib>
#include "Normal.hpp"

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

#define POSITIU +1
#define NEGATIU -1
#define NCOEFS   6

#ifndef M_PI
#define M_PI    3.141592653589793238462643383279502884197196
#endif

//---------------------------------------------------------------------------

static double coef[] =
  {
    +0.0,
    +0.319381530,
    -0.356563782,
    +1.781477937,
    -1.821255978,
    +1.330274429
  };

//===========================================================================
// Funcio de densitat de la Normal(0, 1)
//===========================================================================
double ccruncher::Normal::pdf(double x)
{
  //return exp(-x*x/2.0)/sqrt(2.0*M_PI);
  return exp(-x*x/2.0)/2.50662827463100050241576528481104525;
}

//===========================================================================
// Funcio de densitat de la Normal(mu, sigma)
//===========================================================================
double ccruncher::Normal::pdf(double x, double mu, double sigma)
{
  return pdf((x-mu)/sigma);
}

//===========================================================================
// Funcio de distribucio de la Normal(0, 1)
// Usa la formula del llibre de HULL.
// La precisio asolida es de 6 decimals.
// x: punt on volem evaluar la densitat.
//===========================================================================
double ccruncher::Normal::cdf(double x)
{
  double ret, k;
  int i, sign;

  if(x < 0.0)
  {
    sign = NEGATIU;
    x = -x;
  }
  else
  {
    sign = POSITIU;
  }

  k = 1.0/(1.0 + x*0.2316419);

  ret = 0.0;

  for(i=NCOEFS-1;i>0;i--)
  {
    ret = (ret + coef[i])*k;
  }

  ret = 1.0 - pdf(x)*ret;

  if(sign == NEGATIU)
  {
    ret = 1.0 - ret;
  }

  return ret;
}

//===========================================================================
// Funcio de distribucio de la Normal(mu, sigma)
//===========================================================================
double ccruncher::Normal::cdf(double x, double mu, double sigma)
{
  return cdf((x-mu)/sigma);
}

//===========================================================================
// cdfinv. precisio del resultat = 9 decimals correctes
// extract from http://home.online.no/~pjacklam/notes/invnorm/
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
