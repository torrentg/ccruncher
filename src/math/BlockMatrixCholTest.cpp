
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
// BlockMatrixCholTest.cpp - BlockMatrixCholTest code
// --------------------------------------------------------------------------
//
// 2005/07/23 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <iostream>
#include <cmath>
#include "math/BlockMatrixChol.hpp"
#include "math/BlockMatrixCholTest.hpp"
#include "utils/Arrays.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00001

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1. test cholesky decomposition (M=1, N=3)
//
// validated with octave using:
//    A = [1,0.5,0.5;0.5,1,0.5;0.5,0.5,1]
//      1.00000  0.50000  0.50000
//      0.50000  1.00000  0.50000
//      0.50000  0.50000  1.00000
//    B = chol(A)' (octave & mathematica considerer L'·L instead of L·L')
//      1.00000  0.00000  0.00000
//      0.50000  0.86603  0.00000
//      0.50000  0.28868  0.81650
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test1()
{
  double valA[] = {
     +0.5
  };
  int n[] = { 3 };
  double solA[] = {
      1.00000, 0.00000, 0.00000,
      0.50000, 0.86603, 0.00000,
      0.50000, 0.28868, 0.81650
  };
  double **A = Arrays<double>::allocMatrix(1,1,valA);

  BlockMatrixChol *chol=NULL;

  ASSERT_NO_THROW(chol = new BlockMatrixChol(A, n, 1));
  ASSERT_EQUALS(3, chol->getDim());

  // checking cholesky values
  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      ASSERT_DOUBLES_EQUAL(solA[3*i+j], chol->get(i,j), EPSILON);
    }
  }

  // checking mult method
  double x[3] = {1.0, 1.0, 1.0};
  double y[3] = {0.0, 0.0, 0.0};
  double z[3] = {1.0, 1.36603, 1.60518};
  chol->mult(x, y);
  for(int i=0;i<3;i++)
  {
    ASSERT_DOUBLES_EQUAL(y[i], z[i], EPSILON);
  }

  // exit function
  delete chol;
  Arrays<double>::deallocMatrix(A, 1);
}

//===========================================================================
// test2. test cholesky decomposition (M=3, N=3)
//
// validated with octave using:
//    A = [1,0.1,0.2;0.1,1,0.3;0.2,0.3,1]
//      1.00000  0.10000  0.20000
//      0.10000  1.00000  0.30000
//      0.20000  0.30000  1.00000
//    B = chol(A)' (octave & mathematica considerer L'·L instead of L·L')
//      1.00000  0.00000  0.00000
//      0.10000  0.99499  0.00000
//      0.20000  0.28141  0.93851
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test2()
{
  // only 1 element per sector (autocorrelation not used => we put 1000)
  double valA[] = {
      1000.00, 0.10000, 0.20000,
      0.10000, 1000.00, 0.30000,
      0.20000, 0.30000, 1000.00
  };
  int n[] = { 1, 1, 1 };
  double solA[] = {
     1.00000, 0.00000, 0.00000,
     0.10000, 0.99499, 0.00000,
     0.20000, 0.28141, 0.93851
  };
  double **A = Arrays<double>::allocMatrix(3,3,valA);

  BlockMatrixChol *chol=NULL;

  ASSERT_NO_THROW(chol = new BlockMatrixChol(A, n, 3));
  ASSERT_EQUALS(3, chol->getDim());
  ASSERT_NO_THROW(check(A, n, 3, chol));

  // checking cholesky values
  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      ASSERT_DOUBLES_EQUAL(solA[3*i+j], chol->get(i,j), EPSILON);
    }
  }

  // checking mult method
  double x[3] = {1.0, 1.0, 1.0};
  double y[3] = {0.0, 0.0, 0.0};
  double z[3] = {1.0, 1.09499, 1.41992};
  chol->mult(x, y);
  for(int i=0;i<3;i++)
  {
    ASSERT_DOUBLES_EQUAL(y[i], z[i], EPSILON);
  }

  // exit function
  delete chol;
  Arrays<double>::deallocMatrix(A, 3);
}

