
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
// RatingsTest.cpp - RatingsTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include <iostream>
#include "Ratings.hpp"
#include "RatingsTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// setUp
//===========================================================================
void RatingsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void RatingsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void RatingsTest::test1()
{
  // note that ratings are unordered
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <ratings>\n\
      <rating name='A' order='1' desc='muy bueno'/>\n\
      <rating name='B' order='2' desc='bueno'/>\n\
      <rating name='D' order='4' desc='malo'/>\n\
      <rating name='C' order='3' desc='regular'/>\n\
      <rating name='E' order='5' desc='fallido'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ratings));
  vector<Rating> &list = *(ratings.getRatings());

  ASSERT(5 == list.size());

  ASSERT_EQUALS(1, list[0].order);
  ASSERT_EQUALS(2, list[1].order);
  ASSERT_EQUALS(3, list[2].order);
  ASSERT_EQUALS(4, list[3].order);
  ASSERT_EQUALS(5, list[4].order);

  ASSERT("A" == list[0].name);
  ASSERT("B" == list[1].name);
  ASSERT("C" == list[2].name);
  ASSERT("D" == list[3].name);
  ASSERT("E" == list[4].name);

  ASSERT("muy bueno" == list[0].desc);
  ASSERT("bueno" == list[1].desc);
  ASSERT("regular" == list[2].desc);
  ASSERT("malo" == list[3].desc);
  ASSERT("fallido" == list[4].desc);

  ASSERT(list[0] < list[1]);
  ASSERT(list[1] < list[2]);
  ASSERT(list[2] < list[3]);
  ASSERT(list[3] < list[4]);
}

//===========================================================================
// test2
//===========================================================================
void RatingsTest::test2()
{
  // note that ratings are incompleted (order 1 not exist)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <ratings>\n\
      <rating name='A' order='2' desc='muy bueno'/>\n\
      <rating name='B' order='3' desc='bueno'/>\n\
      <rating name='D' order='4' desc='malo'/>\n\
      <rating name='C' order='5' desc='regular'/>\n\
      <rating name='E' order='6' desc='fallido'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &ratings));
}

//===========================================================================
// test3
//===========================================================================
void RatingsTest::test3()
{
  // note that ratings are incorrect (A is a name repeated)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <ratings>\n\
      <rating name='A' order='1' desc='muy bueno'/>\n\
      <rating name='A' order='2' desc='bueno'/>\n\
      <rating name='D' order='3' desc='malo'/>\n\
      <rating name='C' order='4' desc='regular'/>\n\
      <rating name='E' order='5' desc='fallido'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &ratings));
}

//===========================================================================
// test4
//===========================================================================
void RatingsTest::test4()
{
  // note that ratings are incorrect (descriptions repeated)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <ratings>\n\
      <rating name='A' order='1' desc='muy bueno'/>\n\
      <rating name='B' order='2' desc='muy bueno'/>\n\
      <rating name='D' order='3' desc='malo'/>\n\
      <rating name='C' order='4' desc='regular'/>\n\
      <rating name='E' order='5' desc='fallido'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &ratings));
}

//===========================================================================
// test5
//===========================================================================
void RatingsTest::test5()
{
  // note that xml are not valid (ratong tag)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <ratings>\n\
      <ratong name='A' order='1' desc='muy bueno'/>\n\
      <rating name='B' order='2' desc='bueno'/>\n\
      <rating name='D' order='3' desc='malo'/>\n\
      <rating name='C' order='4' desc='regular'/>\n\
      <rating name='E' order='5' desc='fallido'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &ratings));
}
