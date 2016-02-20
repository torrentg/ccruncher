
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

#include "params/Ratings.hpp"
#include "params/RatingsTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

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

  ASSERT("A" == ratings[0].name);
  ASSERT("B" == ratings[1].name);
  ASSERT("D" == ratings[2].name);
  ASSERT("C" == ratings[3].name);
  ASSERT("E" == ratings[4].name);

  ASSERT(0 == ratings.indexOf("A"));
  ASSERT(1 == ratings.indexOf("B"));
  ASSERT(2 == ratings.indexOf("D"));
  ASSERT(3 == ratings.indexOf("C"));
  ASSERT(4 == ratings.indexOf("E"));

  ASSERT("very good" == ratings[0].description);
  ASSERT("good" == ratings[1].description);
  ASSERT("very bad" == ratings[2].description);
  ASSERT("bad" == ratings[3].description);
  ASSERT("defaulted" == ratings[4].description);
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
  // error because only 1 rating (required a minimum of 2 ratings)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='very good'/>\n\
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

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::RatingsTest::test5()
{
// creation from scratch
  Ratings ratings;
  ratings.push_back(Rating("A", "very good"));
  ratings.push_back(Rating("B", "good"));
  ratings.push_back(Rating("C", "bad"));
  ratings.push_back(Rating("D", "very bad"));
  ratings.push_back(Rating("E", "defaulted"));
  ASSERT_EQUALS((size_t)5, ratings.size());
  ASSERT_EQUALS((unsigned char)0, ratings.indexOf("A"));
  ASSERT_EQUALS((unsigned char)1, ratings.indexOf("B"));
  ASSERT_EQUALS((unsigned char)2, ratings.indexOf("C"));
  ASSERT_EQUALS((unsigned char)3, ratings.indexOf("D"));
  ASSERT_EQUALS((unsigned char)4, ratings.indexOf("E"));
  ASSERT_THROW(ratings.indexOf("K"));
  ASSERT(Ratings::isValid(ratings));
}

//===========================================================================
// test6
//===========================================================================
void ccruncher_test::RatingsTest::test6()
{
// creation from scratch
  vector<Rating> ratings;

  ratings.push_back(Rating("A", "very good"));

  // less than 2 ratings
  ASSERT(!Ratings::isValid(ratings));

  ratings.push_back(Rating("A", "good"));
  ratings.push_back(Rating("C", "bad"));
  ratings.push_back(Rating("D", "very bad"));
  ratings.push_back(Rating("E", "defaulted"));

  // repeated rating
  ASSERT(!Ratings::isValid(ratings));
}

