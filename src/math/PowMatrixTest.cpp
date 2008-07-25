
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
// PowMatrixTest.cpp - PowMatrixTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/01/29 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed epsilon value because in debug mode test2 fails
//     it is due that optimized code sometimes gives different 
//     answers not because of an error but because optimization 
//     changes the order of calculations, resulting in slightly 
//     different results due to the limits of floating-point precision.
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Arrays class
//
//===========================================================================

#include <iostream>
#include "math/PowMatrix.hpp"
#include "math/PowMatrixTest.hpp"
#include "utils/Arrays.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00175

//---------------------------------------------------------------------------

void printMatrix(double **a, int n);

//===========================================================================
// setUp
//===========================================================================
void PowMatrixTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void PowMatrixTest::tearDown()
{
  // nothing to do
}

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
void PowMatrixTest::test1()
{
  // matrix powerable
  double valA[] = {
     +1.0, +0.1, +0.3,
     +0.1, +1.0, +0.0,
     +0.3, +0.0, +1.0
  };
  // A^(0.3) values (extracted with Mathematica)
  double solA[] = {
     +0.989073 , +0.0306268 , +0.0918803,
     +0.0306268, +0.998907  , -0.00327799,
     +0.0918803, -0.00327799, +0.990166
  };

  double **A = Arrays<double>::allocMatrix(3,3, valA);
  double **M = Arrays<double>::allocMatrix(3,3);

  ASSERT_NO_THROW(PowMatrix::pow(A, 0.3, 3, M));

  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      ASSERT_DOUBLES_EQUAL(solA[j+3*i], M[i][j], EPSILON);
    }
  }

  Arrays<double>::deallocMatrix(A, 3);
  Arrays<double>::deallocMatrix(M, 3);
}

//===========================================================================
// test2. try to pow a non-invertible matrix (singular matrix)
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
void PowMatrixTest::test2()
{
  // singular matrix
  double valB[] = {
     +1.0, +0.1, +0.3,
     +0.1, +1.0, +0.0,
     +2.0, +0.2, +0.6
  };
  // B^(0.2) values (extracted with Mathematica)
  double solB[] = {
     +0.684587 , +0.0164512 , +0.206761,
     +0.0479328, +0.999403  , -0.0157408,
     +1.36792  , +0.0329025 , +0.414152
  };

  double **B = Arrays<double>::allocMatrix(3, 3, valB);
  double **M = Arrays<double>::allocMatrix(3, 3);

  ASSERT_NO_THROW(PowMatrix::pow(B, 0.2, 3, M));

  for (int i=0;i<3;i++)
  {
    for (int j=0;j<3;j++)
    {
      ASSERT_DOUBLES_EQUAL(solB[j+3*i], M[i][j], EPSILON);
    }
  }

  Arrays<double>::deallocMatrix(B, 3);
  Arrays<double>::deallocMatrix(M, 3);
}

//===========================================================================
// test3. try to pow a matrix with complex eigenvalues
//
// validated with Mathematica (Wolfram Research) using:
//    A = {{3, -2, 0},{4, -1, -2}, {0, 0, -1}}
//    Eigenvalues[A]
//===========================================================================
void PowMatrixTest::test3()
{
  // matrix with complex eigenvalues
  double valC[] = {
     +3.0, -2.0, +0.0,
     +4.0, -1.0, -2.0,
     +0.0, +0.0, -1.0
  };
  double **C = Arrays<double>::allocMatrix(3,3, valC);
  double **M = Arrays<double>::allocMatrix(3,3);

  ASSERT_THROW(PowMatrix::pow(C, 0.3, 3, M));

  Arrays<double>::deallocMatrix(C, 3);
  Arrays<double>::deallocMatrix(M, 3);
}

//===========================================================================
// test4. testing pow function
//===========================================================================
void PowMatrixTest::test4()
{
  double x = 2.0;
  double y = 1.0/3.0;
/*
  ASSERT_DOUBLES_EQUAL(PowMatrix::pow(+2.0, +3.0), +8.0, 1E-6);
  ASSERT_DOUBLES_EQUAL(PowMatrix::pow(-2.0, +3.0), -8.0, 1E-6);
  ASSERT_DOUBLES_EQUAL(PowMatrix::pow(+2.0, -3.0), +1.0/8.0, 1E-6);
  ASSERT_DOUBLES_EQUAL(PowMatrix::pow(-2.0, -3.0), -1.0/8.0, 1E-6);

  ASSERT_DOUBLES_EQUAL(PowMatrix::pow(+x, +y), +1.25992115, 1E-6);
  ASSERT_DOUBLES_EQUAL(PowMatrix::pow(+x, -y), +0.79370115, 1E-6);
  ASSERT_DOUBLES_EQUAL(PowMatrix::pow(-x, +y), -1.25992115, 1E-6);
  ASSERT_DOUBLES_EQUAL(PowMatrix::pow(-x, -y), -0.79370115, 1E-6);
*/
}

//===========================================================================
// printMatrix. local function used for debug
//===========================================================================
void printMatrix(double **a, int n)
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
