
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2008 Gerard Torrent
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
// TStudentTest.hpp - TStudentTest header - $Rev: 435 $
// --------------------------------------------------------------------------
//
// 2008/12/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _TStudentTest_
#define _TStudentTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class TStudentTest : public TestFixture<TStudentTest>
{

  private:

    void test_pdf();
    void test_cdf();
    void test_cdfinv();


  public:

    TEST_FIXTURE(TStudentTest)
    {
      TEST_CASE(test_pdf);
      TEST_CASE(test_cdf);
      TEST_CASE(test_cdfinv);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(TStudentTest);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
