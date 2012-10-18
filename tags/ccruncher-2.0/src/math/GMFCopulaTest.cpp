
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
#include "math/GMFCopulaTest.hpp"

//---------------------------------------------------------------------------

#define NITERS 100000
#define EPSILON  0.01
#define NBINS 100

//===========================================================================
// test1. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::GMFCopulaTest::test1()
{
  // valid factor correlation matrix
  double sigmas[] = {
    +0.2, +0.4, +0.3,
    +0.4, +0.3, -0.2,
    +0.3, -0.2, +0.4
  };
  int n[3] = {1, 1, 1};

  // copula construction
  vector<vector<double> > correls(3, vector<double>(3));
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) correls[i][j] = sigmas[3*i+j];
  GMFCopula copula = GMFCopula(correls, vector<unsigned int>(n, n+3));

  // testing copula
  testCopula(copula, correls, n);

  // checks copula clone method (I)
  GMFCopula *clonedCopula1 = (GMFCopula*) copula.clone(false);
  testCopula(*clonedCopula1, correls, n);
  delete clonedCopula1;

  // checks copula clone method (II)
  GMFCopula *clonedCopula2 = (GMFCopula*) copula.clone(true);
  testCopula(*clonedCopula2, correls, n);
  delete clonedCopula2;
}

//===========================================================================
// test2. try to create a copula with non valid correlation matrix
//===========================================================================
void ccruncher_test::GMFCopulaTest::test2()
{
  int n[3] = {1, 1, 1};
  vector<vector<double> > correls(3, vector<double>(3));

  // factor loading out of range
  double sigmas1[] = {
     +2.0, +1.0, +4.0,
     +2.0, +2.0, +5.0,
     +2.0, +1.0, +6.0
  };
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) correls[i][j] = sigmas1[3*i+j];
  ASSERT_THROW(GMFCopula(correls, vector<unsigned int>(n, n+3)));

  // correlations out of range
  double sigmas2[] = {
     +0.2, +1.0, +4.0,
     +2.0, +0.3, +5.0,
     +2.0, +1.0, +0.4
  };
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) correls[i][j] = sigmas2[3*i+j];
  ASSERT_THROW(GMFCopula(correls, vector<unsigned int>(n, n+3)));

  // non symmetric matrix
  double sigmas3[] = {
     +0.2, +0.1, +0.1,
     +0.2, +0.3, +0.1,
     +0.1, +0.1, +0.4
  };
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) correls[i][j] = sigmas3[3*i+j];
  ASSERT_THROW(GMFCopula(correls, vector<unsigned int>(n, n+3)));

  // factor non definite-positive matrix
  double sigmas4[] = {
     +0.2, +0.0, +0.8,
     +0.0, +0.3, +0.8,
     +0.8, +0.8, +0.4
  };
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) correls[i][j] = sigmas4[3*i+j];
  ASSERT_THROW(GMFCopula(correls, vector<unsigned int>(n, n+3)));
}

//===========================================================================
// test3. try to acces components
//===========================================================================
void ccruncher_test::GMFCopulaTest::test3()
{
  // valid correlation matrix
  double sigmas[] = {
    +0.2, +0.4, +0.3,
    +0.4, +0.3, -0.2,
    +0.3, -0.2, +0.4
  };
  int n[3] = {1, 1, 1};

  vector<vector<double> > correls(3, vector<double>(3));
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) correls[i][j] = sigmas[3*i+j];
  GMFCopula copula = GMFCopula(correls, vector<unsigned int>(n, n+3));

  ASSERT(isnan(copula.get(0)));
  copula.next();
  ASSERT(isnan(copula.get(-1)));
  ASSERT(!isnan(copula.get(0)));
  ASSERT(!isnan(copula.get(1)));
  ASSERT(!isnan(copula.get(2)));
  ASSERT(isnan(copula.get(3)));
}

//===========================================================================
// test4. testing constructor based on other copula
//===========================================================================
void ccruncher_test::GMFCopulaTest::test4()
{
  // valid correlation matrix
  double sigmas[] = {
    +0.2, +0.4, +0.3,
    +0.4, +0.3, -0.2,
    +0.3, -0.2, +0.4
  };
  int n[3] = {1, 1, 1};

  // copula construction
  vector<vector<double> > correls(3, vector<double>(3));
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) correls[i][j] = sigmas[3*i+j];
  GMFCopula orig = GMFCopula(correls, vector<unsigned int>(n, n+3));
  GMFCopula copula = GMFCopula(orig);

  // testing copula
  testCopula(copula, correls, n);
}

