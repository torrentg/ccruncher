
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
// GaussianCopulaTest.cpp - GaussianCopulaTest code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/05/20 - Gerard Torrent [gerard@mail.generacio.com]
//   . implemented Arrays class
//
// 2005/07/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/07/24 - Gerard Torrent [gerard@mail.generacio.com]
//   . class CopulaNormal renamed to GaussianCopula
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <iostream>
#include <cmath>
#include "math/GaussianCopula.hpp"
#include "math/GaussianCopulaTest.hpp"
#include "utils/Arrays.hpp"

//---------------------------------------------------------------------------

#define NITERS 100000
#define EPSILON  0.01
#define NBINS 100

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::GaussianCopulaTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::GaussianCopulaTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::GaussianCopulaTest::test1()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Arrays<double>::allocMatrix(3,3,sigmas);

  // copula construction
  GaussianCopula copula = GaussianCopula(3, correls);

  // testing copula
  ASSERT_NO_THROW(testCopula(copula, sigmas, 3));

  // correls deallocated by copula destructor
}

//===========================================================================
// test2. try to create a copula with non valid correlation matrix
//===========================================================================
void ccruncher_test::GaussianCopulaTest::test2()
{
  // non valid correlation matrix (non definite positive)
  double sigmas[] = {
     +2.0, +1.0, +4.0,
     +2.0, +2.0, +5.0,
     +2.0, +1.0, +6.0
  };
  double **correls = Arrays<double>::allocMatrix(3,3,sigmas);

  ASSERT_THROW(GaussianCopula(3, correls));

  // correls deallocated by copula destructor
}

//===========================================================================
// test3. try to acces components
//===========================================================================
void ccruncher_test::GaussianCopulaTest::test3()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Arrays<double>::allocMatrix(3,3,sigmas);

  GaussianCopula copula = GaussianCopula(3, correls);

  ASSERT(isnan(copula.get(-1)));
  ASSERT(!isnan(copula.get(0)));
  ASSERT(!isnan(copula.get(1)));
  ASSERT(!isnan(copula.get(2)));
  ASSERT(isnan(copula.get(3)));

  // correls deallocated by copula destructor
}

//===========================================================================
// test4. testing constructor based on other copula
//===========================================================================
void ccruncher_test::GaussianCopulaTest::test4()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Arrays<double>::allocMatrix(3,3,sigmas);

  // copula construction
  GaussianCopula orig = GaussianCopula(3, correls);
  GaussianCopula copula = GaussianCopula(orig);

  // testing copula
  ASSERT_NO_THROW(testCopula(copula, sigmas, 3));

  // correls deallocated by copula destructor
}

//===========================================================================
// generate a 2-D histogram
//===========================================================================
void ccruncher_test::GaussianCopulaTest::computeDensity(GaussianCopula &copula)
{
  double x, y;
  int a, b;
  int hits[NBINS+1][NBINS+1];

  // initializing counters
  for(int i=0;i<(NBINS+1);i++)
  {
    for(int j=0;j<(NBINS+1);j++)
    {
      hits[i][j] = 0;
    }
  }

  // generating copulas
  for(int k=0;k<NITERS;k++)
  {
    copula.next();

    x = copula.get(0);
    y = copula.get(1);

    a = (int)(x*NBINS);
    b = (int)(y*NBINS);

    hits[a][b]++;
  }

  // plotting histogram
  for(int i=0;i<NBINS;i++)
  {
    x = (double)(i)/(double)(NBINS);

    for(int j=0;j<NBINS;j++)
    {
      y = (double)(j)/(double)(NBINS);

      cout << x << "\t" << y << "\t" << hits[i][j] << endl;
    }
  }
}

//===========================================================================
// test5. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::GaussianCopulaTest::test5()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.0,
    +0.0 , +1.0
  };
  double **correls = Arrays<double>::allocMatrix(2,2,sigmas);

  // copula construction
  GaussianCopula copula = GaussianCopula(2, correls);

  // testing copula
  ASSERT_NO_THROW(testCopula(copula, sigmas, 2));

  // making 2-D density plot
  //computeDensity(copula);

  // correls deallocated by copula destructor
}

//===========================================================================
// computes the correlation factor of 2 series
// extracted from 'Numerical Recipes in C'
//===========================================================================
double ccruncher_test::GaussianCopulaTest::pearsn(double *x, double *y, int n)
{
  double ax=0.0, ay=0.0, sxx=0.0, syy=0.0, sxy=0.0;

  // trobem les mitjes
  for(int i=0;i<n;i++)
  {
    ax += x[i];
    ay += y[i];
  }

  ax /= n;
  ay /= n;

  // calculem el coeficient de correlacio
  for(int i=0;i<n;i++)
  {
    double xt = x[i] - ax;
    double yt = y[i] - ay;

    sxx += xt*xt;
    syy += yt*yt;
    sxy += xt*yt;
  }

  return sxy/(sqrt(sxx*syy)+1e-20);
}

//===========================================================================
// testCopula. generates NITERS realization of a copula and test that expected
// correlations are true
// correls is a vector of size nxn
//===========================================================================
void ccruncher_test::GaussianCopulaTest::testCopula(GaussianCopula &copula, double *correls, int n)
{
  double **values;

  // testing size method
  ASSERT(copula.size() == n);

  // allocating space
  values = Arrays<double>::allocMatrix(n, NITERS);

  // generating copulas
  for(int i=0;i<NITERS;i++)
  {
    copula.next();

    for (int j=0;j<n;j++)
    {

      values[j][i] = copula.get(j);
    }
  }

  for (int i=0;i<n;i++)
  {
    for (int j=0;j<n;j++)
    {
      double sigma = pearsn(values[i], values[j], NITERS);
      ASSERT_EQUALS_EPSILON(correls[j+n*i], sigma, EPSILON);
    }
  }

  // exit
  Arrays<double>::deallocMatrix(values, n);
}
