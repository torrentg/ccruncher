
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
// BondTest.cpp - BondTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//***************************************************************************

#include <iostream>
#include "BondTest.hpp"
#include "Bond.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00001

//***************************************************************************
// setUp
//***************************************************************************
void BondTest::setUp()
{
  // nothing to do
}

//***************************************************************************
// setUp
//***************************************************************************
void BondTest::tearDown()
{
  // nothing to do
}

//***************************************************************************
// test1
//***************************************************************************
void BondTest::test1()
{
  Bond bond = Bond();

  // adquisition date before issue
  ASSERT_NO_THROW(bond.setProperty("issuedate", "1/1/2005"));
  ASSERT_NO_THROW(bond.setProperty("term", "24"));
  ASSERT_NO_THROW(bond.setProperty("nominal", "1000.0"));
  ASSERT_NO_THROW(bond.setProperty("rate", "0.04"));
  ASSERT_NO_THROW(bond.setProperty("ncoupons", "4"));
  ASSERT_NO_THROW(bond.setProperty("adquisitiondate", "1/6/2004"));
  ASSERT_NO_THROW(bond.setProperty("adquisitionprice", "995.00"));

  ASSERT(bond.validate());
  ASSERT_NO_THROW(bond.initialize());
  
  DateValues *array = NULL;
  int size;

  ASSERT_NO_THROW(array = bond.getEvents());
  ASSERT_NO_THROW(size = bond.getSize());
  ASSERT_EQUALS(6, size);

  ASSERT(Date("01/06/2004") == array[0].date);
  ASSERT_DOUBLES_EQUAL(-995.0, array[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[0].exposure, EPSILON);
  
  ASSERT(Date("01/01/2005") == array[1].date);
  ASSERT_DOUBLES_EQUAL(0.0, array[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[1].exposure, EPSILON);

  ASSERT(Date("01/07/2005") == array[2].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[2].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[2].exposure, EPSILON);

  ASSERT(Date("01/01/2006") == array[3].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[3].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[3].exposure, EPSILON);

  ASSERT(Date("01/07/2006") == array[4].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[4].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[4].exposure, EPSILON);

  ASSERT(Date("01/01/2007") == array[5].date);
  ASSERT_DOUBLES_EQUAL(1020.0, array[5].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[5].exposure, EPSILON);

  ASSERT_THROW(bond.setProperty("ncoupons", "5"));
}

//***************************************************************************
// test2
//***************************************************************************
void BondTest::test2()
{
  Bond bond = Bond();

  // adquisition date at issue
  ASSERT_NO_THROW(bond.setProperty("issuedate", "1/1/2005"));
  ASSERT_NO_THROW(bond.setProperty("term", "24"));
  ASSERT_NO_THROW(bond.setProperty("nominal", "1000.0"));
  ASSERT_NO_THROW(bond.setProperty("rate", "0.04"));
  ASSERT_NO_THROW(bond.setProperty("ncoupons", "4"));
  ASSERT_NO_THROW(bond.setProperty("adquisitiondate", "1/1/2005"));
  ASSERT_NO_THROW(bond.setProperty("adquisitionprice", "995.00"));

  ASSERT(bond.validate());
  ASSERT_NO_THROW(bond.initialize());
  
  DateValues *array = NULL;
  int size;

  ASSERT_NO_THROW(array = bond.getEvents());
  ASSERT_NO_THROW(size = bond.getSize());
  ASSERT_EQUALS(5, size);

  ASSERT(Date("01/01/2005") == array[0].date);
  ASSERT_DOUBLES_EQUAL(-995.0, array[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[0].exposure, EPSILON);

  ASSERT(Date("01/07/2005") == array[1].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[1].exposure, EPSILON);

  ASSERT(Date("01/01/2006") == array[2].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[2].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[2].exposure, EPSILON);

  ASSERT(Date("01/07/2006") == array[3].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[3].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[3].exposure, EPSILON);

  ASSERT(Date("01/01/2007") == array[4].date);
  ASSERT_DOUBLES_EQUAL(1020.0, array[4].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[4].exposure, EPSILON);

  ASSERT_THROW(bond.setProperty("ncoupons", "5"));
}

//***************************************************************************
// test3
//***************************************************************************
void BondTest::test3()
{
  Bond bond = Bond();

  // adquisition date after issue
  ASSERT_NO_THROW(bond.setProperty("issuedate", "1/1/2005"));
  ASSERT_NO_THROW(bond.setProperty("term", "24"));
  ASSERT_NO_THROW(bond.setProperty("nominal", "1000.0"));
  ASSERT_NO_THROW(bond.setProperty("rate", "0.04"));
  ASSERT_NO_THROW(bond.setProperty("ncoupons", "4"));
  ASSERT_NO_THROW(bond.setProperty("adquisitiondate", "1/3/2005"));
  ASSERT_NO_THROW(bond.setProperty("adquisitionprice", "995.00"));

  ASSERT(bond.validate());
  ASSERT_NO_THROW(bond.initialize());
  
  DateValues *array = NULL;
  int size;

  ASSERT_NO_THROW(array = bond.getEvents());
  ASSERT_NO_THROW(size = bond.getSize());
  ASSERT_EQUALS(5, size);

  ASSERT(Date("01/03/2005") == array[0].date);
  ASSERT_DOUBLES_EQUAL(-995.0, array[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[0].exposure, EPSILON);

  ASSERT(Date("01/07/2005") == array[1].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[1].exposure, EPSILON);

  ASSERT(Date("01/01/2006") == array[2].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[2].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[2].exposure, EPSILON);

  ASSERT(Date("01/07/2006") == array[3].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[3].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[3].exposure, EPSILON);

  ASSERT(Date("01/01/2007") == array[4].date);
  ASSERT_DOUBLES_EQUAL(1020.0, array[4].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[4].exposure, EPSILON);

  ASSERT_THROW(bond.setProperty("ncoupons", "5"));
}

//***************************************************************************
// test4
//***************************************************************************
void BondTest::test4()
{
  Bond bond = Bond();

  // adquisition date at first coupon date
  ASSERT_NO_THROW(bond.setProperty("issuedate", "1/1/2005"));
  ASSERT_NO_THROW(bond.setProperty("term", "24"));
  ASSERT_NO_THROW(bond.setProperty("nominal", "1000.0"));
  ASSERT_NO_THROW(bond.setProperty("rate", "0.04"));
  ASSERT_NO_THROW(bond.setProperty("ncoupons", "4"));
  ASSERT_NO_THROW(bond.setProperty("adquisitiondate", "1/7/2005"));
  ASSERT_NO_THROW(bond.setProperty("adquisitionprice", "995.00"));

  ASSERT(bond.validate());
  ASSERT_NO_THROW(bond.initialize());
  
  DateValues *array = NULL;
  int size;

  ASSERT_NO_THROW(array = bond.getEvents());
  ASSERT_NO_THROW(size = bond.getSize());
  ASSERT_EQUALS(4, size);

  ASSERT(Date("01/07/2005") == array[0].date);
  ASSERT_DOUBLES_EQUAL(-995.0, array[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[0].exposure, EPSILON);

  ASSERT(Date("01/01/2006") == array[1].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[1].exposure, EPSILON);

  ASSERT(Date("01/07/2006") == array[2].date);
  ASSERT_DOUBLES_EQUAL(20.0, array[2].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[2].exposure, EPSILON);

  ASSERT(Date("01/01/2007") == array[3].date);
  ASSERT_DOUBLES_EQUAL(1020.0, array[3].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[3].exposure, EPSILON);

  ASSERT_THROW(bond.setProperty("ncoupons", "5"));
}

//***************************************************************************
// test5
//***************************************************************************
void BondTest::test5()
{
  Bond bond = Bond();

  // adquisition after finalization
  ASSERT_NO_THROW(bond.setProperty("issuedate", "1/1/2005"));
  ASSERT_NO_THROW(bond.setProperty("term", "24"));
  ASSERT_NO_THROW(bond.setProperty("nominal", "1000.0"));
  ASSERT_NO_THROW(bond.setProperty("rate", "0.04"));
  ASSERT_NO_THROW(bond.setProperty("ncoupons", "4"));
  ASSERT_NO_THROW(bond.setProperty("adquisitiondate", "1/7/2008"));
  ASSERT_NO_THROW(bond.setProperty("adquisitionprice", "995.00"));

  ASSERT(!bond.validate());
}

//***************************************************************************
// test6
//***************************************************************************
void BondTest::test6()
{
  Bond bond = Bond();

  // term modulus ncoupons distinct 0
  ASSERT_NO_THROW(bond.setProperty("issuedate", "1/1/2005"));
  ASSERT_NO_THROW(bond.setProperty("term", "24"));
  ASSERT_NO_THROW(bond.setProperty("nominal", "1000.0"));
  ASSERT_NO_THROW(bond.setProperty("rate", "0.04"));
  ASSERT_NO_THROW(bond.setProperty("ncoupons", "5"));
  ASSERT_NO_THROW(bond.setProperty("adquisitiondate", "1/7/2004"));
  ASSERT_NO_THROW(bond.setProperty("adquisitionprice", "995.00"));

  ASSERT(!bond.validate());
}

//***************************************************************************
// test7
//***************************************************************************
void BondTest::test7()
{
  Bond bond = Bond();

  // a unique coupon
  ASSERT_NO_THROW(bond.setProperty("issuedate", "1/1/2005"));
  ASSERT_NO_THROW(bond.setProperty("term", "24"));
  ASSERT_NO_THROW(bond.setProperty("nominal", "1000.0"));
  ASSERT_NO_THROW(bond.setProperty("rate", "0.04"));
  ASSERT_NO_THROW(bond.setProperty("ncoupons", "1"));
  ASSERT_NO_THROW(bond.setProperty("adquisitiondate", "1/7/2005"));
  ASSERT_NO_THROW(bond.setProperty("adquisitionprice", "995.00"));

  ASSERT(bond.validate());
  ASSERT_NO_THROW(bond.initialize());
  
  DateValues *array = NULL;
  int size;

  ASSERT_NO_THROW(array = bond.getEvents());
  ASSERT_NO_THROW(size = bond.getSize());
  ASSERT_EQUALS(2, size);

  ASSERT(Date("01/07/2005") == array[0].date);
  ASSERT_DOUBLES_EQUAL(-995.0, array[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[0].exposure, EPSILON);

  ASSERT(Date("01/01/2007") == array[1].date);
  ASSERT_DOUBLES_EQUAL(1080.0, array[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, array[1].exposure, EPSILON);

  ASSERT_THROW(bond.setProperty("ncoupons", "5"));
}
