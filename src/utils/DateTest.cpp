
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#define EPSILON 1E-14

using namespace ccruncher;

//===========================================================================
// test_nad
//===========================================================================
void ccruncher_test::DateTest::test_nad()
{
  Date date0 = NAD;
  long *lptr = (long*)(&date0);
  ASSERT(*lptr == 1721026L);
  date0 = Date();
  ASSERT(*lptr == 1721026L);
  date0 = Date(1,1,0);
  date0 -= 1 + Date::numDaysInMonth(0,0);
  ASSERT(*lptr == 1721026L);
}

//===========================================================================
// test_constructors
//===========================================================================
void ccruncher_test::DateTest::test_constructors()
{
  Date date1 = Date(25,7,2001);
  Date date2 = Date("25/07/2001");
  Date date3 = Date(date2);
  Date date4 = Date(1,1,2012);
  Date now = Date(time(nullptr));

  ASSERT(date1 == date2);
  ASSERT(date2 == date3);
  ASSERT(date3 == date2);
  ASSERT(date4 == Date("1/1/2012"));
  ASSERT(Date("01/01/1900") < now);
  ASSERT_THROW(Date("30/02/2003"));
}

//===========================================================================
// test_valid
//===========================================================================
void ccruncher_test::DateTest::test_valid()
{
  ASSERT(Date::valid(25,12,2004));
  ASSERT(!Date::valid(25,13,2004));
  ASSERT(!Date::valid(0,11,2004));
  ASSERT(!Date::valid(30,2,2004));
}

//===========================================================================
// test_gets
//===========================================================================
void ccruncher_test::DateTest::test_gets()
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
void ccruncher_test::DateTest::test_rollers()
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

  ASSERT(Date("23/01/2004") == add(Date("24/01/2004"), -1, 'D'))
  ASSERT(Date("03/02/2004") == add(Date("24/01/2004"), +10, 'D'))
  ASSERT(Date("25/01/2005") == add(Date("25/12/2004"), 1, 'M'));
  ASSERT(Date("28/02/2005") == add(Date("30/01/2005"), 1, 'M'));
  ASSERT(Date("25/03/2005") == add(Date("25/12/2004"), 3, 'M'));
  ASSERT(Date("25/12/2005") == add(Date("25/12/2004"), 1, 'Y'));
  ASSERT(Date("28/02/2005") == add(Date("29/02/2004"), 1, 'Y'));
  ASSERT(Date("25/12/2014") == add(Date("25/12/2004"), 10, 'Y'));

  ASSERT_THROW(add(Date("25/12/2004"), 10, 'd'));
  ASSERT_THROW(add(Date("25/12/2004"), 10, 'm'));
  ASSERT_THROW(add(Date("25/12/2004"), 10, 'y'));
  ASSERT_THROW(add(Date("25/12/2004"), 10, 'Z'));
}

//===========================================================================
// test_misc
//===========================================================================
void ccruncher_test::DateTest::test_misc()
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
void ccruncher_test::DateTest::test_comparators()
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
void ccruncher_test::DateTest::test_dayofweek()
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
void ccruncher_test::DateTest::test_distances()
{
  Date date0 = Date( 1, 1,2010);
  Date date1 = Date( 2, 1,2010);
  Date date2 = Date( 1, 2,2010);
  Date date3 = Date(28, 2,2010);
  Date date4 = Date(31,12,2010);
  Date date5 = Date( 1, 1,2011);
  Date date6 = Date(15, 1,2010);
  Date date7 = Date(2 , 1,2012);

  ASSERT_EQUALS_EPSILON(          0.0, diff(date0, date0, 'D'), EPSILON);
  ASSERT_EQUALS_EPSILON(date1 - date0, diff(date0, date1, 'D'), EPSILON);
  ASSERT_EQUALS_EPSILON(date2 - date0, diff(date0, date2, 'D'), EPSILON);
  ASSERT_EQUALS_EPSILON(date3 - date0, diff(date0, date3, 'D'), EPSILON);
  ASSERT_EQUALS_EPSILON(date4 - date0, diff(date0, date4, 'D'), EPSILON);
  ASSERT_EQUALS_EPSILON(date5 - date0, diff(date0, date5, 'D'), EPSILON);
  ASSERT_EQUALS_EPSILON(date6 - date0, diff(date0, date6, 'D'), EPSILON);
  ASSERT_EQUALS_EPSILON(date7 - date0, diff(date0, date7, 'D'), EPSILON);

  ASSERT_EQUALS_EPSILON(           0.0, diff(date0, date0, 'M'), EPSILON);
  ASSERT_EQUALS_EPSILON(      1.0/30.0, diff(date0, date1, 'M'), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0           , diff(date0, date2, 'M'), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0 +27.0/30.0, diff(date0, date3, 'M'), EPSILON);
  ASSERT_EQUALS_EPSILON(11.0+30.0/30.0, diff(date0, date4, 'M'), EPSILON);
  ASSERT_EQUALS_EPSILON(12.0          , diff(date0, date5, 'M'), EPSILON);
  ASSERT_EQUALS_EPSILON(     14.0/30.0, diff(date0, date6, 'M'), EPSILON);
  ASSERT_EQUALS_EPSILON(24.0+ 1.0/30.0, diff(date0, date7, 'M'), EPSILON);

  Date date10 = Date(1, 1, 2011);
  Date date11 = Date(1, 1, 2012);
  Date date12 = Date(1, 1, 2013);
  Date date13 = Date(31,12, 2011);
  Date date14 = Date(2, 1, 2010);
  Date date15 = Date(1, 2, 2010);
  Date date16 = Date(2, 2, 2010);
  Date date17 = Date(2, 2, 2011);

  ASSERT_EQUALS_EPSILON(          1.0, diff(date0, date10, 'Y'), EPSILON);
  ASSERT_EQUALS_EPSILON(          2.0, diff(date0, date11, 'Y'), EPSILON);
  ASSERT_EQUALS_EPSILON(          3.0, diff(date0, date12, 'Y'), EPSILON);
  ASSERT_EQUALS_EPSILON(          2.0, diff(date0, date13, 'Y'), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0/(12.0*30), diff(date0, date14, 'Y'), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0/12.0     , diff(date0, date15, 'Y'), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0/12.0+1.0/(12.0*30)  , diff(date0, date16, 'Y'), EPSILON);
  ASSERT_EQUALS_EPSILON(1+1.0/12.0+1.0/(12.0*30), diff(date0, date17, 'Y'), EPSILON);

  ASSERT_THROW(diff(date0, date1, 'd'));
  ASSERT_THROW(diff(date0, date1, 'm'));
  ASSERT_THROW(diff(date0, date1, 'y'));
  ASSERT_THROW(diff(date0, date1, 'Z'));

  ASSERT_EQUALS(Date("1/1/2012")-Date("1/1/2011"), 365L);
}

//===========================================================================
// test_interval_increments
//===========================================================================
void ccruncher_test::DateTest::test_intervals()
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
  date0.add("+76D");
  ASSERT(Date("17/03/1971") == date0);
  date0.add("10M");
  ASSERT(Date("17/01/1972") == date0);
  date0.add("40Y");
  ASSERT(Date("17/01/2012") == date0);
  date0.add("2M");
  ASSERT(Date("17/03/2012") == date0);
  date0.add("-17D");
  ASSERT(Date("29/02/2012") == date0);
  date0.add("1Y");
  ASSERT(Date("28/02/2013") == date0);
}


