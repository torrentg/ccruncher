
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
// InterestTest.cpp - InterestTest code
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
#include "Interest.hpp"
#include "InterestTest.hpp"
#include "utils/XMLUtils.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00001

//===========================================================================
// setUp
//===========================================================================
void InterestTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void InterestTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void InterestTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <interest name='discount' date='18/02/2003'>\n\
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
  XMLUtils::initialize();
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // correlation matrix creation
  Interest iobj;
  ASSERT_NO_THROW(iobj = Interest(*(doc->getDocumentElement())));

  ASSERT("discount" == iobj.getName());
  ASSERT(Date("18/02/2003") == iobj.getFecha());

  Date date0 = Date("18/02/2003");

  double vupdate[] = {
    1.00000, 1.00278, 1.00651, 1.01073, 1.01464, 1.01840,
    1.02232, 1.02643, 1.03050, 1.03478, 1.03902, 1.04348,
    1.04803, 1.05203, 1.05634, 1.06054, 1.06491, 1.06917,
    1.07361, 1.07808, 1.08243, 1.08697, 1.09139, 1.09599,
    1.10062};

  double vactual[] = {
    1.000000, 0.997226, 0.993531, 0.989384, 0.985574, 0.981937,
    0.978166, 0.974250, 0.970407, 0.966386, 0.962448, 0.958330,
    0.954167, 0.950539, 0.946663, 0.942916, 0.939046, 0.935303,
    0.931439, 0.927579, 0.923845, 0.919991, 0.916265, 0.912417,
    0.908578};

  for (int i=0;i<25;i++)
  {
    Date aux = addMonths(date0, i);
    double val1 = iobj.getUpdateCoef(date0, aux);
    double val2 = iobj.getActualCoef(date0, aux);
    ASSERT_DOUBLES_EQUAL(vupdate[i], val1, EPSILON);
    ASSERT_DOUBLES_EQUAL(vactual[i], val2, EPSILON);
  }

  delete wis;
  delete parser;
  XMLUtils::terminate();
}
