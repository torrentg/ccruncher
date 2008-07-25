
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
// GaussianCopulaTest.hpp - GaussianCopulaTest header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/07/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/07/24 - Gerard Torrent [gerard@mail.generacio.com]
//   . class CopulaNormal renamed to GaussianCopula
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#ifndef _GaussianCopulaTest_
#define _GaussianCopulaTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>
#include "math/GaussianCopula.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class GaussianCopulaTest : public TestFixture<GaussianCopulaTest>
{

  private:

    double pearsn(double *x, double *y, int n);
    void testCopula(GaussianCopula &, double *, int n);
    void computeDensity(GaussianCopula &copula);

    void test1();
    void test2();
    void test3();
    void test4();
    void test5();


  public:

    TEST_FIXTURE(GaussianCopulaTest)
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

REGISTER_FIXTURE(GaussianCopulaTest);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
