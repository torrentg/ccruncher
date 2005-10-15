
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
// CholeskyDecompositionTest.cpp - CholeskyDecompositionTest code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Arrays class
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <iostream>
#include <cmath>
#include "math/CholeskyDecomposition.hpp"
#include "math/CholeskyDecompositionTest.hpp"
#include "utils/Arrays.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00001

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::CholeskyDecompositionTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::CholeskyDecompositionTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1. test standar cholesky decomposition
//
// validated with octave using:
//    A = [2,1,1;1,2,1;1,1,2]
//      2  1  1
//      1  2  1
//      1  1  2
//    B = chol(A)
//      1.41421  0.70711  0.70711
//      0.00000  1.22474  0.40825
//      0.00000  0.00000  1.15470
//===========================================================================
void ccruncher_test::CholeskyDecompositionTest::test1()
{
  // cholesky decomposition feasible
  double valA[] = {
     +2.0, +1.0, +1.0,
     +1.0, +2.0, +1.0,
     +1.0, +1.0, +2.0
  };
  // chol(A) values (extracted with octave)
  double solA[] = {
     +1.41421, +0.70711, +0.70711,
     +0.00000, +1.22474, +0.40825,
     +0.00000, +0.00000, +1.15470
  };
  double **A = Arrays<double>::allocMatrix(3,3,valA);
  double *aux = Arrays<double>::allocVector(3);

  ASSERT(CholeskyDecomposition::choldc(A, aux, 3));

  // moving diagonal elements to A
  for(int i=0;i<3;i++)
  {
    A[i][i] = aux[i];
  }

  // attention, octave & mathematica make L'·L and choldc make L·L'
  for (int i=0;i<3;i++)
  {
    for (int j=i;j<3;j++)
    {
      ASSERT_DOUBLES_EQUAL(solA[j+3*i], A[j][i], EPSILON);
    }
  }

  Arrays<double>::deallocMatrix(A, 3);
  Arrays<double>::deallocVector(aux);
}

//===========================================================================
// test2. non feasible cholesky decomposition test
//
// validated with octave using:
//    A = [2,1,4;2,2,5;2,1,6]
//      2  1  4
//      2  2  5
//      2  1  6
//    B = chol(A)
//      error: chol: matrix not positive definite
//===========================================================================
void ccruncher_test::CholeskyDecompositionTest::test2()
{
  // cholesky decomposition non feasible
  double valB[] = {
     +2.0, +1.0, +4.0,
     +2.0, +2.0, +5.0,
     +2.0, +1.0, +6.0
  };
  double **B = Arrays<double>::allocMatrix(3,3, valB);
  double *aux = Arrays<double>::allocVector(3);

  ASSERT(!CholeskyDecomposition::choldc(B, aux, 3));

  Arrays<double>::deallocMatrix(B, 3);
  Arrays<double>::deallocVector(aux);
}