//===========================================================================
// test3. test cholesky decomposition (M=2, N=5)
//
// validated with octave using:
//    A = [1,0.5,0.5,0.1,0.1;0.5,1,0.5,0.1,0.1;0.5,0.5,1,0.1,0.1;0.1,0.1,0.1,1,0.35;0.1,0.1,0.1,0.35,1]
//     1.00000  0.50000  0.50000  0.10000  0.10000
//     0.50000  1.00000  0.50000  0.10000  0.10000
//     0.50000  0.50000  1.00000  0.10000  0.10000
//     0.10000  0.10000  0.10000  1.00000  0.35000
//     0.10000  0.10000  0.10000  0.35000  1.00000
//    B = chol(A)' (octave & mathematica considerer L'·L instead of L·L')
//     1.00000  0.00000  0.00000  0.00000  0.00000
//     0.50000  0.86603  0.00000  0.00000  0.00000
//     0.50000  0.28868  0.81650  0.00000  0.00000
//     0.10000  0.05774  0.04082  0.99247  0.00000
//     0.10000  0.05774  0.04082  0.33754  0.93331
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test3()
{
  double valA[] = {
      0.50000, 0.10000,
      0.10000, 0.35000
  };
  int n[] = { 3, 2 };
  double solA[] = {
     1.00000, 0.00000, 0.00000, 0.00000, 0.00000,
     0.50000, 0.86603, 0.00000, 0.00000, 0.00000,
     0.50000, 0.28868, 0.81650, 0.00000, 0.00000,
     0.10000, 0.05774, 0.04082, 0.99247, 0.00000,
     0.10000, 0.05774, 0.04082, 0.33754, 0.93331
  };
  double **A = Arrays<double>::allocMatrix(2,2,valA);

  BlockMatrixChol *chol=NULL;

  ASSERT_NO_THROW(chol = new BlockMatrixChol(A, n, 2));
  ASSERT_EQUALS(5, chol->getDim());
  ASSERT_NO_THROW(check(A, n, 2, chol));

  // checking cholesky values
  for (int i=0;i<5;i++)
  {
    for (int j=0;j<5;j++)
    {
      ASSERT_DOUBLES_EQUAL(solA[5*i+j], chol->get(i,j), EPSILON);
    }
  }

  // checking mult method
  double x[5] = {1.0, 1.0, 1.0, 1.0, 1.0};
  double y[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  double z[5] = {1.0, 1.36603, 1.60518, 1.19103, 1.46941};
  chol->mult(x, y);
  for(int i=0;i<5;i++)
  {
    ASSERT_DOUBLES_EQUAL(y[i], z[i], EPSILON);
  }

  // exit function
  delete chol;
  Arrays<double>::deallocMatrix(A, 2);
}

//===========================================================================
// test4. test cholesky decomposition (M=4, N=100)
//
// validated checking chol(A)·chol(A)' = A
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test4()
{
  double valA[] = {
      0.50000, 0.10000, 0.20000, 0.15000,
      0.10000, 0.75000, 0.12500, 0.05000,
      0.20000, 0.12500, 0.80000, 0.15000,
      0.15000, 0.05000, 0.15000, 0.65000
  };
  int n[] = { 25, 25, 25, 25 };
  double **A = Arrays<double>::allocMatrix(4,4,valA);

  BlockMatrixChol *chol=NULL;

  ASSERT_NO_THROW(chol = new BlockMatrixChol(A, n, 4));
  ASSERT_EQUALS(100, chol->getDim());
  ASSERT_NO_THROW(check(A, n, 4, chol));

  // exit function
  delete chol;
  Arrays<double>::deallocMatrix(A, 4);
}

//===========================================================================
// test5.
// trying to decompose a non-definite positive block matrix
// observe that autocorrelation factors are very lows (compared with
// the rest of correlation factors)
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test5()
{
  double valA[] = {
      0.00100, 0.10000, 0.20000, 0.15000,
      0.10000, 0.00100, 0.12500, 0.05000,
      0.20000, 0.12500, 0.00100, 0.15000,
      0.15000, 0.05000, 0.15000, 0.00100
  };
  int n[] = { 25, 25, 25, 25 };
  double **A = Arrays<double>::allocMatrix(4,4,valA);

  BlockMatrixChol *chol=NULL;
  ASSERT_THROW(chol = new BlockMatrixChol(A, n, 4));

  // exit function
  Arrays<double>::deallocMatrix(A, 4);
}

//===========================================================================
// test6.
// show that we can do a 50.000 x 50.000 matrix cholesky decomposition
// with a low memory footprint and reduced number of multiplications
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test6()
{
  // code comented because factorization + debug mode + valgrind
  // spent too much time
  // if you want test, uncoment below block and run in non debug
  // mode. factorization takes less than 1 minute and mult()
  // takes some fraction of second. Amazing !!!
  ASSERT(true);
/*
  double valA[] = {
      0.50000, 0.10000, 0.20000, 0.15000,
      0.10000, 0.75000, 0.12500, 0.05000,
      0.20000, 0.12500, 0.80000, 0.15000,
      0.15000, 0.05000, 0.15000, 0.65000
  };
  int n[] = { 12500, 12500, 12500, 12500 };
  double **A = Arrays<double>::allocMatrix(4,4,valA);

  // performing cholesky factorization
  BlockMatrixChol *chol=NULL;
  ASSERT_NO_THROW(chol = new BlockMatrixChol(A, n, 4));

  // checking optimized L·x
  double x[50000], y[50000];
  for(int i=0;i<50000;i++) x[i] = 1.0;
  chol->mult(x, y);

  // exit function
  delete chol;
  Arrays<double>::deallocMatrix(A, 4);
*/
}

//===========================================================================
// test2. test cholesky decomposition (M=4, N=3), sector2 have 0 elements
//
// validated with octave using:
//    A = [1,0.1,0.2;0.1,1,0.3;0.2,0.3,1]
//      1.00000  0.10000  0.20000
//      0.10000  1.00000  0.30000
//      0.20000  0.30000  1.00000
//    B = chol(A)' (octave & mathematica considerer L'·L instead of L·L')
//      1.00000  0.00000  0.00000
//      0.10000  0.99499  0.00000
//      0.20000  0.28141  0.93851
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test7()
{
  // only 1 element per sector (autocorrelation not used => we put 1000)
  // sector2 (0 elements have values 1000 to force error if exist)
  double valA[] = {
      1000.00, 1000.00, 0.10000, 0.20000,
      1000.00, 1000.00, 1000.00, 1000.00,
      0.10000, 1000.00, 1000.00, 0.30000,
      0.20000, 1000.00, 0.30000, 1000.00
  };
  int n[] = { 1, 0, 1, 1 };
  double solA[] = {
     1.00000, 0.00000, 0.00000,
     0.10000, 0.99499, 0.00000,
     0.20000, 0.28141, 0.93851
  };
  double **A = Arrays<double>::allocMatrix(4,4,valA);

  BlockMatrixChol *chol=NULL;

  ASSERT_NO_THROW(chol = new BlockMatrixChol(A, n, 4));
  ASSERT_EQUALS(3, chol->getDim());

  // checking cholesky values
  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      ASSERT_DOUBLES_EQUAL(solA[3*i+j], chol->get(i,j), EPSILON);
    }
  }

  // checking mult method
  double x[3] = {1.0, 1.0, 1.0};
  double y[3] = {0.0, 0.0, 0.0};
  double z[3] = {1.0, 1.09499, 1.41992};
  chol->mult(x, y);
  for(int i=0;i<3;i++)
  {
    ASSERT_DOUBLES_EQUAL(y[i], z[i], EPSILON);
  }

  // checking that a 0 elements matrix decomposition crash
  delete chol;
  n[0] = 0; n[1] = 0; n[2] = 0; n[3] = 0;
  ASSERT_THROW(chol = new BlockMatrixChol(A, n, 4));

  // exit function
  Arrays<double>::deallocMatrix(A, 4);
}

//===========================================================================
//  given a blockmatrix, A, and his cholesky decomposition, L, check that:
//    L·L' = A
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::check(double **C, int *n, int M, BlockMatrixChol *chol) throw(Exception)
{
  // retrieving the dimension matrix
  int N = chol->getDim();

  // allocating memory for spe array
  int *spe = Arrays<int>::allocVector(N, 0);

  // filling spe
  for (int i=0,j=0;j<M;j++)
  {
    for(int k=0;k<n[j];k++)
    {
      spe[i] = j;
      i++;
    }
  }

  // checking L·L'=A
  for (int i=0;i<N;i++)
  {
    for (int j=0;j<N;j++)
    {
      double val1 = (i==j?1.0:C[spe[i]][spe[j]]);
      double val2 = 0.0;

      for (int k=0;k<N;k++)
      {
        val2 += chol->get(i,k)*chol->get(j,k);
      }

      if (fabs(val1-val2) > EPSILON)
      {
        Arrays<int>::deallocVector(spe);
        throw Exception("check failed");
      }
    }
  }

  // dealloc spe array
  Arrays<int>::deallocVector(spe);
}
