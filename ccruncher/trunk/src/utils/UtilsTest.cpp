
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
// UtilsTest.cpp - UtilsTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (see jama/tnt_stopwatch && boost/timer)
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//===========================================================================

#include <iostream>
#include <string>
#include "Utils.hpp"
#include "UtilsTest.hpp"

//---------------------------------------------------------------------------

#define XLENGTH 5
#define YLENGTH 7
#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void UtilsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void UtilsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void UtilsTest::test1()
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

  vector1 = Utils::allocVector(XLENGTH);
  vector2 = Utils::allocVector(XLENGTH, vvals);
  vector3 = Utils::allocVector(YLENGTH, 1.0);

  for (int i=0;i<XLENGTH;i++)
  {
    ASSERT(isnan(vector1[i]));
    ASSERT_DOUBLES_EQUAL(vvals[i], vector2[i], EPSILON);
  }

  for (int i=0;i<YLENGTH;i++)
  {
    ASSERT_DOUBLES_EQUAL(1.0, vector3[i], EPSILON);
  }

  Utils::initVector(vector1, XLENGTH, 1.0);

  for (int i=0;i<XLENGTH;i++)
  {
    ASSERT_DOUBLES_EQUAL(1.0, vector1[i], EPSILON);
  }

  Utils::copyVector(vector1, XLENGTH, vector2);

  for (int i=0;i<XLENGTH;i++)
  {
    ASSERT_DOUBLES_EQUAL(vector1[i], vector2[i], EPSILON);
  }

  matrix1 = Utils::allocMatrix(XLENGTH, YLENGTH);
  matrix2 = Utils::allocMatrix(XLENGTH, YLENGTH, 1.0);
  matrix3 = Utils::allocMatrix(XLENGTH, YLENGTH, mvals);
  matrix4 = Utils::allocMatrix(XLENGTH, YLENGTH, matrix3);

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

  Utils::initMatrix(matrix1, XLENGTH, YLENGTH, 1.0);

  for (int i=0;i<XLENGTH;i++)
  {
    for (int j=0;j<YLENGTH;j++)
    {
      ASSERT_DOUBLES_EQUAL(1.0, matrix1[i][j], EPSILON);
    }
  }

  Utils::copyMatrix(matrix1, XLENGTH, YLENGTH, matrix3);

  for (int i=0;i<XLENGTH;i++)
  {
    for (int j=0;j<YLENGTH;j++)
    {
      ASSERT_DOUBLES_EQUAL(1.0, matrix3[i][j], EPSILON);
    }
  }

  Utils::prodMatrixVector(matrix1, vector3, XLENGTH, YLENGTH, vector1);

  for (int j=0;j<XLENGTH;j++)
  {
    ASSERT_DOUBLES_EQUAL(7.0, vector1[j], EPSILON);
  }

  Utils::deallocVector(vector1);
  Utils::deallocVector(vector2);
  Utils::deallocVector(vector3);

  Utils::deallocMatrix(matrix1, XLENGTH);
  Utils::deallocMatrix(matrix2, XLENGTH);
  Utils::deallocMatrix(matrix3, XLENGTH);
  Utils::deallocMatrix(matrix4, XLENGTH);
}

//===========================================================================
// test2
//===========================================================================
void UtilsTest::test2()
{
  vector<string> tokens;
  string str1 = "  I Am A String Ready For Sacrifice  ";

  Utils::tokenize(str1, tokens, " ");

  ASSERT(7 == tokens.size());

  ASSERT(tokens[0] == "I");
  ASSERT(tokens[1] == "Am");
  ASSERT(tokens[2] == "A");
  ASSERT(tokens[3] == "String");
  ASSERT(tokens[4] == "Ready");
  ASSERT(tokens[5] == "For");
  ASSERT(tokens[6] == "Sacrifice");

  ASSERT("  I Am A String Ready For Sacrifice" == Utils::rtrim(str1));
  ASSERT("I Am A String Ready For Sacrifice  " == Utils::ltrim(str1));
  ASSERT("I Am A String Ready For Sacrifice" == Utils::trim(str1));
  ASSERT("  I AM A STRING READY FOR SACRIFICE  " == Utils::uppercase(str1));
  ASSERT("  i am a string ready for sacrifice  " == Utils::lowercase(str1));
  ASSERT("   " == Utils::blanks(3));
}
