
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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
#include <limits>
#include <gsl/gsl_cdf.h>
#include "params/CDF.hpp"
#include "params/CDFTest.hpp"

#define EPSILON 1e-5

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::CDFTest::test1()
{
  CDF cdf;

  cdf.setRange(0.0, numeric_limits<double>::infinity());

  ASSERT_THROW(cdf.add(100.0, +1.5));
  ASSERT_THROW(cdf.add(100.0, -0.5));
  ASSERT_THROW(cdf.add(10.0, NAN));
  ASSERT_THROW(cdf.add(NAN, 0.5));

  cdf.add(0.0, 0.0);
  cdf.add(0.1, 0.1);
  cdf.add(0.2, 0.2);
  cdf.add(0.3, 0.2);
  ASSERT_THROW(cdf.add(0.4, 0.1));
  cdf.add(0.15, 0.15);
  ASSERT_EQUALS(5ul, cdf.getPoints().size());
  ASSERT_EQUALS(0.15, cdf.getPoints()[2].first);
  cdf.add(0.4, 0.4);
  ASSERT_EQUALS(0.4, cdf.getPoints().back().first);
  ASSERT_THROW(cdf.setRange(0.1, 0.5));
  ASSERT_THROW(cdf.setRange(10.0, -10.0));
  cdf.setRange(0.0, 1.0);
  ASSERT_THROW(cdf.add(1.5, 0.7));
  cdf.add(0.05, 0.0);
  ASSERT_EQUALS(0.05, cdf.getPoints()[1].first);
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::CDFTest::test2()
{
  CDF cdf;

  for(int i=-50; i<=50; i++) {
    double x = i/10.0;
    cdf.add(x, gsl_cdf_tdist_P(x, 12.0));
  }
  ASSERT(cdf.getInterpolationType() == "cspline");

  for(int i=-50; i<50; i++) {

    double x = i/10.0;
    double y = gsl_cdf_tdist_P(+x, 12.0);
    double y1 = cdf.evalue(x);
    ASSERT_EQUALS_EPSILON(y, y1, EPSILON);

    x += 0.05;
    y = gsl_cdf_tdist_P(+x, 12.0);
    y1 = cdf.evalue(x);
    ASSERT_EQUALS_EPSILON(y, y1, EPSILON);
  }
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::CDFTest::test3()
{
  CDF cdf(0.0, +INFINITY);
  Date d[4] = { Date("1/1/2012"), Date("1/1/2014"), Date("1/1/2015"), Date("1/1/2017") };
  double p[4] = { 0.0, 0.5, 0.75, 0.9 };

  for(size_t i=0; i<4; i++) {
    double t = d[i]-d[0];
    cdf.add(t, p[i]);
  }

  // checking methods
  ASSERT_EQUALS(cdf.getInterpolationType(), "cspline");

  // checking evalue method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(cdf.evalue(d[i]-d[0]), p[i], EPSILON);

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double t = (d[i-1]-d[0]) + j*(d[i]-d[i-1])/100.0;
        // monotone
        ASSERT(cdf.evalue(d[i-1]-d[0]) < cdf.evalue(t));
        ASSERT(cdf.evalue(t) < cdf.evalue(d[i]-d[0]));
      }
    }
  }
}

//===========================================================================
// test4 (default rating)
//===========================================================================
void ccruncher_test::CDFTest::test4()
{
  CDF cdf(0.0, +INFINITY);
  cdf.add(0.0, 1.0);
  ASSERT_EQUALS_EPSILON(cdf.evalue(-1.0), 0.0, EPSILON);
  ASSERT_EQUALS_EPSILON(cdf.evalue(-0.1), 0.0, EPSILON);
  ASSERT_EQUALS_EPSILON(cdf.evalue(0.0), 1.0, EPSILON);
  ASSERT_EQUALS_EPSILON(cdf.evalue(0.1), 1.0, EPSILON);
  ASSERT_EQUALS_EPSILON(cdf.evalue(10.0), 1.0, EPSILON);
  ASSERT_EQUALS_EPSILON(cdf.evalue(100.0), 1.0, EPSILON);
}

//===========================================================================
// test5
// check precision when pd are near 0%
//===========================================================================
void ccruncher_test::CDFTest::test5()
{
  CDF cdf(0.0, +INFINITY);
  cdf.add(1*365.0+1, 0.001);
  cdf.add(2*365.0+1, 0.005);
  cdf.add(4*365.0+2, 0.03);

  double tvalues[] = { 60.0, 120.0, 360.0, 366.0, 500.0, 600.0, 700.0, 731.0,
                       800.0, 900.0, 1000.0, 1100.0, 1200.0, 1300.0, 1400.0,
                       1462.0, 1500.0 };

  double pvalues[] = { 9.74906e-05, 0.000205991, 0.000970285, 0.001,
                       0.00188457, 0.00292754, 0.00442538, 0.005,
                       0.00648195, 0.00908552, 0.0121543, 0.0156055,
                       0.0193563, 0.023324, 0.0274258, 0.03, 0.03 };

  for(int i=0; i<14; i++)
  {
    ASSERT_EQUALS_EPSILON(cdf.evalue(tvalues[i]), pvalues[i], 1e-5);
  }
}

