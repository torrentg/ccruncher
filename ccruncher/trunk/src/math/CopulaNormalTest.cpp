
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
// CopulaNormalTest.cpp - CopulaNormalTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//===========================================================================

#include <iostream>
#include <cmath>
#include "CopulaNormal.hpp"
#include "CopulaNormalTest.hpp"
#include "Normal.hpp"
#include "utils/Utils.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

#define NITERS 100000
#define EPSILON  0.1
#define NTALLS   100

//===========================================================================
// setUp
//===========================================================================
void CopulaNormalTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void CopulaNormalTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void CopulaNormalTest::test1()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Utils::allocMatrix(3,3,sigmas);

  // copula construction
  CopulaNormal copula = CopulaNormal(3, correls);

  // testing copula
  ASSERT_NO_THROW(testCopula(copula, sigmas, 3));

  // correls deallocated by copula destructor
}

//===========================================================================
// test2. try to create a copula with non valid correlation matrix
//===========================================================================
void CopulaNormalTest::test2()
{
  // non valid correlation matrix (non definite positive)
  double sigmas[] = {
     +2.0, +1.0, +4.0,
     +2.0, +2.0, +5.0,
     +2.0, +1.0, +6.0
  };
  double **correls = Utils::allocMatrix(3,3,sigmas);

  ASSERT_THROW(CopulaNormal(3, correls));

  // correls deallocated by copula destructor
}

//===========================================================================
// test3. try to acces components
//===========================================================================
void CopulaNormalTest::test3()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Utils::allocMatrix(3,3,sigmas);

  CopulaNormal copula = CopulaNormal(3, correls);

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
void CopulaNormalTest::test4()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4 , +0.3 ,
    +0.4 , +1.0 , -0.25,
    +0.3 , -0.25, +1.0
  };
  double **correls = Utils::allocMatrix(3,3,sigmas);

  // copula construction
  CopulaNormal orig = CopulaNormal(3, correls);
  CopulaNormal copula = CopulaNormal(orig);

  // testing copula
  ASSERT_NO_THROW(testCopula(copula, sigmas, 3));

  // correls deallocated by copula destructor
}


//===========================================================================
// generate a 2-D histogram
//===========================================================================
void computeDensity(CopulaNormal &copula)
{
  double x, y;
  int a, b;
  int hits[NTALLS+1][NTALLS+1];

  // initializing counters
  for(int i=0;i<(NTALLS+1);i++)
  {
    for(int j=0;j<(NTALLS+1);j++)
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
    
    a = (int)(x*NTALLS);
    b = (int)(y*NTALLS);
    
    hits[a][b]++;
  }

  // plotting histogram
  for(int i=0;i<NTALLS;i++)
  {
    x = (double)(i)/(double)(NTALLS);
    
    for(int j=0;j<NTALLS;j++)
    {
      y = (double)(j)/(double)(NTALLS);

      cout << x << "\t" << y << "\t" << hits[i][j] << endl;
    }
  }
}

//===========================================================================
// test5. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void CopulaNormalTest::test5()
{
  // valid correlation matrix
  double sigmas[] = {
    +1.0 , +0.4,
    +0.4 , +1.0
  };
  double **correls = Utils::allocMatrix(2,2,sigmas);

  // copula construction
  CopulaNormal copula = CopulaNormal(2, correls);

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
double CopulaNormalTest::pearsn(double *x, double *y, int n)
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
void CopulaNormalTest::testCopula(CopulaNormal &copula, double *correls, int n)
{
  double **values;

  // testing size method
  ASSERT(copula.size() == n);

  // allocating space
  values = Utils::allocMatrix(n, NITERS);

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
      ASSERT_DOUBLES_EQUAL(correls[j+n*i], sigma, EPSILON);
    }
  }

  // exit
  Utils::deallocMatrix(values, n);
}
