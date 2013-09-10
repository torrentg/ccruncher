
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#ifndef _ParamsTest_
#define _ParamsTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/MiniCppUnit.hxx"

//---------------------------------------------------------------------------

namespace ccruncher_test {

//---------------------------------------------------------------------------

class ParamsTest : public TestFixture<ParamsTest>
{

  private:

    void test1();
    void test2();
    void test3();
    void test4();

  public:

    TEST_FIXTURE(ParamsTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
      TEST_CASE(test4);
    }

};

REGISTER_FIXTURE(ParamsTest)

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
