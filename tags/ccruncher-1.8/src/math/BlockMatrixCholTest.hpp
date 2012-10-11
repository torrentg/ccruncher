
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

#ifndef _BlockMatrixCholTest_
#define _BlockMatrixCholTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>
#include "math/BlockMatrixChol.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class BlockMatrixCholTest : public TestFixture<BlockMatrixCholTest>
{

  private:

    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();
    void test8();
    void test9();
    void test10();
    void test11();
    void run(double *correls, double *solution, double condnum, int *n, int M);
    void check(double **correls, int *n, int M, BlockMatrixChol *chol);


  public:

    TEST_FIXTURE(BlockMatrixCholTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
      TEST_CASE(test4);
      TEST_CASE(test5);
      TEST_CASE(test6);
      TEST_CASE(test7);
      TEST_CASE(test8);
      TEST_CASE(test9);
      TEST_CASE(test10);
      TEST_CASE(test11);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(BlockMatrixCholTest);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------