
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#include "params/DefaultProbabilities.hpp"
#include "params/DefaultProbabilitiesTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 2E-5

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::DefaultProbabilitiesTest::getRatings()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='rating1'/>\n\
      <rating name='E' description='default'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;
  Ratings ret;
  xmlparser.parse(xmlcontent, &ret);

  return ret;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <dprobs>\n\
      <dprob rating='A' t='0Y' value='0.00'/>\n\
      <dprob rating='A' t='2Y' value='0.50'/>\n\
      <dprob rating='A' t='3Y' value='0.75'/>\n\
      <dprob rating='A' t='5Y' value='0.90'/>\n\
      <!-- optionally you can add default rating info (value=1 always) -->\n\
    </dprobs>";
  Date date("1/1/2012");
  Date dvalues[] = { Date("1/1/2012"), Date("1/1/2014"), Date("1/1/2015"), Date("1/1/2017") };
  double pvalues0[] = { 0.0, 0.5, 0.75, 0.9 };

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();
  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().date1 = date;

  // dprob function creation
  DefaultProbabilities pd;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &pd));

  // checking methods
  ASSERT_EQUALS(pd.size(), 2ul);
  ASSERT_EQUALS(pd.getIndexDefault(), 1ul);
  ASSERT_EQUALS(pd[0].getInterpolationType(), "cspline");
  ASSERT_EQUALS(pd[1].getInterpolationType(), "none");

  // checking evalue method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd[0].evalue(dvalues[i]-date), pvalues0[i], EPSILON);
    ASSERT_EQUALS_EPSILON(pd[1].evalue(dvalues[i]-date), 1.0, EPSILON)

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double t = (dvalues[i-1]-date) + j*(dvalues[i]-dvalues[i-1])/100.0;
        // monotone
        ASSERT(pd[0].evalue(dvalues[i-1]-date) < pd[0].evalue(t));
        ASSERT(pd[0].evalue(t) < pd[0].evalue(dvalues[i]-date));
        // default = 1
        ASSERT_EQUALS_EPSILON(pd[1].evalue(t), 1.0, EPSILON);
        // inv(eval(t)) = t
        ASSERT_EQUALS_EPSILON(pd[0].inverse(pd[0].evalue(t)), t, EPSILON);
      }
    }
  }

  // checking inverse method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd[0].inverse(pvalues0[i]), dvalues[i]-date, EPSILON);
    ASSERT_EQUALS_EPSILON(pd[1].inverse(pvalues0[i]), 0.0, EPSILON);

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double p = pvalues0[i-1] + j*(pvalues0[i]-pvalues0[i-1])/100.0;
        // monotone
        ASSERT(pd[0].inverse(pvalues0[i-1]) < pd[0].inverse(p));
        ASSERT(pd[0].inverse(p) < pd[0].inverse(pvalues0[i]));
        // default = 0
        ASSERT_EQUALS_EPSILON(pd[1].inverse(p), 0.0, EPSILON);
      }
    }
  }
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test2()
{
  // non valid dprob function (value at t=0 distinct that 0)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <dprobs>\n\
      <dprob rating='A' t='0Y' value='0.02'/>\n\
      <dprob rating='A' t='2Y' value='0.50'/>\n\
      <dprob rating='A' t='3Y' value='0.75'/>\n\
      <dprob rating='A' t='5Y' value='0.90'/>\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd;

  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().date1 = Date("1/1/2012");
  ASSERT_THROW(xmlparser.parse(xmlcontent, &pd));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test3()
{
  // non valid dprob function, non monotone
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <dprobs>\n\
      <dprob rating='A' t='0Y' value='0.00'/>\n\
      <dprob rating='A' t='2Y' value='0.50'/>\n\
      <dprob rating='A' t='3Y' value='0.25'/>\n\
      <dprob rating='A' t='5Y' value='0.50'/>\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd;

  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().date1 = Date("1/1/2012");
  ASSERT_THROW(xmlparser.parse(xmlcontent, &pd));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test4()
{
  // non valid probabilities (values out of range [0,1])
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <dprobs>\n\
      <dprob rating='A' t='0Y' value='0.00'/>\n\
      <dprob rating='A' t='2Y' value='-0.50'/>\n\
      <dprob rating='A' t='3Y' value='-0.75'/>\n\
      <dprob rating='A' t='5Y' value='-0.85'/>\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd;

  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().date1 = Date("1/1/2012");
  ASSERT_THROW(xmlparser.parse(xmlcontent, &pd));
}

//===========================================================================
// test6
// check precision when pd are near 0%
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test6()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <dprobs>\n\
      <dprob rating='A' t='0D'  value='0.000'/>\n\
      <dprob rating='A' t='12M' value='0.001'/>\n\
      <dprob rating='A' t='24M' value='0.005'/>\n\
      <dprob rating='A' t='48M' value='0.03'/>\n\
      <!-- optionally you can add default rating info (value=0 always) -->\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd;

  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().date1 = Date("1/1/2012");
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &pd));

  double pvalues[] = { 0.00005, 0.00010, 0.00100, 0.00200, 0.00300, 0.00400,
                       0.00500, 0.00600, 0.00700, 0.00800, 0.00900, 0.01000,
                       0.02000 };

  double tvalues[] = { 31.1449, 61.3834, 366, 513.284, 605.83, 674.93,
                       731, 778.854, 821.464, 860.461, 896.79, 931.055,
                       1215.9 };

  // checking values
  for(int i=0; i<13; i++)
  {
    ASSERT_EQUALS_EPSILON(pd[0].inverse(pvalues[i]), tvalues[i], 1e-5);
  }
}

//===========================================================================
// test7
// case with dprob[t=0]=dprob[t=1]=dprob[t=2]=0
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test7()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <dprobs>\n\
      <dprob rating='A' t='0M'  value='0.000'/>\n\
      <dprob rating='A' t='1M'  value='0.000'/>\n\
      <dprob rating='A' t='2M'  value='0.000'/>\n\
      <dprob rating='A' t='3M'  value='0.0002'/>\n\
      <dprob rating='A' t='12M' value='0.001'/>\n\
      <dprob rating='A' t='24M' value='0.005'/>\n\
      <dprob rating='A' t='48M' value='0.03'/>\n\
      <!-- optionally you can add default rating info (value=0 always) -->\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  Date date("1/1/2012");
  DefaultProbabilities pd;

  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().date1 = Date("1/1/2012");
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &pd));

  for(int i=0; i<=add(date,2,'M')-date; i++)
  {
    ASSERT_EQUALS(pd[0].evalue(i), 0.0);
  }
  ASSERT(pd[0].evalue((add(date,2,'M')-date)+1) > 0.0);


  ASSERT_EQUALS(pd[0].inverse(0.0), add(date,2,'M')-date);
}

