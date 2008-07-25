
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
// Normal.hpp - Normal header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added private constructor (non-instantiable class)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2007/08/06 - Gerard Torrent [gerard@fobos.generacio.com]
//   . cdf(x) and pdf(x) converted to inline
//
//===========================================================================

#ifndef _Normal_
#define _Normal_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cmath>

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class Normal
{

  private:

    // non-instantiable class
    Normal() {};


  public:

    inline static double pdf(double x);
    static double pdf(double x, double mu, double sigma);
    inline static double cdf(double x);
    static double cdf(double x, double mu, double sigma);
    static double cdfinv(double p);

};

//---------------------------------------------------------------------------

}

#define POSITIVE +1
#define NEGATIVE -1
#define NCOEFS   6

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
// probability density function Normal(0, 1)
//===========================================================================
double ccruncher::Normal::pdf(double x)
{
  //return exp(-x*x/2.0)/sqrt(2.0*M_PI);
  return exp(-x*x/2.0)/2.50662827463100050241576528481104525;
}

//===========================================================================
// cumulative distribution function Normal(0, 1)
// use algorithm explained in Hull book
// precision achieved = 6 digits
// x: point where we evaluate function
//===========================================================================
double ccruncher::Normal::cdf(double x)
{
  double ret, k;
  int i, sign;

  if(x < 0.0)
  {
    sign = NEGATIVE;
    x = -x;
  }
  else
  {
    sign = POSITIVE;
  }

  k = 1.0/(1.0 + x*0.2316419);

  ret = 0.0;

  for(i=NCOEFS-1;i>0;i--)
  {
    ret = (ret + coef[i])*k;
  }

  ret = 1.0 - pdf(x)*ret;

  if(sign == NEGATIVE)
  {
    ret = 1.0 - ret;
  }

  return ret;
}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