//===========================================================================
// test5. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::GMFCopulaTest::test5()
{
  // valid correlation matrix
  double sigmas[] = {
    +0.4 , +0.6,
    +0.6 , +0.5
  };
  int n[2] = {3, 2};

  // copula construction
  vector<vector<double> > correls(2, vector<double>(2));
  for(int i=0; i<2; i++) for(int j=0; j<2; j++) correls[i][j] = sigmas[2*i+j];
  GMFCopula copula = GMFCopula(correls, vector<unsigned int>(n, n+2));

  // testing copula
  testCopula(copula, correls, n);

  // making 2-D density plot
  //computeDensity(copula);
}

//===========================================================================
// test6. generates NITERS realization of a copula and test that expected
// correlations are true
//===========================================================================
void ccruncher_test::GMFCopulaTest::test6()
{
  // valid correlation matrix
  double sigmas[] = {
    +0.4
  };
  int n[] = {2};

  // copula construction
  vector<vector<double> > correls(1, vector<double>(1));
  correls[0][0] = sigmas[0];
  GMFCopula copula = GMFCopula(correls, vector<unsigned int>(n, n+1));

  // testing copula
  testCopula(copula, correls, n);
}

//===========================================================================
// generate a 2-D histogram
// can be viewed with gnuplot using:
//  > set pm3d map
//  > set dgrid3d 100,100
//  > set hidden3d
//  > splot "file.dat" using 1:2:3
//===========================================================================
void ccruncher_test::GMFCopulaTest::computeDensity(Copula &copula)
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
double ccruncher_test::GMFCopulaTest::pearsn(const vector<double> &x, const vector<double> &y)
{
  double ax=0.0, ay=0.0, sxx=0.0, syy=0.0, sxy=0.0;
  int n = x.size();

  // computing mean
  for(int i=0; i<n; i++)
  {
    ax += x[i];
    ay += y[i];
  }

  ax /= n;
  ay /= n;

  // computing correlation
  for(int i=0; i<n; i++)
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
int ccruncher_test::GMFCopulaTest::getSector(int x, int *n, int m)
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
void ccruncher_test::GMFCopulaTest::testCopula(Copula &copula, const vector<vector<double> > &correls, int *n)
{
  // number of sectors
  int m = correls.size();

  // testing size method
  int nn = 0;
  for(int i=0; i<m; i++) nn += n[i];
  ASSERT(copula.size() == nn);

  // allocating space
  vector<vector<double> > values(nn, vector<double>(NITERS,NAN));

  // generating copulas
  for(int i=0; i<NITERS; i++)
  {
    copula.next();
    for (int j=0; j<nn; j++)
    {
      values[j][i] = copula.get(j);
    }
  }

  // doing assertions
  vector<vector<double> > sm = spearman(correls);
  for (int i=0; i<nn; i++)
  {
    int s1 = getSector(i, n, m);
    for (int j=0; j<nn; j++)
    {
      if (i==j) continue;
      int s2 = getSector(j, n, m);
      double sigma = pearsn(values[i], values[j]);
      ASSERT_EQUALS_EPSILON(sm[s1][s2], sigma, EPSILON);
    }
  }
}

//===========================================================================
// given a correlation  matrix between factors (diag = factor loadings)
// returns the correlation matrix between copula components (pearson correl)
//===========================================================================
vector<vector<double> > ccruncher_test::GMFCopulaTest::spearman(const vector<vector<double> > &M)
{
  vector<vector<double> > ret = M;

  // factor correlation -> multivariate gaussian correlation
  for(unsigned int i=0; i<M.size(); i++) {
    ret[i][i] = M[i][i] * M[i][i];
    for(unsigned int j=i+1; j<M.size(); j++) {
      ret[i][j] = M[i][i] * M[j][j] * M[i][j];
      ret[j][i] = ret[i][j];
    }
  }

  // multivariate gaussian correlation -> copula correlation
  for(unsigned int i=0; i<M.size(); i++) {
    for(unsigned int j=i; j<M.size(); j++) {
      ret[i][j] = 6.0/M_PI * asin(ret[i][j]/2.0);
      ret[j][i] = ret[i][j];
    }
  }

  return ret;
}
