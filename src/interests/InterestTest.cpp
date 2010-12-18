
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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
#include "interests/Interest.hpp"
#include "interests/InterestTest.hpp"
#include "utils/ExpatParser.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00001

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::InterestTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::InterestTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// simple interest
//===========================================================================
void ccruncher_test::InterestTest::test1()
{
  double vactual[] = {
    1.000000, 0.996678, 0.993213, 0.988875, 0.985168, 0.981461, 
    0.977756, 0.973914, 0.970037, 0.966125, 0.962181, 0.958205, 
    0.954198, 0.950489, 0.946795, 0.943118, 0.939457, 0.935812, 
    0.932184, 0.928571, 0.924974, 0.921394, 0.917829, 0.914280, 
    0.910747};
  
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='simple'>\n\
        <rate t='0' r='0.0'/>\n\
        <rate t='1' r='0.04'/>\n\
        <rate t='2' r='0.041'/>\n\
        <rate t='3' r='0.045'/>\n\
        <rate t='6' r='0.0455'/>\n\
        <rate t='12' r='0.048'/>\n\
        <rate t='24' r='0.049'/>\n\
        <rate t='60' r='0.05'/>\n\
        <rate t='120' r='0.052'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  // correlation matrix creation
  Interest iobj;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &iobj));

  Date date0 = Date("18/02/2003");

  for (int i=0;i<25;i++)
  {
    Date aux = addMonths(date0, i);
    double val = iobj.getFactor(aux, date0);
    ASSERT_EQUALS_EPSILON(vactual[i], val, EPSILON);
  }
}

//===========================================================================
// compound interest
//===========================================================================
void ccruncher_test::InterestTest::test2()
{
  double vactual[] = {
    1.000000, 0.996737, 0.993325, 0.989056, 0.985382, 0.981696, 
    0.977998, 0.974152, 0.970257, 0.966313, 0.962321, 0.958283, 
    0.954198, 0.950396, 0.946596, 0.942799, 0.939004, 0.935212, 
    0.931424, 0.927638, 0.923856, 0.920077, 0.916301, 0.912529, 
    0.908760};

  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='compound'>\n\
        <rate t='0' r='0.0'/>\n\
        <rate t='1' r='0.04'/>\n\
        <rate t='2' r='0.041'/>\n\
        <rate t='3' r='0.045'/>\n\
        <rate t='6' r='0.0455'/>\n\
        <rate t='12' r='0.048'/>\n\
        <rate t='24' r='0.049'/>\n\
        <rate t='60' r='0.05'/>\n\
        <rate t='120' r='0.052'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  // correlation matrix creation
  Interest iobj;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &iobj));

  Date date0 = Date("18/02/2003");

  for (int i=0;i<25;i++)
  {
    Date aux = addMonths(date0, i);
    double val = iobj.getFactor(aux, date0);
    ASSERT_EQUALS_EPSILON(vactual[i], val, EPSILON);
  }
}

//===========================================================================
// continuous interest
//===========================================================================
void ccruncher_test::InterestTest::test3()
{
  double vactual[] = {
    1.000000, 0.996672, 0.99319, 0.9888130, 0.985057, 0.981288, 
    0.977507, 0.973571, 0.969583, 0.965545, 0.961457, 0.957320, 
    0.953134, 0.949243, 0.945355, 0.941470, 0.937588, 0.933709, 
    0.929833, 0.925961, 0.922091, 0.918225, 0.914363, 0.910504, 
    0.906649};
    
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='continuous'>\n\
        <rate t='0' r='0.0'/>\n\
        <rate t='1' r='0.04'/>\n\
        <rate t='2' r='0.041'/>\n\
        <rate t='3' r='0.045'/>\n\
        <rate t='6' r='0.0455'/>\n\
        <rate t='12' r='0.048'/>\n\
        <rate t='24' r='0.049'/>\n\
        <rate t='60' r='0.05'/>\n\
        <rate t='120' r='0.052'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  // correlation matrix creation
  Interest iobj;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &iobj));

  Date date0 = Date("18/02/2003");

  for (int i=0;i<25;i++)
  {
    Date aux = addMonths(date0, i);
    double val = iobj.getFactor(aux, date0);
    ASSERT_EQUALS_EPSILON(vactual[i], val, EPSILON);
  }
}
