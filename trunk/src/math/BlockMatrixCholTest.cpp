
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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
#include "math/BlockMatrixChol.hpp"
#include "math/BlockMatrixCholTest.hpp"
#include "utils/Arrays.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.0001

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
// test1. 
// test cholesky decomposition (M=1, N=3)
// validated with octave using:
//    A = [1,0.5,0.5;0.5,1,0.5;0.5,0.5,1]
//      1.00000  0.50000  0.50000
//      0.50000  1.00000  0.50000
//      0.50000  0.50000  1.00000
//    B = chol(A)' (octave & mathematica considerer L'·L instead of L·L')
//      1.00000  0.00000  0.00000
//      0.50000  0.86603  0.00000
//      0.50000  0.28868  0.81650
//    c = cond(B)
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
  double cond = 2.0;
  run(valA, solA, cond, n, 1);
}

//===========================================================================
// test2. 
// test cholesky decomposition (M=3, N=3)
// validated with octave using:
//    A = [1,0.1,0.2;0.1,1,0.3;0.2,0.3,1]
//      1.00000  0.10000  0.20000
//      0.10000  1.00000  0.30000
//      0.20000  0.30000  1.00000
//    B = chol(A)' (octave & mathematica considerer L'·L instead of L·L')
//      1.00000  0.00000  0.00000
//      0.10000  0.99499  0.00000
//      0.20000  0.28141  0.93851
//    c = cond(B)
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
  double cond = 1.4408;
  run(valA, solA, cond, n, 3);
}

//===========================================================================
// test3. 
// test cholesky decomposition (M=2, N=5)
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
//    c = cond(B)
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
  double cond = 2.0406;
  run(valA, solA, cond, n, 2);
}

//===========================================================================
// test4. 
// test cholesky decomposition (M=4, N=100)
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
  if (chol!= NULL) delete chol;
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
  double cond = 261.194;

  // performing cholesky factorization
  BlockMatrixChol *chol=NULL;
  ASSERT_NO_THROW(chol = new BlockMatrixChol(A, n, 4));
  ASSERT_EQUALS_EPSILON(chol->getConditionNumber(), cond, EPSILON);
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
// test7. 
// test cholesky decomposition (M=4, N=3), sector2 have 0 elements
// validated with octave using:
//    A = [1,0.1,0.2;0.1,1,0.3;0.2,0.3,1]
//      1.00000  0.10000  0.20000
//      0.10000  1.00000  0.30000
//      0.20000  0.30000  1.00000
//    B = chol(A)' (octave & mathematica considerer L'·L instead of L·L')
//      1.00000  0.00000  0.00000
//      0.10000  0.99499  0.00000
//      0.20000  0.28141  0.93851
//    c = cond(B)
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
  double cond = 1.4408;
  run(valA, solA, cond, n, 4);
}

//===========================================================================
// test8. 
// test cholesky decomposition (M=1, N=2)
// validated with octave using:
//    A = [1,0.4;0.4,1]
//      1.00000  0.40000
//      0.40000  1.00000
//    B = chol(A)' (octave & mathematica considerer L'·L instead of L·L')
//      1.00000  0.00000
//      0.40000  0.91652
//    c = cond(B)
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test8()
{
  double valA[] = {
     +0.4
  };
  int n[] = { 2 };
  double solA[] = {
      1.00000, 0.00000,
      0.40000, 0.91652
  };
  double cond = 1.5275;
  run(valA, solA, cond, n, 1);
}

//===========================================================================
// test9.
// test cholesky decomposition (M=1, N=1)
// dim(A)=1 that means that A=1 because diagonal values are 1 allways
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test9()
{
  double valA[] = {
     +0.4
  };
  int n[] = { 1 };
  double solA[] = {
      1.00000
  };
  double cond = 1.0;
  run(valA, solA, cond, n, 1);
}

