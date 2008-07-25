
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
// ProvaTest.cpp - ProvaTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#include <string>
#include "ProvaTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-12

//***************************************************************************
// setUp
//***************************************************************************
void ProvaTest::setUp()
{
  // nothing to do
}

//***************************************************************************
// setUp
//***************************************************************************
void ProvaTest::tearDown()
{
  // nothing to do
}

//***************************************************************************
// test_assert_equals
//***************************************************************************
void ProvaTest::test_assert_equals()
{
  int a = 3;
  double b = 3.0;
  
  ASSERT_EQUALS(3, a); // ok
  ASSERT_EQUALS(3, b); // ok
}

//***************************************************************************
// test_assert
//***************************************************************************
void ProvaTest::test_assert()
{
  int a=1, b=1;
  
  ASSERT(a == b); // ok
}

//***************************************************************************
// test_assert_message
//***************************************************************************
void ProvaTest::test_assert_message()
{
  int a=1, b=2;
  
  ASSERT_MESSAGE(a == b, "assignement fails!"); // ko
}

//***************************************************************************
// test_fail
//***************************************************************************
void ProvaTest::test_fail()
{
  int a=1, b=2;
  
  if (a != b)
  {
    FAIL("assignement fails!");
  }
}

//***************************************************************************
// test_assert_throw
//***************************************************************************
void ProvaTest::test_assert_throw()
{
  ASSERT_THROW(throwFunc(true));  // ok
  ASSERT_THROW(throwFunc(false)); // ko
}

//***************************************************************************
// test_assert_no_throw
//***************************************************************************
void ProvaTest::test_assert_no_throw()
{
  ASSERT_NO_THROW(throwFunc(false)); // ok
  ASSERT_NO_THROW(throwFunc(true));  // ko
}

//***************************************************************************
// test_assert_doubles_equal
//***************************************************************************
void ProvaTest::test_assert_doubles_equal()
{
  double x=1.0, y=1.000000000000000001;
  
  ASSERT_DOUBLES_EQUAL(x, y, EPSILON);     // ok
  ASSERT_DOUBLES_EQUAL(x, y+0.1, EPSILON); // ko
}

//***************************************************************************
// throwfunc. internal suport function
//***************************************************************************
void ProvaTest::throwFunc(bool x)
{
  if (x == true)
  {
    throw 1;
  }
  else
  {
    // nothing to do
  }
}
