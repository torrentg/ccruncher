
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
// SurvivalTest.hpp - SurvivalTest header
// --------------------------------------------------------------------------
//
// 2005/05/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/06/28 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added test6()
//
//===========================================================================

#ifndef _SurvivalTest_
#define _SurvivalTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>
#include "ratings/Ratings.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;

//---------------------------------------------------------------------------

class SurvivalTest : public TestFixture<SurvivalTest>
{

  private:

    Ratings getRatings();

    void test1(void);
    void test2(void);
    void test3(void);
    void test4(void);
    void test5(void);
    void test6(void);


  public:

    TEST_FIXTURE(SurvivalTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
      TEST_CASE(test4);
      TEST_CASE(test5);
      TEST_CASE(test6);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(SurvivalTest);

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
