
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
// BlockGaussianCopulaTest.hpp - BlockGaussianCopulaTest header
// --------------------------------------------------------------------------
//
// 2005/07/24 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _BlockGaussianCopulaTest_
#define _BlockGaussianCopulaTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>
#include "math/BlockGaussianCopula.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class BlockGaussianCopulaTest : public TestFixture<BlockGaussianCopulaTest>
{

  private:

    double pearsn(double *x, double *y, int n);
    void testCopula(BlockGaussianCopula &, double *, int n);
    void computeDensity(BlockGaussianCopula &copula);

    void test1();
    void test2();
    void test3();
    void test4();
    void test5();


  public:

    TEST_FIXTURE(BlockGaussianCopulaTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
      TEST_CASE(test4);
      TEST_CASE(test5);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(BlockGaussianCopulaTest);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
