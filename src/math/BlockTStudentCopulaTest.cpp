
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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
#include "math/BlockTStudentCopula.hpp"
#include "math/BlockTStudentCopulaTest.hpp"
#include "utils/Arrays.hpp"

//---------------------------------------------------------------------------

#define NITERS 100000
#define EPSILON  0.01
#define NBINS 100
#define ndf 3.0

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::test1()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Arrays<double>::allocMatrix(3,3,sigmas);
  int n[3] = {1, 1, 1};

  // copula construction
  BlockTStudentCopula copula = BlockTStudentCopula(correls, n, 3, ndf);

  // testing copula
  testCopula(copula, sigmas, n, 3);

  // checking invalid degree of freedom
  ASSERT_THROW(BlockTStudentCopula(correls, n, 3, -1));

  // checks copula clone method (I)
  BlockTStudentCopula *clonedCopula1 = (BlockTStudentCopula*) copula.clone(false);
  testCopula(*clonedCopula1, sigmas, n, 3);
  delete clonedCopula1;

  // checks copula clone method (II)
  BlockTStudentCopula *clonedCopula2 = (BlockTStudentCopula*) copula.clone();
  testCopula(*clonedCopula2, sigmas, n, 3);
  delete clonedCopula2;

  // exit function
  Arrays<double>::deallocMatrix(correls, 3);
}

//===========================================================================
// test2. try to create a copula with non valid correlation matrix
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::test2()
{
  // non valid correlation matrix (non definite positive)
  double sigmas[] = {
     +2.0, +1.0, +4.0,
     +2.0, +2.0, +5.0,
     +2.0, +1.0, +6.0
  };
  double **correls = Arrays<double>::allocMatrix(3,3,sigmas);
  int n[3] = {1, 1, 1};

  ASSERT_THROW(BlockTStudentCopula(correls, n, 3, ndf));

  // exit function
  Arrays<double>::deallocMatrix(correls, 3);
}

//===========================================================================
// test3. try to acces components
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::test3()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Arrays<double>::allocMatrix(3,3,sigmas);
  int n[3] = {1, 1, 1};

  BlockTStudentCopula copula = BlockTStudentCopula(correls, n, 3, ndf);

  ASSERT(isnan(copula.get(-1)));
  ASSERT(!isnan(copula.get(0)));
  ASSERT(!isnan(copula.get(1)));
  ASSERT(!isnan(copula.get(2)));
  ASSERT(isnan(copula.get(3)));

  // exit function
  Arrays<double>::deallocMatrix(correls, 3);
}

//===========================================================================
// test4. testing constructor based on other copula
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::test4()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Arrays<double>::allocMatrix(3,3,sigmas);
  int n[3] = {1, 1, 1};

  // copula construction
  BlockTStudentCopula orig = BlockTStudentCopula(correls, n, 3, ndf);
  BlockTStudentCopula copula = BlockTStudentCopula(orig);

  // testing copula
  testCopula(copula, sigmas, n, 3);

  // exit function
  Arrays<double>::deallocMatrix(correls, 3);
}

//===========================================================================
// test5. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::test5()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.2,
    +0.2 , +1.0
  };
  double **correls = Arrays<double>::allocMatrix(2,2,sigmas);
  int n[2] = {1, 1};

  // copula construction
  BlockTStudentCopula copula = BlockTStudentCopula(correls, n, 2, ndf);

  // testing copula
  testCopula(copula, sigmas, n, 2);

  // making 2-D density plot
  //computeDensity(copula);

  // exit function
  Arrays<double>::deallocMatrix(correls, 2);
}

//===========================================================================
// test6. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::test6()
{
  // valid correlation matrix
  double sigmas[] = {
    +0.4
  };
  double **correls = Arrays<double>::allocMatrix(1,1,sigmas);
  int n[] = {2};

  // copula construction
  BlockTStudentCopula copula = BlockTStudentCopula(correls, n, 1, ndf);

  // testing copula
  testCopula(copula, sigmas, n, 1);

  // exit function
  Arrays<double>::deallocMatrix(correls, 1);
}

//===========================================================================
// generate a 2-D histogram
// can be viewed with gnuplot using:
//  > set pm3d map
//  > set dgrid3d 100,100
//  > set hidden3d
//  > splot "file.dat" using 1:2:3
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::computeDensity(BlockTStudentCopula &copula)
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
// computes the correlation factor of 2 series
// extracted from 'Numerical Recipes in C'
//===========================================================================
double ccruncher_test::BlockTStudentCopulaTest::pearsn(double *x, double *y, int n)
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
// given an element index returns the sector
// x: element index value (0-based)
// n: number of elements at each sector
// m: number of sectors
//===========================================================================
int ccruncher_test::BlockTStudentCopulaTest::getSector(int x, int *n, int m)
{
  int acum = 0;
  for(int i=0; i<m; i++) {
    acum += n[i];
    if (x+1 <= acum) return i;
  }
  ASSERT(false);
  return -1;
}

//===========================================================================
// testCopula. generates NITERS realization of a copula and test that expected
// correlations are true
// copula: copula to be tested
// correls: vector of size mxm containing sectors correlations 
// n: number of elements at each sector
// m: number of sectors
//===========================================================================
void ccruncher_test::BlockTStudentCopulaTest::testCopula(BlockTStudentCopula &copula, double *correls, int *n, int m)
{
  double **values;

  // testing size method
  int nn = 0;
  for(int i=0; i<m; i++) nn += n[i];
  ASSERT(copula.size() == nn);

  // allocating space
  values = Arrays<double>::allocMatrix(nn, NITERS);

  // generating copulas
  for(int i=0;i<NITERS;i++)
  {
    copula.next();
    for (int j=0;j<nn;j++)
    {
      values[j][i] = copula.get(j);
    }
  }
  for (int i=0;i<nn;i++)
  {
    int s1 = getSector(i, n, m);
    for (int j=0;j<nn;j++)
    {
      if (i==j) continue;
      int s2 = getSector(j, n, m);
      double sigma = pearsn(values[i], values[j], NITERS);
      ASSERT_EQUALS_EPSILON(correls[s2+m*s1], sigma, EPSILON);
    }
  }

  // exit
  Arrays<double>::deallocMatrix(values, nn);
}

