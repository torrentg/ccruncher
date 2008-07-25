
//***************************************************************************
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
// ProvaTest.hpp - ProvaTest header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#ifndef _ProvaTest_
#define _ProvaTest_

//---------------------------------------------------------------------------

#include <MiniCppUnit.hxx>

//---------------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------------

class ProvaTest : public TestFixture<ProvaTest>
{

  private:
  
    void throwFunc(bool);
    
    void test_assert_equals();
    void test_assert();
    void test_assert_message();
    void test_fail();
    void test_assert_throw();
    void test_assert_no_throw();
    void test_assert_doubles_equal();  
  
  public:
    
    TEST_FIXTURE( ProvaTest )
    {
      TEST_CASE(test_assert_equals);
      TEST_CASE(test_assert);
      TEST_CASE(test_assert_message);
      TEST_CASE(test_fail);
      TEST_CASE(test_assert_throw);
      TEST_CASE(test_assert_no_throw);
      TEST_CASE(test_assert_doubles_equal);
    }
    
    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(ProvaTest);

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
