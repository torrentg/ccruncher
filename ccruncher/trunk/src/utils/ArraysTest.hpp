
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
// ArraysTest.hpp - ArraysTest header
// --------------------------------------------------------------------------
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (segregated from UtilsTest.hpp)
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . created ccruncher_test namespace
//
//===========================================================================

#ifndef _ArraysTest_
#define _ArraysTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class ArraysTest : public TestFixture<ArraysTest>
{

  private:

    void test1(void);
    void test2(void);


  public:

    TEST_FIXTURE(ArraysTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(ArraysTest);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
