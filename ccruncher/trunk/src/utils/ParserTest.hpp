
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
// ParserTest.hpp - ParserTest header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//===========================================================================

#ifndef _ParserTest_
#define _ParserTest_

//---------------------------------------------------------------------------

#include <MiniCppUnit.hxx>

//---------------------------------------------------------------------------

using namespace ccruncher;

//---------------------------------------------------------------------------

class ParserTest : public TestFixture<ParserTest>
{

  private:

    void test_int(void);
    void test_long(void);
    void test_double(void);
    void test_date(void);
    void test_bool(void);
    void test_toString(void);


  public:

    TEST_FIXTURE(ParserTest)
    {
      TEST_CASE(test_int);
      TEST_CASE(test_long);
      TEST_CASE(test_double);
      TEST_CASE(test_date);
      TEST_CASE(test_bool);
      TEST_CASE(test_toString);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(ParserTest);

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
