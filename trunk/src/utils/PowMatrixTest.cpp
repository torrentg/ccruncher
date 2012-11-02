
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
#include "utils/PowMatrix.hpp"
#include "utils/PowMatrixTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00175

//===========================================================================
// test1. test standar pow matrix
//
// validated with Mathematica (Wolfram Research) using:
//    A = {{1.0, 0.1, 0.3},{0.1, 1.0, 0.0},{0.3, 0.0, 1.0}}
//    A // MatrixForm
//        1.0, 0.1, 0.3,
//        0.1, 1.0, 0.0,
//        0.3, 0.0, 1.0
//    MatrixPower[A, 0.3] // MatrixForm
//        +0.989073  +0.0306268  +0.0918803
//        +0.0306268 +0.998907   -0.00327799
//        +0.0918803 -0.00327799 +0.990166
//===========================================================================
void ccruncher_test::PowMatrixTest::test1()
{
  // matrix
  double valA[] = {
     +1.0, +0.1, +0.3,
     +0.1, +1.0, +0.0,
     +0.3, +0.0, +1.0
  };
  // A^(0.3) values
  double solA[] = {
     +0.989073 , +0.0306268 , +0.0918803,
     +0.0306268, +0.998907  , -0.00327799,
     +0.0918803, -0.00327799, +0.990166
  };

  vector<vector<double> > A(3, vector<double>(3));
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) A[i][j] = valA[3*i+j];
  vector<vector<double> > M(3, vector<double>(3));

  ASSERT_NO_THROW(PowMatrix::pow(A, 0.3, M));

  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      ASSERT_EQUALS_EPSILON(solA[j+3*i], M[i][j], EPSILON);
    }
  }

  // checking array version
  double *aA[3] = {valA+0, valA+3, valA+6};
  double aux[9];
  double *aM[3] = {aux+0, aux+3, aux+6};

  ASSERT_NO_THROW(PowMatrix::pow(aA, 0.3, 3, aM));

  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
       ASSERT_EQUALS_EPSILON(solA[j+3*i], aM[i][j], EPSILON);
    }
  }

}

//===========================================================================
// test2. test standar pow matrix
//
// validated with Mathematica (Wolfram Research) using:
//    A = {{1.0, 0.1, 0.3},{0.1, 1.0, 0.0},{2.0, 0.2, 0.6}}
//    A // MatrixForm
//      A =
//        1.0, 0.1, 0.3,
//        0.1, 1.0, 0.0,
//        2.0, 0.2, 0.6
//    MatrixPower[A, 0.2] // MatrixForm
//      A^0.2 =
//        +0.684587  +0.0164512 +0.206761
//        +0.0479328 +0.999403  -0.0157408
//        +1.36792   +0.0329025 +0.414152
//===========================================================================
void ccruncher_test::PowMatrixTest::test2()
{
  // matrix
  double valA[] = {
     +1.0, +0.1, +0.3,
     +0.1, +1.0, +0.0,
     +2.0, +0.2, +0.6
  };
  // A^(0.2) values
  double solA[] = {
     +0.684587 , +0.0164512 , +0.206761,
     +0.0479328, +0.999403  , -0.0157408,
     +1.36792  , +0.0329025 , +0.414152
  };

  vector<vector<double> > A(3, vector<double>(3));
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) A[i][j] = valA[3*i+j];
  vector<vector<double> > M(3, vector<double>(3));

  ASSERT_NO_THROW(PowMatrix::pow(A, 0.2, M));

  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      ASSERT_EQUALS_EPSILON(solA[j+3*i], M[i][j], EPSILON);
    }
  }
}


//===========================================================================
// test3. try to pow a non-invertible matrix (singular matrix)
//
// validated with Mathematica (Wolfram Research) using:
//    A = {{1.0, 0.1, 0.3},{0.1, 1.0, 0.0},{1.0, 0.1, 0.3}}
//    A // MatrixForm
//      A =
//        1.0, 0.1, 0.3,
//        0.1, 1.0, 0.0,
//        2.0, 0.2, 0.6
//    Det[A]
//      = 0.
//    MatrixPower[A, 0.2] // MatrixForm
//      A^0.2 =
//        0.808758  0.0179912  0.244533
//        0.0370471 0.999317  -0.0190559
//        0.808758  0.0179912  0.244533
//===========================================================================
void ccruncher_test::PowMatrixTest::test3()
{
  // matrix
  double valA[] = {
     +1.0, +0.1, +0.3,
     +0.1, +1.0, +0.0,
     +1.0, +0.1, +0.3
  };
  // A^(0.2) values
  double solA[] = {
    0.808758,  0.0179912,  0.244533,
    0.0370471, 0.999317,  -0.0190559,
    0.808758,  0.0179912,  0.244533
  };

  vector<vector<double> > A(3, vector<double>(3));
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) A[i][j] = valA[3*i+j];
  vector<vector<double> > M(3, vector<double>(3));

  ASSERT_NO_THROW(PowMatrix::pow(A, 0.2, M));

  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      ASSERT_EQUALS_EPSILON(solA[j+3*i], M[i][j], EPSILON);
    }
  }
}

//===========================================================================
// test4. try to pow a matrix with complex eigenvalues
//
// validated with Mathematica (Wolfram Research) using:
//    A = {{3, -2, 0},{4, -1, -2}, {0, 0, -1}}
//    Eigenvalues[A]
//===========================================================================
void ccruncher_test::PowMatrixTest::test4()
{
  // matrix with complex eigenvalues
  double valA[] = {
     +3.0, -2.0, +0.0,
     +4.0, -1.0, -2.0,
     +0.0, +0.0, -1.0
  };

  vector<vector<double> > A(3, vector<double>(3));
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) A[i][j] = valA[3*i+j];
  vector<vector<double> > M(3, vector<double>(3));

  ASSERT_THROW(PowMatrix::pow(A, 0.3, M));
}

//===========================================================================
// test5. testing pow function
//===========================================================================
void ccruncher_test::PowMatrixTest::test5()
{
  double x = 2.0;
  double y = 1.0/3.0;

  ASSERT_EQUALS_EPSILON(ccruncher::fpow(+2.0, +3.0), +8.0, 1E-6);
  ASSERT_EQUALS_EPSILON(ccruncher::fpow(-2.0, +3.0), -8.0, 1E-6);
  ASSERT_EQUALS_EPSILON(ccruncher::fpow(+2.0, -3.0), +1.0/8.0, 1E-6);
  ASSERT_EQUALS_EPSILON(ccruncher::fpow(-2.0, -3.0), -1.0/8.0, 1E-6);

  ASSERT_EQUALS_EPSILON(ccruncher::fpow(+x, +y), +1.25992115, 1E-6);
  ASSERT_EQUALS_EPSILON(ccruncher::fpow(+x, -y), +0.79370115, 1E-6);
  ASSERT_EQUALS_EPSILON(ccruncher::fpow(-x, +y), -1.25992115, 1E-6);
  ASSERT_EQUALS_EPSILON(ccruncher::fpow(-x, -y), -0.79370115, 1E-6);

}

//===========================================================================
// printMatrix. local function used for debug
//===========================================================================
void ccruncher_test::PowMatrixTest::printMatrix(double **a, int n)
{
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      cout << a[i][j] << " \t";
    }
    cout << endl;
  }
}
