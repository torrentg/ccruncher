
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
// DateTest.cpp - DateTest code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added tests related to getDayOfWeek()
//
// 2007/08/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . added new tests
//
//===========================================================================

#include <iostream>
#include <string>
#include "utils/Date.hpp"
#include "utils/DateTest.hpp"
#include "utils/Exception.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::DateTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::DateTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test_constructors
//===========================================================================
void ccruncher_test::DateTest::test_constructors(void)
{
  Date now = Date();
  Date date1 = Date(25,7,2001);
  Date date2 = Date("25/07/2001");
  Date date3 = Date(20010725);

  ASSERT(date1 == date2);
  ASSERT(date2 == date3);
  ASSERT_THROW(Date("30/02/2003"));
}

//===========================================================================
// test_valid
//===========================================================================
void ccruncher_test::DateTest::test_valid(void)
{
  ASSERT(Date::valid(25,12,2004));
  ASSERT(!Date::valid(25,13,2004));
  ASSERT(!Date::valid(0,11,2004));
  ASSERT(!Date::valid(30,2,2004));
}

//===========================================================================
// test_gets
//===========================================================================
void ccruncher_test::DateTest::test_gets(void)
{
  Date date1 = Date(25,12,2005);

  ASSERT_EQUALS(25, date1.getDay());
  ASSERT_EQUALS(12, date1.getMonth());
  ASSERT_EQUALS(2005, date1.getYear());
  ASSERT_EQUALS(359, date1.getDayOfYear());

  Date date2 = Date(25,12,2008);
  ASSERT_EQUALS(25, date2.getDay());
  ASSERT_EQUALS(12, date2.getMonth());
  ASSERT_EQUALS(2008, date2.getYear());
  ASSERT_EQUALS(360, date2.getDayOfYear());
}

//===========================================================================
// test_gets
//===========================================================================
void ccruncher_test::DateTest::test_sets(void)
{
  Date date1;

  date1 = Date("24/12/2004");
  ASSERT_NO_THROW(date1.setDay(27));
  ASSERT_THROW(date1.setDay(35));

  date1 = Date("24/12/2004");
  ASSERT_NO_THROW(date1.setMonth(11));
  ASSERT_THROW(date1.setMonth(13));

  date1 = Date("24/12/2004");
  ASSERT_NO_THROW(date1.setYear(2019));
  ASSERT_THROW(date1.setYear(-10));
}

//===========================================================================
// test_rollers
//===========================================================================
void ccruncher_test::DateTest::test_rollers(void)
{
  Date date1 = Date("25/12/2004");
  Date date2;

  ASSERT(Date("25/12/2004") == date1++);
  ASSERT(Date("26/12/2004") == date1);
  ASSERT(Date("27/12/2004") == ++date1);

  ASSERT(Date("27/12/2004") == date1--);
  ASSERT(Date("26/12/2004") == date1);
  ASSERT(Date("25/12/2004") == --date1);

  date1 += 3;
  ASSERT(Date("28/12/2004") == date1);

  date1 -= 3;
  ASSERT(Date("25/12/2004") == date1);

  date2 = nextDate(date1);
  ASSERT(Date("26/12/2004") == date2);

  date2 = previousDate(date1);
  ASSERT(Date("24/12/2004") == date2);

  date2 = nextMonth(date1);
  ASSERT(Date("25/1/2005") == date2);
  ASSERT(Date("28/2/2005") == nextMonth(Date("30/1/2005")));

  date2 = addMonths(date1, 3);
  ASSERT(Date("25/3/2005") == date2);
}

//===========================================================================
// test_misc
//===========================================================================
void ccruncher_test::DateTest::test_misc(void)
{
  Date date1 = Date(1,2,2005);

  ASSERT("01/02/2005" == date1.toString());
  ASSERT(20050201 == date1.longDate());

  ASSERT_EQUALS(false, date1.isLeapYear());
  ASSERT_EQUALS(false, Date::isLeapYear(2005));
  ASSERT_EQUALS(true, Date::isLeapYear(2004));

  ASSERT_EQUALS(365, date1.numDaysInYear());
  ASSERT_EQUALS(365, Date::numDaysInYear(2005));
  ASSERT_EQUALS(366, Date::numDaysInYear(2004));

  ASSERT_EQUALS(28, date1.numDaysInMonth());
  ASSERT_EQUALS(28, Date::numDaysInMonth(2, 2005));
  ASSERT_EQUALS(29, Date::numDaysInMonth(2, 2004));
}

//===========================================================================
// test_misc
//===========================================================================
void ccruncher_test::DateTest::test_comparators(void)
{
  Date date1 = Date(23,11,2003);
  Date date2 = Date(25,12,2004);
  Date datex = Date(23,11,2003);

  ASSERT(date1 < date2);
  ASSERT(!(date1 < datex));

  ASSERT(date1 <= date2);
  ASSERT(date1 <= datex);

  ASSERT(!(date1 == date2));
  ASSERT(date1 == datex);

  ASSERT(date1 != date2);
  ASSERT(!(date1 != datex));

  ASSERT(!(date1 > date2));
  ASSERT(!(date1 > datex));

  ASSERT(!(date1 >= date2));
  ASSERT(date1 >= datex);

  ASSERT(date1 == ccruncher::min(date1, date2));
  ASSERT(date2 == ccruncher::max(date1, date2));
  ASSERT(date1 == ccruncher::min(date1, datex));
  ASSERT(date1 == ccruncher::max(date1, datex));
}

//===========================================================================
// test_dayofweek
//===========================================================================
void ccruncher_test::DateTest::test_dayofweek(void)
{
  Date date0 = Date(15, 5,1994);  //sunday    (0)
  Date date1 = Date( 7, 4,2008);  //monday    (1)
  Date date2 = Date(18, 2,2014);  //tuesday   (2)
  Date date3 = Date(20, 6,1984);  //wednesday (3)
  Date date4 = Date( 8,12,2005);  //thursday  (4)
  Date date5 = Date(17, 9,2010);  //friday    (5)
  Date date6 = Date( 4,12,1971);  //saturday  (6)

  ASSERT(date0.getDayOfWeek() == 0);
  ASSERT(date1.getDayOfWeek() == 1);
  ASSERT(date2.getDayOfWeek() == 2);
  ASSERT(date3.getDayOfWeek() == 3);
  ASSERT(date4.getDayOfWeek() == 4);
  ASSERT(date5.getDayOfWeek() == 5);
  ASSERT(date6.getDayOfWeek() == 6);
}
