
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

#include <iostream>
#include <cmath>
#include <gsl/gsl_cdf.h>
#include "math/FastTStudentCdf.hpp"
#include "math/FastTStudentCdfTest.hpp"

//---------------------------------------------------------------------------

//#define EPSILON  1.0002e-6
#define EPSILON  1.02e-7

//===========================================================================
// test1. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::FastTStudentCdfTest::test1()
{
  for (int i=2; i<=50; i++)
  {
    double ndf = (double)(i);
    double x0 = gsl_cdf_tdist_Pinv(0.0001, ndf);
    double max_err = -1e10;
    FastTStudentCdf tcdf(ndf);
    double x = x0;

    do
    {
      double approx = tcdf.eval(x);
      ASSERT(0.0 <= approx && approx <= 1.0);
      double exact = gsl_cdf_tdist_P(x, ndf);
      double err = fabs(approx-exact);
      if (max_err < err) 
      {
        max_err = err;
      }
      
      x += 0.0001;
    }
    while(x < -x0);

    ASSERT(max_err < EPSILON);
  }
}

