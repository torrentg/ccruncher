
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
// ParserTest.cpp - ParserTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//===========================================================================

#include <iostream>
#include <string>
#include <climits>
#include "Parser.hpp"
#include "ParserTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void ParserTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ParserTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test_int
//===========================================================================
void ParserTest::test_int(void)
{
  ASSERT_EQUALS(1, Parser::intValue("1"));
  ASSERT_EQUALS(2, Parser::intValue("+2"));
  ASSERT_EQUALS(-3, Parser::intValue("-3"));
  ASSERT_THROW(Parser::intValue("4.5"));
  ASSERT_THROW(Parser::intValue("55555555555555555555555"));
  ASSERT_THROW(Parser::intValue(" 6 "));
  ASSERT_THROW(Parser::intValue(" -7 "));
  ASSERT_THROW(Parser::intValue(" - 8 "));
  ASSERT_THROW(Parser::intValue("- 9"));
}

//===========================================================================
// test_long
//===========================================================================
void ParserTest::test_long(void)
{
  ASSERT_EQUALS(1L, Parser::longValue("1"));
  ASSERT_EQUALS(2L, Parser::longValue("+2"));
  ASSERT_EQUALS(-3L, Parser::longValue("-3"));
  ASSERT_THROW(Parser::longValue("4.5"));
  ASSERT_EQUALS(2147483647L, Parser::longValue("2147483647"));
  ASSERT_THROW(Parser::longValue(" 6 "));
  ASSERT_THROW(Parser::longValue(" -7 "));
  ASSERT_THROW(Parser::longValue(" - 8 "));
  ASSERT_THROW(Parser::longValue("- 9"));
}

//===========================================================================
// test_double
//===========================================================================
void ParserTest::test_double(void)
{
  ASSERT_DOUBLES_EQUAL(1.0, Parser::doubleValue("1"), EPSILON);
  ASSERT_DOUBLES_EQUAL(2.0, Parser::doubleValue("+2"), EPSILON);
  ASSERT_DOUBLES_EQUAL(-3.0, Parser::doubleValue("-3"), EPSILON);
  ASSERT_DOUBLES_EQUAL(4.5, Parser::doubleValue("4.5"), EPSILON);
  ASSERT_DOUBLES_EQUAL(4.0, Parser::doubleValue("4."), EPSILON);
  ASSERT_THROW(Parser::doubleValue(" 6 "));
  ASSERT_DOUBLES_EQUAL(-10.12345365457657886789699, Parser::doubleValue("-10.12345365457657886789699"), EPSILON);
  ASSERT_DOUBLES_EQUAL(-12343246556457666756876867968907609.12, Parser::doubleValue("-12343246556457666756876867968907609.12"), EPSILON);
  ASSERT_DOUBLES_EQUAL(0.9, Parser::doubleValue(".9"), EPSILON);
  ASSERT_DOUBLES_EQUAL(0.1, Parser::doubleValue("+.10"), EPSILON);
  ASSERT_DOUBLES_EQUAL(-0.11, Parser::doubleValue("-.11"), EPSILON);
}

//===========================================================================
// date_test
//===========================================================================
void ParserTest::test_date(void)
{
  Date date1 = Parser::dateValue("5/1/2001");

  ASSERT_EQUALS(5, date1.getDay());
  ASSERT_EQUALS(1, date1.getMonth());
  ASSERT_EQUALS(2001, date1.getYear());

  ASSERT_THROW(Parser::dateValue("-01/01/2001"));
  ASSERT_THROW(Parser::dateValue("01.01.2001"));
  ASSERT_THROW(Parser::dateValue("01-01-2001"));
  ASSERT_THROW(Parser::dateValue("01:01:2001"));
  ASSERT_THROW(Parser::dateValue("35/01/2000"));
  ASSERT_THROW(Parser::dateValue("01/01/2000 11:30"));
  ASSERT_THROW(Parser::dateValue("29/02/2003"));
  ASSERT_THROW(Parser::dateValue("02/13/2000"));
}

//===========================================================================
// toString
//===========================================================================
void ParserTest::test_toString(void)
{
  Date date1 = Date("1/1/2005");

  ASSERT("35" == Parser::int2string(35));  
  ASSERT("50000" == Parser::long2string(50000));  
  ASSERT("3.1415" == Parser::double2string(3.1415));  
  ASSERT("01/01/2005" == Parser::date2string(date1));  
}
