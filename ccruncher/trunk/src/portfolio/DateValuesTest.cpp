
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
// DateValuesTest.hpp - DateValuesTest code
// --------------------------------------------------------------------------
//
// 2005/03/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <iostream>
#include "DateValuesTest.hpp"
#include "DateValues.hpp"
#include "utils/XMLUtils.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;

// --------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void DateValuesTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void DateValuesTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// constructors tests
//===========================================================================
void DateValuesTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <values at='01/01/2002' cashflow='10.0' exposure='1500.0' recovery='2000.0' />";

  // creating xml
  XMLUtils::initialize();
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // default constructor
  DateValues *p1 = NULL;
  ASSERT_NO_THROW(p1 = new DateValues());
  if (p1 != NULL)
  {
    ASSERT(p1->date == Date(1,1,1));
    ASSERT(isnan(p1->cashflow));
    ASSERT(isnan(p1->exposure));
    ASSERT(isnan(p1->recovery));
  }

  // values constructor
  DateValues *p2 = NULL;
  ASSERT_NO_THROW(p2 = new DateValues(Date(1,2,2006),1.0, 2.0, 3.0));
  if (p2 != NULL)
  {
    ASSERT(p2->date == Date(1,2,2006));
    ASSERT_DOUBLES_EQUAL(p2->cashflow, 1.0, EPSILON);
    ASSERT_DOUBLES_EQUAL(p2->exposure, 2.0, EPSILON);
    ASSERT_DOUBLES_EQUAL(p2->recovery, 3.0, EPSILON);
  }
    
  // DOMNode constructor
  DateValues *p3 = NULL;
  ASSERT_NO_THROW(p3 = new DateValues(*(doc->getDocumentElement())));
  if (p3 != NULL)
  {
    ASSERT(p3->date == Date(1,1,2002));
    ASSERT_DOUBLES_EQUAL(p3->cashflow, 10.0, EPSILON);
    ASSERT_DOUBLES_EQUAL(p3->exposure, 1500.0, EPSILON);
    ASSERT_DOUBLES_EQUAL(p3->recovery, 2000.0, EPSILON);
  }

  // exit function
  if (p1 != NULL) delete p1;
  if (p2 != NULL) delete p2;
  if (p3 != NULL) delete p3;
  delete parser;
  XMLUtils::terminate();
}

//===========================================================================
// constructors tests (testing crash due to error in XML)
//===========================================================================
void DateValuesTest::test2()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <values at='01/01/2002' cashfloq='10.0' exposure='1500.0' recovery='2000.0' />";

  // creating xml
  XMLUtils::initialize();
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // DOMNode constructor
  DateValues *p1 = NULL;
  ASSERT_THROW(p1 = new DateValues(*(doc->getDocumentElement())));

  // exit function
  if (p1 != NULL) delete p1;
  delete parser;
  XMLUtils::terminate();
}

//===========================================================================
// comparators tests
//===========================================================================
void DateValuesTest::test3()
{
  DateValues val1(Date(10,10,2001), +1.0, +2.0, +3.0);
  DateValues val2(Date(01,01,2006), -1.0, -2.0, -3.0);
  
  // checking that sorts by date
  ASSERT(val1 < val2);
}
