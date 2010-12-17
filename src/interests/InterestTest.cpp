
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
    1.000000, 0.997179, 0.993421, 0.989208, 0.985361, 0.981703, 
    0.977924, 0.974012, 0.970186, 0.966198, 0.962306, 0.958251, 
    0.954168, 0.950628, 0.946861, 0.943231, 0.939497, 0.935900, 
    0.932198, 0.928514, 0.924964, 0.921313, 0.917795, 0.914175, 
    0.910577};
  
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
    1.000000, 0.997226, 0.993531, 0.989384, 0.985574, 0.981937,
    0.978166, 0.974250, 0.970407, 0.966386, 0.962448, 0.958330,
    0.954167, 0.950539, 0.946663, 0.942916, 0.939046, 0.935303,
    0.931439, 0.927579, 0.923845, 0.919991, 0.916265, 0.912417,
    0.908578};

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
    1.000000, 0.997175, 0.993399, 0.989149, 0.985253, 0.981534, 
    0.977678, 0.973671, 0.969737, 0.965620, 0.961586, 0.957368, 
    0.953102, 0.949390, 0.945424, 0.941590, 0.937631, 0.933802, 
    0.929849, 0.925899, 0.922081, 0.918138, 0.914326, 0.910390, 
    0.906464};
    
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
