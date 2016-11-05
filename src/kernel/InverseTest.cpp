
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#include <limits>
#include <gsl/gsl_cdf.h>
#include "kernel/InverseTest.hpp"
#include "kernel/Inverse.hpp"

#define EPSILON 5e-2

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
// gaussian case
//===========================================================================
void ccruncher_test::InverseTest::test1()
{
  CDF cdf(0.0, +INFINITY);
  cdf.add(365.0, 0.1);

  vector<int> nodes(366);
  for(size_t i=0; i<nodes.size(); i++) nodes[i] = i;

  Inverse inverse(+INFINITY, 365.0, cdf, nodes);
  ASSERT(inverse.size() == 11);
  ASSERT(inverse.getInterpolationType() == "cspline");

  double minval = gsl_cdf_ugaussian_Pinv(cdf.evalue(1.0));
  double maxval = gsl_cdf_ugaussian_Pinv(cdf.evalue(365.0));

  for(int i=0; i<10000; i++)
  {
    double x = minval + i*(maxval-minval)/10000.0;
    double t = inverse.evalue(x);
    ASSERT(0.0 <= t && t <= 365.0);
    double y = gsl_cdf_ugaussian_Pinv(cdf.evalue(t));
    ASSERT_EQUALS_EPSILON(x, y, EPSILON);
  }
}

//===========================================================================
// test2
// t-Student case
//===========================================================================
void ccruncher_test::InverseTest::test2()
{
  double ndf = 3.0;
  CDF cdf(0.0, +INFINITY);
  cdf.add(365.0, 0.1);

  vector<int> nodes(366);
  for(size_t i=0; i<nodes.size(); i++) nodes[i] = i;

  Inverse inverse(ndf, 365.0, cdf, nodes);
  ASSERT(inverse.size() == 18);
  ASSERT(inverse.getInterpolationType() == "cspline");

  double minval = gsl_cdf_tdist_Pinv(cdf.evalue(1.0), ndf);
  double maxval = gsl_cdf_tdist_Pinv(cdf.evalue(365.0), ndf);

  for(int i=0; i<10000; i++)
  {
    double x = minval + i*(maxval-minval)/10000.0;
    double t = inverse.evalue(x);
    ASSERT(0.0 <= t && t <= 365.0);
    double y = gsl_cdf_tdist_Pinv(cdf.evalue(t), ndf);
    ASSERT_EQUALS_EPSILON(x, y, EPSILON);
  }
}

//===========================================================================
// test3
// negative time range
//===========================================================================
void ccruncher_test::InverseTest::test3()
{
  double ndf = 3.0;
  CDF cdf(0.0, +INFINITY);
  cdf.add(365.0, 0.1);

  vector<int> nodes(366);
  for(size_t i=0; i<nodes.size(); i++) nodes[i] = i;

  Inverse inverse;
  ASSERT_THROW(inverse.init(ndf, -365.0, cdf, nodes));
}

//===========================================================================
// test4
// invariant to catch changes
//===========================================================================
void ccruncher_test::InverseTest::test4()
{
  CDF cdf(0.0, +INFINITY);
  cdf.add(1*365.0, 0.01);
  cdf.add(2*365.0, 0.02);
  cdf.add(3*365.0, 0.05);
  cdf.add(4*365.0, 0.10);
  cdf.add(5*365.0, 0.17);
  ASSERT_EQUALS(cdf.getInterpolationType(), "cspline");

  vector<int> nodes(5*365+1);
  for(size_t i=0; i<nodes.size(); i++) nodes[i] = i;

  Inverse inverse(5.0, 5*365.0, cdf, nodes);
  ASSERT(inverse.size() == 36);
  ASSERT(inverse.getInterpolationType() == "cspline");

  double xvalues[] = { -10.0, -9.0, -8.0, -7.0, -6.0, -5.0, -4.0, -3.0,
                       -2.0, -1.0, 0.0 };

  double tvalues[] = { 2.79995, 4.61895, 8.0686, 14.9838, 30.2429, 67.4158,
                       172.994, 599.476, 1103.77, 1925.0, 1925.0 };

  for(int i=0; i<11; i++)
  {
    ASSERT_EQUALS_EPSILON(inverse.evalue(xvalues[i]), tvalues[i], EPSILON);
  }
}

//===========================================================================
// test5
// preserves linear interpolation
//===========================================================================
void ccruncher_test::InverseTest::test5()
{
  CDF cdf(0.0, +INFINITY);
  cdf.add(1*365.0, 0.001);
  cdf.add(2*365.0, 0.2);
  ASSERT_EQUALS(cdf.getInterpolationType(), "linear");

  vector<int> nodes(2*365+1);
  for(size_t i=0; i<nodes.size(); i++) nodes[i] = i;

  Inverse inverse(5.0, 2*365.0, cdf, nodes);
  ASSERT(inverse.size() == 199);
  ASSERT(inverse.getInterpolationType() == "linear");
}

//===========================================================================
// test6
// reduced number of nodes
//===========================================================================
void ccruncher_test::InverseTest::test6()
{
  CDF cdf(0.0, +INFINITY);
  cdf.add(365.0, 0.05);
  ASSERT_EQUALS(cdf.getInterpolationType(), "linear");

  vector<int> nodes = {365};

  Inverse inverse(5.0, 1*365.0, cdf, nodes);
  ASSERT(inverse.size() == 2);
  ASSERT(inverse.getInterpolationType() == "linear");
}

//===========================================================================
// test7
//===========================================================================
void ccruncher_test::InverseTest::test7()
{
  CDF cdf(0.0, +INFINITY);
  cdf.add(90.0, 0.01);
  cdf.add(180.0, 0.02);
  cdf.add(270.0, 0.03);
  cdf.add(365.0, 0.04);
  ASSERT_EQUALS(cdf.getInterpolationType(), "cspline");

  vector<int> nodes = {365};

  Inverse inverse(5.0, 365.0, cdf, nodes);
  ASSERT(inverse.size() == 2);
  ASSERT(inverse.getInterpolationType() == "linear");
}