//===========================================================================
// test10. 
// test cholesky decomposition (M=2, various N)
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test10()
{
  double valA[] = {
      0.50000, 0.10000,
      0.10000, 0.35000
  };

  int n1[] = { 1, 1 }; //A=[1,0.1;0.1,1]
  double sol1[] = {    //B=chol(A)'
      1.00000, 0.00000,
      0.10000, 0.99499
  };
  double cond1 = 1.1055; //c=cond(B)
  run(valA, sol1, cond1, n1, 2);

  int n2[] = { 1, 2 }; //A=[1,0.1,0.1; 0.1,1,0.35; 0.1,0.35,1]
  double sol2[] = {    //B=chol(A)'
   1.00000,   0.00000,   0.00000,
   0.10000,   0.99499,   0.00000,
   0.10000,   0.34171,   0.93447
  };
  double cond2 = 1.4676; //c=cond(B)
  run(valA, sol2, cond2, n2, 2);

  int n3[] = { 1, 3 }; //A=[1,0.1,0.1,0.1; 0.1,1,0.35,0.35; 0.1,0.35,1,0.35; 0.1,0.35,0.35,1]
  double sol3[] = {    //B=chol(A)'
   1.00000,   0.00000,   0.00000,   0.00000,
   0.10000,   0.99499,   0.00000,   0.00000,
   0.10000,   0.34171,   0.93447,   0.00000,
   0.10000,   0.34171,   0.23889,   0.90342
  };
  double cond3 = 1.6364; //c=cond(B)
  run(valA, sol3, cond3, n3, 2);

  int n4[] = { 2, 1 }; //A=[1,0.5,0.1; 0.5,1,0.1; 0.1,0.1,1]
  double sol4[] = {    //B=chol(A)'
   1.00000,   0.00000,   0.00000,
   0.50000,   0.86603,   0.00000,
   0.10000,   0.05774,   0.99331
  };
  double cond4 = 1.7534; //c=cond(B)
  run(valA, sol4, cond4, n4, 2);

  int n5[] = { 2, 2 }; //A=[1,0.5,0.1,0.1; 0.5,1,0.1,0.1; 0.1,0.1,1,0.35; 0.1,0.1,0.35,1]
  double sol5[] = {    //B=chol(A)'
   1.00000,   0.00000,   0.00000,   0.00000,
   0.50000,   0.86603,   0.00000,   0.00000,
   0.10000,   0.05774,   0.99331,   0.00000,
   0.10000,   0.05774,   0.33893,   0.93370
  };
  double cond5 = 1.8103; //c=cond(B)
  run(valA, sol5, cond5, n5, 2);

  int n6[] = { 2, 3 }; //A=[1,0.5,0.1,0.1,0.1; 0.5,1,0.1,0.1,0.1; 0.1,0.1,1,0.35,0.35; 0.1,0.1,0.35,1,0.35; 0.1,0.1,0.35,0.35,1]
  double sol6[] = {    //B=chol(A)'
   1.00000,   0.00000,   0.00000,   0.00000,   0.00000,
   0.50000,   0.86603,   0.00000,   0.00000,   0.00000,
   0.10000,   0.05774,   0.99331,   0.00000,   0.00000,
   0.10000,   0.05774,   0.33893,   0.93370,   0.00000,
   0.10000,   0.05774,   0.33893,   0.23754,   0.90298
  };
  double cond6 = 1.9311; //c=cond(B)
  run(valA, sol6, cond6, n6, 2);

  int n7[] = { 3, 1 }; //A=[1,0.5,0.5,0.1; 0.5,1,0.5,0.1; 0.5,0.5,1,0.1; 0.1,0.1,0.1,1]
  double sol7[] = {    //B=chol(A)'
   1.00000,   0.00000,   0.00000,   0.00000,
   0.50000,   0.86603,   0.00000,   0.00000,
   0.50000,   0.28868,   0.81650,   0.00000,
   0.10000,   0.05774,   0.04082,   0.99247
  };
  double cond7 = 2.0145; //c=cond(B)
  run(valA, sol7, cond7, n7, 2);

  int n8[] = { 3, 2 }; //A=[1,0.5,0.5,0.1,0.1; 0.5,1,0.5,0.1,0.1; 0.5,0.5,1,0.1,0.1; 0.1,0.1,0.1,1,0.35; 0.1,0.1,0.1,0.35,1]
  double sol8[] = {    //B=chol(A)'
   1.00000,   0.00000,   0.00000,   0.00000,   0.00000,
   0.50000,   0.86603,   0.00000,   0.00000,   0.00000,
   0.50000,   0.28868,   0.81650,   0.00000,   0.00000,
   0.10000,   0.05774,   0.04082,   0.99247,   0.00000,
   0.10000,   0.05774,   0.04082,   0.33754,   0.93331
  };
  double cond8 = 2.0406; //c=cond(B)
  run(valA, sol8, cond8, n8, 2);

  int n9[] = { 3, 3 }; //A=[1,0.5,0.5,0.1,0.1,0.1; 0.5,1,0.5,0.1,0.1,0.1; 0.5,0.5,1,0.1,0.1,0.1; 0.1,0.1,0.1,1,0.35,0.35; 0.1,0.1,0.1,0.35,1,0.35; 0.1,0.1,0.1,0.35,0.35,1]
  double sol9[] = {    //B=chol(A)'
   1.00000,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000,
   0.50000,   0.86603,   0.00000,   0.00000,   0.00000,   0.00000,
   0.50000,   0.28868,   0.81650,   0.00000,   0.00000,   0.00000,
   0.10000,   0.05774,   0.04082,   0.99247,   0.00000,   0.00000,
   0.10000,   0.05774,   0.04082,   0.33754,   0.93331,   0.00000,
   0.10000,   0.05774,   0.04082,   0.33754,   0.23686,   0.90275
  };
  double cond9 = 2.0907; //c=cond(B)
  run(valA, sol9, cond9, n9, 2);

}

