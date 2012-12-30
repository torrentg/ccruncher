
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

#include <iostream>
#include "params/Interest.hpp"
#include "params/InterestTest.hpp"
#include "utils/ExpatParser.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00001

//===========================================================================
// simple interest
//===========================================================================
void ccruncher_test::InterestTest::test1()
{
  double vactual[] = {
    1.000000, 0.996678, 0.993213, 0.988875, 0.985127, 0.981502,
    0.977756, 0.973872, 0.970079, 0.966125, 0.962267, 0.958248,
    0.954198, 0.950671, 0.946916, 0.943299, 0.939577, 0.935991,
    0.932302, 0.928630, 0.925092, 0.921452, 0.917945, 0.914338,
    0.910747 };

  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='simple' spline='linear'>\n\
        <rate t='0M' r='0.0'/>\n\
        <rate t='1M' r='0.04'/>\n\
        <rate t='2M' r='0.041'/>\n\
        <rate t='3M' r='0.045'/>\n\
        <rate t='6M' r='0.0455'/>\n\
        <rate t='1Y' r='0.048'/>\n\
        <rate t='2Y' r='0.049'/>\n\
        <rate t='5Y' r='0.05'/>\n\
        <rate t='10Y' r='0.052'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  Date date0 = Date("18/02/2003");
  Interest interest(date0);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &interest));

  for (int i=0; i<25; i++)
  {
    Date aux = add(date0, i, 'M');
    double val = interest.getFactor(aux);
    ASSERT_EQUALS_EPSILON(vactual[i], val, EPSILON);
  }
}

//===========================================================================
// compound interest
//===========================================================================
void ccruncher_test::InterestTest::test2()
{
  double vactual[] = {
    1.000000, 0.996737, 0.993325, 0.989056, 0.985342, 0.981736,
    0.977998, 0.974110, 0.970299, 0.966313, 0.962409, 0.958327,
    0.954198, 0.950583, 0.946720, 0.942985, 0.939128, 0.935399,
    0.931548, 0.927700, 0.923980, 0.920139, 0.916425, 0.912590,
    0.90876 };

  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='compound'>\n\
        <rate t='0D' r='0.0'/>\n\
        <rate t='1M' r='0.04'/>\n\
        <rate t='2M' r='0.041'/>\n\
        <rate t='3M' r='0.045'/>\n\
        <rate t='6M' r='0.0455'/>\n\
        <rate t='1Y' r='0.048'/>\n\
        <rate t='24M' r='0.049'/>\n\
        <rate t='60M' r='0.05'/>\n\
        <rate t='10Y' r='0.052'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  Date date0 = Date("18/02/2003");
  Interest iobj(date0);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &iobj));

  for (int i=0; i<25; i++)
  {
    Date aux = add(date0, i, 'M');
    double val = iobj.getFactor(aux);
    ASSERT_EQUALS_EPSILON(vactual[i], val, EPSILON);
  }
}

//===========================================================================
// continuous interest
//===========================================================================
void ccruncher_test::InterestTest::test3()
{
  double vactual[] = {
    1.000000, 0.996672, 0.993190, 0.988813, 0.985016, 0.981329,
    0.977507, 0.973528, 0.969627, 0.965545, 0.961546, 0.957365,
    0.953134, 0.949434, 0.945483, 0.941661, 0.937715, 0.933900,
    0.929960, 0.926024, 0.922218, 0.918289, 0.914489, 0.910567,
    0.906649 };

  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='continuous'>\n\
        <rate t='0D' r='0.0'/>\n\
        <rate t='1M' r='0.04'/>\n\
        <rate t='2M' r='0.041'/>\n\
        <rate t='3M' r='0.045'/>\n\
        <rate t='6M' r='0.0455'/>\n\
        <rate t='1Y' r='0.048'/>\n\
        <rate t='2Y' r='0.049'/>\n\
        <rate t='5Y' r='0.05'/>\n\
        <rate t='10Y' r='0.052'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  Date date0 = Date("18/02/2003");
  Interest iobj(date0);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &iobj));


  for (int i=0; i<25; i++)
  {
    Date aux = add(date0, i, 'M');
    double val = iobj.getFactor(aux);
    ASSERT_EQUALS_EPSILON(vactual[i], val, EPSILON);
  }
}

//===========================================================================
// cubic spline option test
// obs: not defined at t=0
//===========================================================================
void ccruncher_test::InterestTest::test4()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='simple' spline='cubic'>\n\
        <rate t='1M' r='4%'/>\n\
        <rate t='6M' r='4.5%'/>\n\
        <rate t='2Y' r='5%'/>\n\
        <rate t='10Y' r='5.2%'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  Date date0 = Date("18/02/2003");
  Interest interest(date0);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &interest));

  /*
  for(int i=0; i<365*10.5; i++) {
    Date d = date0 + i;
    cout << d.toString() << "\t" << i << "\t" << interest.getValue(d) << "\t" << interest.getFactor(d) << endl;
  }
  */

  ASSERT_EQUALS_EPSILON(0.0, interest.getValue(date0), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0390481, interest.getValue(date0+1), EPSILON);
  ASSERT_EQUALS_EPSILON(0.04, interest.getValue(add(date0, 1, 'M')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.045, interest.getValue(add(date0, 6, 'M')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.05, interest.getValue(add(date0, 2, 'Y')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.052, interest.getValue(add(date0, 10, 'Y')), EPSILON);
}

