
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
// ArraysTest.cpp - ArraysTest code
// --------------------------------------------------------------------------
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (segregated from UtilsTest.cpp)
//
//===========================================================================

#include <iostream>
#include <cmath>
#include <string>
#include "utils/Arrays.hpp"
#include "utils/ArraysTest.hpp"

//---------------------------------------------------------------------------

#define XLENGTH 5
#define YLENGTH 7
#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void ArraysTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ArraysTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ArraysTest::test1()
{
  double *vector1, *vector2, *vector3;
  double **matrix1, **matrix2, **matrix3, **matrix4;
  double vvals[] = {1.0, 2.0, 3.0, 4.0, 5.0};
  double mvals[] = {
    1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,
    2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,
    3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
    4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 1.0,
    5.0, 6.0, 7.0, 8.0, 9.0, 1.0, 2.0
  };

  vector1 = Arrays<double>::allocVector(XLENGTH, NAN);
  vector2 = Arrays<double>::allocVector(XLENGTH, vvals);
  vector3 = Arrays<double>::allocVector(YLENGTH, 1.0);

  for (int i=0;i<XLENGTH;i++)
  {
    ASSERT(isnan(vector1[i]));
    ASSERT_DOUBLES_EQUAL(vvals[i], vector2[i], EPSILON);
  }

  for (int i=0;i<YLENGTH;i++)
  {
    ASSERT_DOUBLES_EQUAL(1.0, vector3[i], EPSILON);
  }

  Arrays<double>::initVector(vector1, XLENGTH, 1.0);

  for (int i=0;i<XLENGTH;i++)
  {
    ASSERT_DOUBLES_EQUAL(1.0, vector1[i], EPSILON);
  }

  Arrays<double>::copyVector(vector1, XLENGTH, vector2);

  for (int i=0;i<XLENGTH;i++)
  {
    ASSERT_DOUBLES_EQUAL(vector1[i], vector2[i], EPSILON);
  }

  matrix1 = Arrays<double>::allocMatrix(XLENGTH, YLENGTH, NAN);
  matrix2 = Arrays<double>::allocMatrix(XLENGTH, YLENGTH, 1.0);
  matrix3 = Arrays<double>::allocMatrix(XLENGTH, YLENGTH, mvals);
  matrix4 = Arrays<double>::allocMatrix(XLENGTH, YLENGTH, matrix3);

  for (int i=0;i<XLENGTH;i++)
  {
    for (int j=0;j<YLENGTH;j++)
    {
      ASSERT(isnan(matrix1[i][j]));
      ASSERT_DOUBLES_EQUAL(1.0, matrix2[i][j], EPSILON);
      ASSERT_DOUBLES_EQUAL(mvals[j+i*YLENGTH], matrix3[i][j], EPSILON);
      ASSERT_DOUBLES_EQUAL(matrix3[i][j], matrix4[i][j], EPSILON);
    }
  }

  Arrays<double>::initMatrix(matrix1, XLENGTH, YLENGTH, 1.0);

  for (int i=0;i<XLENGTH;i++)
  {
    for (int j=0;j<YLENGTH;j++)
    {
      ASSERT_DOUBLES_EQUAL(1.0, matrix1[i][j], EPSILON);
    }
  }

  Arrays<double>::copyMatrix(matrix1, XLENGTH, YLENGTH, matrix3);

  for (int i=0;i<XLENGTH;i++)
  {
    for (int j=0;j<YLENGTH;j++)
    {
      ASSERT_DOUBLES_EQUAL(1.0, matrix3[i][j], EPSILON);
    }
  }

  Arrays<double>::prodMatrixVector(matrix1, vector3, XLENGTH, YLENGTH, vector1);

  for (int j=0;j<XLENGTH;j++)
  {
    ASSERT_DOUBLES_EQUAL(7.0, vector1[j], EPSILON);
  }

  Arrays<double>::deallocVector(vector1);
  Arrays<double>::deallocVector(vector2);
  Arrays<double>::deallocVector(vector3);

  Arrays<double>::deallocMatrix(matrix1, XLENGTH);
  Arrays<double>::deallocMatrix(matrix2, XLENGTH);
  Arrays<double>::deallocMatrix(matrix3, XLENGTH);
  Arrays<double>::deallocMatrix(matrix4, XLENGTH);
}

//===========================================================================
// test2
//===========================================================================
void ArraysTest::test2()
{
  double Avals[] = {1,2,3,4,5,6};
  double **A = Arrays<double>::allocMatrix(2, 3, Avals);
  double Bvals[] = {1,2,3};
  double **B = Arrays<double>::allocMatrix(3, 1, Bvals);
  double Cvals[] = {14,32};
  double **C = Arrays<double>::allocMatrix(2, 1);

  Arrays<double>::prodMatrixMatrix(A, B, 2, 3, 1, C);

  for (int i=0;i<2;i++)
  {
    for(int j=0;j<1;j++)
    {
      ASSERT_DOUBLES_EQUAL(C[i][j], Cvals[i], EPSILON);
    }
  }

  Arrays<double>::deallocMatrix(A, 2);
  Arrays<double>::deallocMatrix(B, 3);
  Arrays<double>::deallocMatrix(C, 2);
}