//===========================================================================
// test11.
// verify that cholesky decomposition don't fails if some block has 0
// elements
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::test11()
{
  double valA[] = {
      0.40000, 0.10000, 0.20000,
      0.10000, 0.50000, 0.30000,
      0.20000, 0.30000, 0.60000
  }; 
  int n[] = { 2, 0, 1 }; //A=[1,0.4,0.2; 0.4,1,0.2; 0.2,0.2,1]
  double solA[] = {      //B=chol(A)'
     1.00000,  0.00000,  0.00000,
     0.40000,  0.91652,  0.00000,
     0.20000,  0.13093,  0.97101
  };
  double cond = 1.6054; //c=cond(B)
  run(valA, solA, cond, n, 3);
}

//===========================================================================
//  given a blockmatrix, A, and his cholesky decomposition, L, check that:
//    L·L' = A
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::run(double *correls, double *solution, double condnum, int *n, int M)
{
  int N = 0;
  for(int i=0; i<M; i++) N += n[i];

  double **A = Arrays<double>::allocMatrix(M, M, correls);
  double **B = Arrays<double>::allocMatrix(N, N, solution);

  BlockMatrixChol *chol=NULL;
  ASSERT_NO_THROW(chol = new BlockMatrixChol(A, n, M));
  ASSERT_EQUALS_EPSILON(chol->getConditionNumber(), condnum, EPSILON);
  ASSERT_EQUALS(N, chol->getDim());

  // checking cholesky values
  for (int i=0;i<N;i++)
  {
    for (int j=0;j<N;j++)
    {
      ASSERT_EQUALS_EPSILON(solution[N*i+j], chol->get(i,j), EPSILON);
    }
  }

  // checking mult method
  double *x = Arrays<double>::allocVector(N);
  double *y = Arrays<double>::allocVector(N);
  double *z = Arrays<double>::allocVector(N);
  for(int i=0; i<N; i++) 
  {
    x[i] = (double) i;
    y[i] = -1.0;
  }
  Arrays<double>::prodMatrixVector(B, x, N, N, z);
  chol->mult(x, y);
  for(int i=0;i<N; i++) 
  {
    ASSERT_EQUALS_EPSILON(z[i], y[i], EPSILON);
    double val = 0.0;
    for(int j=0;j<N; j++) 
    {
      val += chol->get(i,j)*x[j];
    }
    ASSERT_EQUALS_EPSILON(val, y[i], EPSILON);
  }

  // checking L·L'=A
  check(A, n, M, chol);

  // exit function
  delete chol;
  Arrays<double>::deallocMatrix(A, M);
  Arrays<double>::deallocMatrix(B, N);
  Arrays<double>::deallocVector(x);
  Arrays<double>::deallocVector(y);
  Arrays<double>::deallocVector(z);
}

//===========================================================================
//  given a blockmatrix, A, and his cholesky decomposition, L, check that:
//    L·L' = A
//===========================================================================
void ccruncher_test::BlockMatrixCholTest::check(double **C, int *n, int M, BlockMatrixChol *chol)
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
        ASSERT(false);
      }
    }
  }

  // dealloc spe array
  Arrays<int>::deallocVector(spe);
}

