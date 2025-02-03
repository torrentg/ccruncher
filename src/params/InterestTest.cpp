
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

#include <iostream>
#include <iomanip>
#include "params/Interest.hpp"
#include "params/InterestTest.hpp"
#include "utils/Date.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 0.00001

//===========================================================================
// Rates list
//===========================================================================
vector<Interest::Rate> ccruncher_test::InterestTest::getRates() const
{
  vector<Interest::Rate> rates;
  Date date0 = Date("18/02/2003");

  rates.push_back(Interest::Rate(add(date0, 0, 'M'), 0.0399));
  rates.push_back(Interest::Rate(add(date0, 1, 'M'), 0.04));
  rates.push_back(Interest::Rate(add(date0, 2, 'M'), 0.041));
  rates.push_back(Interest::Rate(add(date0, 3, 'M'), 0.045));
  rates.push_back(Interest::Rate(add(date0, 6, 'M'), 0.0455));
  rates.push_back(Interest::Rate(add(date0, 1, 'Y'), 0.048));
  rates.push_back(Interest::Rate(add(date0, 2, 'Y'), 0.049));
  rates.push_back(Interest::Rate(add(date0, 5, 'Y'), 0.05));
  rates.push_back(Interest::Rate(add(date0, 10, 'Y'), 0.052));

  return rates;
}

//===========================================================================
// simple interest
//===========================================================================
void ccruncher_test::InterestTest::test1()
{
  double vactual[] = {
    1.000000, 0.996943, 0.993421, 0.989154, 0.985377, 0.981724,
    0.977950, 0.974038, 0.970219, 0.966237, 0.962352, 0.958306,
    0.954228, 0.950694, 0.946931, 0.943306, 0.939577, 0.935984,
    0.932288, 0.928609, 0.925064, 0.921417, 0.917903, 0.914289,
    0.910691 };

  Date date0 = Date("18/02/2003");
  Interest interest(date0, Interest::InterestType::Simple, Interest::SplineType::Linear);
  vector<Interest::Rate> rates = getRates();

  // inserting unordered rates
  for(size_t i=0; i<rates.size(); i=i+2) {
    ASSERT_NO_THROW(interest.insertRate(rates[i]));
  }
  for(size_t i=1; i<rates.size(); i=i+2) {
    ASSERT_NO_THROW(interest.insertRate(rates[i]));
  }

  for(int i=0; i<25; i++) {
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
    1.000000, 0.996998, 0.993530, 0.989332, 0.985590, 0.981958,
    0.978192, 0.974277, 0.970440, 0.966426, 0.962495, 0.958386,
    0.954229, 0.950606, 0.946736, 0.942993, 0.939129, 0.935392,
    0.931533, 0.927678, 0.923950, 0.920101, 0.91638, 0.9125380,
    0.908700 };

  Date date0 = Date("18/02/2003");
  Interest interest(date0, Interest::InterestType::Compound);
  interest.insertRates(getRates());

  for(int i=0; i<25; i++) {
    Date aux = add(date0, i, 'M');
    double val = interest.getFactor(aux);
    ASSERT_EQUALS_EPSILON(vactual[i], val, EPSILON);
  }
}

//===========================================================================
// continuous interest
//===========================================================================
void ccruncher_test::InterestTest::test3()
{
  double vactual[] = {
    1.000000, 0.996938, 0.993399, 0.989095, 0.985270, 0.981556,
    0.977705, 0.973698, 0.969771, 0.965661, 0.961635, 0.957425,
    0.953165, 0.949458, 0.945499, 0.941669, 0.937716, 0.933893,
    0.929945, 0.926001, 0.922188, 0.91825, 0.914444, 0.9105140,
    0.906588 };

  Date date0 = Date("18/02/2003");
  Interest interest(date0, Interest::InterestType::Continuous);
  interest.insertRates(getRates());

  for(int i=0; i<25; i++) {
    Date aux = add(date0, i, 'M');
    double val = interest.getFactor(aux);
    ASSERT_EQUALS_EPSILON(vactual[i], val, EPSILON);
  }
}

//===========================================================================
// cubic spline option test
// obs: not defined at t=0
//===========================================================================
void ccruncher_test::InterestTest::test4()
{
  Date date0 = Date("18/02/2003");
  Interest interest(date0, Interest::InterestType::Simple, Interest::SplineType::Cubic);
  interest.insertRate(add(date0, 1, 'M'), 0.04);
  interest.insertRate(add(date0, 6, 'M'), 0.045);
  interest.insertRate(add(date0, 2, 'Y'), 0.05);
  interest.insertRate(add(date0, 10, 'Y'), 0.052);

  /*
  for(int i=0; i<365*10.5; i++) {
    Date d = date0 + i;
    cout << d.toString() << "\t" << i << "\t" << interest.getRate(d) << "\t" << interest.getFactor(d) << endl;
  }
  */

  ASSERT_EQUALS_EPSILON(0.0, interest.getRate(date0), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0390481, interest.getRate(date0+1), EPSILON);
  ASSERT_EQUALS_EPSILON(0.04, interest.getRate(add(date0, 1, 'M')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.045, interest.getRate(add(date0, 6, 'M')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.05, interest.getRate(add(date0, 2, 'Y')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.052, interest.getRate(add(date0, 10, 'Y')), EPSILON);
}

//===========================================================================
// static methods
//===========================================================================
void ccruncher_test::InterestTest::test5()
{
  // testing getInterestType method
  ASSERT(Interest::getInterestType("simple") == Interest::InterestType::Simple);
  ASSERT(Interest::getInterestType("compound") == Interest::InterestType::Compound);
  ASSERT(Interest::getInterestType("continuous") == Interest::InterestType::Continuous);
  ASSERT_THROW(Interest::getInterestType("XXX"));
  ASSERT_THROW(Interest::getInterestType("Simple"));

  // testing getSplineType method
  ASSERT(Interest::getSplineType("linear") == Interest::SplineType::Linear);
  ASSERT(Interest::getSplineType("cubic") == Interest::SplineType::Cubic);
  ASSERT_THROW(Interest::getSplineType("XXX"));
  ASSERT_THROW(Interest::getSplineType("CuBic"));
}

//===========================================================================
// twisted cases
//===========================================================================
void ccruncher_test::InterestTest::test6()
{
  Date date0("08/10/2016");
  Interest interest(date0);

  // non-valid date
  ASSERT_THROW(interest.insertRate(NAD, 0.0));

  // rate out-of-range
  ASSERT_THROW(interest.insertRate(date0+1, -1.0));
  ASSERT_THROW(interest.insertRate(date0+1, +2.0));

  // rate date before interest date
  ASSERT_THROW(interest.insertRate(date0-1, 0.0));

  // repeated rate date
  interest.insertRate(date0+1, 0.001);
  ASSERT_THROW(interest.insertRate(date0+1, 0.002));
}

