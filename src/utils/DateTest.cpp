
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include <string>
#include "utils/Date.hpp"
#include "utils/DateTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

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
  Date invalid_date = Date();
  Date date1 = Date(25,7,2001);
  Date date2 = Date("25/07/2001");
  Date date3 = Date(date2);
  Date now = Date(time(NULL));

  ASSERT(nad[0] == *((long*)(&invalid_date)));
  ASSERT(date1 == date2);
  ASSERT(date2 == date3);
  ASSERT(date3 == date2);
  ASSERT(Date("01/01/1900") < now);
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

  date2 = date1 + 1;
  ASSERT(Date("26/12/2004") == date2);

  date2 = date1 - 1;
  ASSERT(Date("24/12/2004") == date2);

  date2 = addMonths(date1, 1);
  ASSERT(Date("25/1/2005") == date2);
  ASSERT(Date("28/2/2005") == addMonths(Date("30/1/2005"), 1));

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

  ASSERT_EQUALS(false, date1.isLeapYear());
  ASSERT_EQUALS(false, Date::isLeapYear(2005));
  ASSERT_EQUALS(true, Date::isLeapYear(2004));

  ASSERT_EQUALS(365, Date::numDaysInYear(2005));
  ASSERT_EQUALS(366, Date::numDaysInYear(2004));

  ASSERT_EQUALS(28, Date::numDaysInMonth(2, 2005));
  ASSERT_EQUALS(29, Date::numDaysInMonth(2, 2004));
  
  ASSERT_THROW(Date(0,0,0));
  ASSERT(NAD == Date(NAD));
}

//===========================================================================
// test_comparators
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

//===========================================================================
// test_distances
//===========================================================================
void ccruncher_test::DateTest::test_distances(void)
{
  Date date0 = Date( 1, 1,2010);
  Date date1 = Date( 2, 1,2010);
  Date date2 = Date( 1, 2,2010);
  Date date3 = Date(28, 2,2010);
  Date date4 = Date(31,12,2010);
  Date date5 = Date( 1, 1,2011);
  Date date6 = Date(15, 1,2010);
  Date date7 = Date(2 , 1,2012);

  ASSERT_EQUALS_EPSILON(      1.0/30.0, date0.getMonthsTo(date1), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0           , date0.getMonthsTo(date2), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0 +27.0/30.0, date0.getMonthsTo(date3), EPSILON);
  ASSERT_EQUALS_EPSILON(11.0+30.0/30.0, date0.getMonthsTo(date4), EPSILON);
  ASSERT_EQUALS_EPSILON(12.0          , date0.getMonthsTo(date5), EPSILON);
  ASSERT_EQUALS_EPSILON(     14.0/30.0, date0.getMonthsTo(date6), EPSILON);
  ASSERT_EQUALS_EPSILON(24.0+ 1.0/30.0, date0.getMonthsTo(date7), EPSILON);
}

//===========================================================================
// test_interval_increments
//===========================================================================
void ccruncher_test::DateTest::test_intervals(void)
{
  ASSERT(ccruncher::isInterval("-450D"));
  ASSERT(ccruncher::isInterval("45D"));
  ASSERT(ccruncher::isInterval("-5Y"));
  ASSERT(ccruncher::isInterval("1974D"));
  ASSERT(ccruncher::isInterval("12000M"));
  ASSERT(ccruncher::isInterval("200000Y"));

  ASSERT(!ccruncher::isInterval("+67d"));
  ASSERT(!ccruncher::isInterval("Y200000Y"));
  ASSERT(!ccruncher::isInterval("200000"));
  ASSERT(!ccruncher::isInterval("2+00000Y"));
 
  Date date0 = Date(31,12,1970);
  date0.addIncrement("+76D");
  ASSERT(Date("17/03/1971") == date0);
  date0.addIncrement("10M");
  ASSERT(Date("17/01/1972") == date0);
  date0.addIncrement("40Y");
  ASSERT(Date("17/01/2012") == date0);
  date0.addIncrement("2M");
  ASSERT(Date("17/03/2012") == date0);
  date0.addIncrement("-17D");
  ASSERT(Date("29/02/2012") == date0);
  date0.addIncrement("1Y");
  ASSERT(Date("28/02/2013") == date0);
}


