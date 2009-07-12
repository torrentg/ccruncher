
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
// DateValuesTest.hpp - DateValuesTest header
// --------------------------------------------------------------------------
//
// 2005/03/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _DateValuesTest_
#define _DateValuesTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>

//---------------------------------------------------------------------------

class DateValuesTest : public TestFixture<DateValuesTest>
{

  private:

    void test1(void);


  public:

    TEST_FIXTURE(DateValuesTest)
    {
      TEST_CASE(test1);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(DateValuesTest);

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------