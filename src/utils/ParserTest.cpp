
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#include <cmath>
#include "utils/Parser.hpp"
#include "utils/ParserTest.hpp"

using namespace ccruncher;

#define EPSILON 1E-14

//===========================================================================
// test_int
//===========================================================================
void ccruncher_test::ParserTest::test_int()
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
void ccruncher_test::ParserTest::test_long()
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
  ASSERT_THROW(Parser::longValue("+ 9"));
}
#include <iostream>
//===========================================================================
// test_ulong
//===========================================================================
void ccruncher_test::ParserTest::test_ulong()
{
  ASSERT_EQUALS(1UL, Parser::ulongValue("1"));
  ASSERT_EQUALS(2UL, Parser::ulongValue("+2"));
  ASSERT_THROW(Parser::ulongValue("-2"));
  ASSERT_THROW(Parser::ulongValue("4.5"));
  ASSERT_EQUALS(2147483647UL, Parser::ulongValue("2147483647"));
  ASSERT_THROW(Parser::ulongValue(" 6 "));
  ASSERT_THROW(Parser::ulongValue(" -7 "));
  ASSERT_THROW(Parser::ulongValue(" - 8 "));
  ASSERT_THROW(Parser::ulongValue("- 9"));
  ASSERT_THROW(Parser::ulongValue("+ 9"));
}

//===========================================================================
// test_double
//===========================================================================
void ccruncher_test::ParserTest::test_double()
{
  ASSERT_EQUALS_EPSILON(1.0, Parser::doubleValue("1"), EPSILON);
  ASSERT_EQUALS_EPSILON(2.0, Parser::doubleValue("+2"), EPSILON);
  ASSERT_EQUALS_EPSILON(-3.0, Parser::doubleValue("-3"), EPSILON);
  ASSERT_EQUALS_EPSILON(4.5, Parser::doubleValue("4.5"), EPSILON);
  ASSERT_EQUALS_EPSILON(4.0, Parser::doubleValue("4."), EPSILON);
  ASSERT_EQUALS_EPSILON(-10.12345365457657886789699, Parser::doubleValue("-10.12345365457657886789699"), EPSILON);
  ASSERT_EQUALS_EPSILON(-12343246556457666756876867968907609.12, Parser::doubleValue("-12343246556457666756876867968907609.12"), EPSILON);
  ASSERT_EQUALS_EPSILON(0.9, Parser::doubleValue(".9"), EPSILON);
  ASSERT_EQUALS_EPSILON(0.1, Parser::doubleValue("+.10"), EPSILON);
  ASSERT_EQUALS_EPSILON(-0.11, Parser::doubleValue("-.11"), EPSILON);

  ASSERT_EQUALS_EPSILON(1.0/100.0, Parser::doubleValue("1%"), EPSILON);
  ASSERT_EQUALS_EPSILON(2.0/100.0, Parser::doubleValue("+2%"), EPSILON);
  ASSERT_EQUALS_EPSILON(-3.0/100.0, Parser::doubleValue("-3%"), EPSILON);
  ASSERT_EQUALS_EPSILON(4.5/100.0, Parser::doubleValue("4.5%"), EPSILON);
  ASSERT_EQUALS_EPSILON(4.0/100.0, Parser::doubleValue("4.%"), EPSILON);
  ASSERT_THROW(Parser::doubleValue(" 6% "));
  ASSERT_THROW(Parser::doubleValue(" 6 % "));
  ASSERT_THROW(Parser::doubleValue("6 %"));
  ASSERT_THROW(Parser::doubleValue("%"));
  ASSERT_THROW(Parser::doubleValue("6%6"));
  ASSERT_EQUALS_EPSILON(-10.12345365457657886789699/100.0, Parser::doubleValue("-10.12345365457657886789699%"), EPSILON);
  ASSERT_EQUALS_EPSILON(-12343246556457666756876867968907609.12/100.0, Parser::doubleValue("-12343246556457666756876867968907609.12%"), EPSILON);
  ASSERT_EQUALS_EPSILON(0.9/100.0, Parser::doubleValue(".9%"), EPSILON);
  ASSERT_EQUALS_EPSILON(0.1/100.0, Parser::doubleValue("+.10%"), EPSILON);
  ASSERT_EQUALS_EPSILON(-0.11/100.0, Parser::doubleValue("-.11%"), EPSILON);

  // expression parsing
  ASSERT_EQUALS_EPSILON(6.0, Parser::doubleValue(" 6 "), EPSILON);
  ASSERT_EQUALS_EPSILON(2.0, Parser::doubleValue("1+1"), EPSILON);
  ASSERT_EQUALS_EPSILON(4.1415926535897932384626, Parser::doubleValue("Pi+1"), EPSILON);
  ASSERT_EQUALS_EPSILON(6.0, Parser::doubleValue("1+5*(3+2)/5"), EPSILON);
}

//===========================================================================
// test_date
//===========================================================================
void ccruncher_test::ParserTest::test_date()
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
// test_bool
//===========================================================================
void ccruncher_test::ParserTest::test_bool()
{
  ASSERT_EQUALS(true, Parser::boolValue("true"));
  ASSERT_EQUALS(false, Parser::boolValue("false"));

  ASSERT_THROW(Parser::boolValue("true "));
  ASSERT_THROW(Parser::boolValue(" true"));
  ASSERT_THROW(Parser::boolValue(" true "));
  ASSERT_THROW(Parser::boolValue("false "));
  ASSERT_THROW(Parser::boolValue(" false"));
  ASSERT_THROW(Parser::boolValue(" false "));
  ASSERT_THROW(Parser::boolValue("TRUE"));
  ASSERT_THROW(Parser::boolValue("FALSE"));
  ASSERT_THROW(Parser::boolValue("True"));
  ASSERT_THROW(Parser::boolValue("False"));
}

