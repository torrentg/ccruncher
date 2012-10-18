
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

#include "params/Ratings.hpp"
#include "params/RatingsTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::RatingsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='very good'/>\n\
      <rating name='B' description='good'/>\n\
      <rating name='D' description='very bad'/>\n\
      <rating name='C' description='bad'/>\n\
      <rating name='E' description='defaulted'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ratings));

  ASSERT(5 == ratings.size());

  ASSERT("A" == ratings.getName(0));
  ASSERT("B" == ratings.getName(1));
  ASSERT("D" == ratings.getName(2));
  ASSERT("C" == ratings.getName(3));
  ASSERT("E" == ratings.getName(4));

  ASSERT(0 == ratings.getIndex("A"));
  ASSERT(1 == ratings.getIndex("B"));
  ASSERT(2 == ratings.getIndex("D"));
  ASSERT(3 == ratings.getIndex("C"));
  ASSERT(4 == ratings.getIndex("E"));

  ASSERT("very good" == ratings.getDescription(0));
  ASSERT("good" == ratings.getDescription(1));
  ASSERT("very bad" == ratings.getDescription(2));
  ASSERT("bad" == ratings.getDescription(3));
  ASSERT("defaulted" == ratings.getDescription(4));
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::RatingsTest::test2()
{
  // note that ratings are incorrect (A is a name repeated)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='very good'/>\n\
      <rating name='A' description='good'/>\n\
      <rating name='D' description='very bad'/>\n\
      <rating name='C' description='bad'/>\n\
      <rating name='E' description='defaulted'/>\n\
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
void ccruncher_test::RatingsTest::test3()
{
  // note that ratings are incorrect (descriptions repeated)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='very good'/>\n\
      <rating name='B' description='very good'/>\n\
      <rating name='D' description='very bad'/>\n\
      <rating name='C' description='bad'/>\n\
      <rating name='E' description='defaulted'/>\n\
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
void ccruncher_test::RatingsTest::test4()
{
  // note that xml are not valid (ratong tag)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <ratong name='A' description='very good'/>\n\
      <rating name='B' description='good'/>\n\
      <rating name='D' description='very bad'/>\n\
      <rating name='C' description='bad'/>\n\
      <rating name='E' description='defaulted'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &ratings));
}
