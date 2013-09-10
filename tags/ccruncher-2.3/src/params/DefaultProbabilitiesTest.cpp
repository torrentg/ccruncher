
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "params/DefaultProbabilities.hpp"
#include "params/DefaultProbabilitiesTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

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

  // dprob function creation
  DefaultProbabilities pd(ratings, date);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &pd));

  // checking methods
  ASSERT_EQUALS(pd.size(), 2);
  ASSERT_EQUALS(pd.getMaxDate(0), Date("1/1/2017"));
  ASSERT_EQUALS(pd.getIndexDefault(), 1);
  ASSERT_EQUALS(pd.getRatings().size(), 2);
  ASSERT_EQUALS(pd.getInterpolationType(0), "cspline");
  ASSERT_EQUALS(pd.getInterpolationType(1), "none");

  // checking evalue method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd.evalue(0, dvalues[i]-date), pvalues0[i], EPSILON);
    ASSERT_EQUALS_EPSILON(pd.evalue(1, dvalues[i]-date), 1.0, EPSILON)

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double t = (dvalues[i-1]-date) + j*(dvalues[i]-dvalues[i-1])/100.0;
        // monotone
        ASSERT(pd.evalue(0, dvalues[i-1]-date) < pd.evalue(0, t));
        ASSERT(pd.evalue(0, t) < pd.evalue(0, dvalues[i]-date));
        // default = 1
        ASSERT_EQUALS_EPSILON(pd.evalue(1, t), 1.0, EPSILON);
        // inv(eval(t)) = t
        ASSERT_EQUALS_EPSILON(pd.inverse(0, pd.evalue(0, t)), t, EPSILON);
      }
    }
  }

  // checking inverse method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd.inverse(0, pvalues0[i]), dvalues[i]-date, EPSILON);
    ASSERT_EQUALS_EPSILON(pd.inverse(1, pvalues0[i]), 0.0, EPSILON);

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double p = pvalues0[i-1] + j*(pvalues0[i]-pvalues0[i-1])/100.0;
        // monotone
        ASSERT(pd.inverse(0, pvalues0[i-1]) < pd.inverse(0, p));
        ASSERT(pd.inverse(0, p) < pd.inverse(0, pvalues0[i]));
        // default = 0
        ASSERT_EQUALS_EPSILON(pd.inverse(1, p), 0.0, EPSILON);
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
  DefaultProbabilities pd(ratings, Date("1/1/2012"));
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
  DefaultProbabilities pd(ratings, Date("1/1/2012"));
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
  DefaultProbabilities pd(ratings, Date("1/1/2012"));
  ASSERT_THROW(xmlparser.parse(xmlcontent, &pd));
}

//===========================================================================
// test5 (equals than test1, but with diferent constructor)
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test5()
{
  Date date("1/1/2012");
  Date dvalues[] = { Date("1/1/2012"), Date("1/1/2014"), Date("1/1/2015"), Date("1/1/2017") };
  double pvalues0[] = { 0.0, 0.5, 0.75, 0.9 };
  double pvalues1[] = { 1.0, 1.0, 1.00, 1.0 };

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  vector<vector<double> > mvalues(2);
  mvalues[0] = vector<double>(pvalues0, pvalues0+4);
  mvalues[1] = vector<double>(pvalues1, pvalues1+4);
  DefaultProbabilities pd(ratings, Date("1/1/2012"), vector<Date>(dvalues, dvalues+4), mvalues);

  // checking methods
  ASSERT_EQUALS(pd.size(), 2);
  ASSERT_EQUALS(pd.getMaxDate(0), Date("1/1/2017"));
  ASSERT_EQUALS(pd.getIndexDefault(), 1);
  ASSERT_EQUALS(pd.getRatings().size(), 2);
  ASSERT_EQUALS(pd.getInterpolationType(0), "cspline");
  ASSERT_EQUALS(pd.getInterpolationType(1), "none");

  // checking evalue method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd.evalue(0, dvalues[i]-date), pvalues0[i], EPSILON);
    ASSERT_EQUALS_EPSILON(pd.evalue(1, dvalues[i]-date), 1.0, EPSILON)

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double t = (dvalues[i-1]-date) + j*(dvalues[i]-dvalues[i-1])/100.0;
        // monotone
        ASSERT(pd.evalue(0, dvalues[i-1]-date) < pd.evalue(0, t));
        ASSERT(pd.evalue(0, t) < pd.evalue(0, dvalues[i]-date));
        // default = 1
        ASSERT_EQUALS_EPSILON(pd.evalue(1, t), 1.0, EPSILON);
        // inv(eval(t)) = t
        ASSERT_EQUALS_EPSILON(pd.inverse(0, pd.evalue(0, t)), t, EPSILON);
      }
    }
  }

  // checking inverse method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd.inverse(0, pvalues0[i]), dvalues[i]-date, EPSILON);
    ASSERT_EQUALS_EPSILON(pd.inverse(1, pvalues0[i]), 0.0, EPSILON);

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double p = pvalues0[i-1] + j*(pvalues0[i]-pvalues0[i-1])/100.0;
        // monotone
        ASSERT(pd.inverse(0, pvalues0[i-1]) < pd.inverse(0, p));
        ASSERT(pd.inverse(0, p) < pd.inverse(0, pvalues0[i]));
        // default = 0
        ASSERT_EQUALS_EPSILON(pd.inverse(1, p), 0.0, EPSILON);
      }
    }
  }
}

//===========================================================================
// test6 (checks distribution assumptions)
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test6()
{
  double mvalues1[] = {0.0, 0.1};
  double mvalues2[] = {1.0, 1.0};
  Date dates[] = { Date("1/1/2012"), Date("1/1/2013") };
  int ivalues[] = {0, 0};

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  vector<vector<double> > mvalues(2);
  mvalues[0] = vector<double>(mvalues1, mvalues1+2);
  mvalues[1] = vector<double>(mvalues2, mvalues2+2);
  DefaultProbabilities pd(ratings, Date("1/1/2012"), vector<Date>(dates, dates+2), mvalues);

  // creating randomizer
  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);

  // checking values
  for(int i=0;i<20000;i++)
  {
    double u = gsl_ran_flat (rng, 0.0, 1.0);
    if (pd.inverse(0,u) > 12.0) {
      ivalues[0]++;
    }
    else {
      ivalues[1]++;
    }
  }

  // deallocates randomizer
  gsl_rng_free(rng);

  // checking that P(X in [0,0.9]) = 0.9
  ASSERT(ivalues[0] > 17500);  // exact value is 18000 (margin=500)
  // checking that P(X in [0.9,1.0]) = 0.1
  ASSERT(ivalues[1] < 2500);  // exact value is 2000 (margin=500)
}

//===========================================================================
// test7
// check precision when pd are near 0%
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test7()
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
  DefaultProbabilities pd(ratings, Date("1/1/2012"));
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
    ASSERT_EQUALS_EPSILON(pd.inverse(0, pvalues[i]), tvalues[i], 1e-5);
  }
}

//===========================================================================
// test8
// case with dprob[t=0]=dprob[t=1]=dprob[t=2]=0
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test8()
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
  DefaultProbabilities pd(ratings, date);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &pd));

  for(int i=0; i<=add(date,2,'M')-date; i++)
  {
    ASSERT_EQUALS(pd.evalue(0, i), 0.0);
  }
  ASSERT(pd.evalue(0, (add(date,2,'M')-date)+1) > 0.0);


  ASSERT_EQUALS(pd.inverse(0, 0.0), add(date,2,'M')-date);
}

