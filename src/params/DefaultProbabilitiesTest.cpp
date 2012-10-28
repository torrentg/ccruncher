
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

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "params/DefaultProbabilities.hpp"
#include "params/DefaultProbabilitiesTest.hpp"
#include "utils/ExpatParser.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-6

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
      <dprob rating='A' t='0' value='0.00'/>\n\
      <dprob rating='A' t='2' value='0.50'/>\n\
      <dprob rating='A' t='3' value='0.75'/>\n\
      <dprob rating='A' t='5' value='0.90'/>\n\
      <!-- optionally you can add default rating info (value=1 always) -->\n\
    </dprobs>";
  double tvalues[] = { 0, 2, 3, 5 };
  double pvalues0[] = { 0.0, 0.5, 0.75, 0.9 };

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd(ratings);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &pd));

  // checking methods
  ASSERT_EQUALS(pd.size(), 2);
  ASSERT_EQUALS(pd.getMinCommonTime(), 5);
  ASSERT_EQUALS(pd.getIndexDefault(), 1);
  ASSERT_EQUALS(pd.getRatings().size(), 2);
  ASSERT_EQUALS(pd.getInterpolationType(0), "cspline");
  ASSERT_EQUALS(pd.getInterpolationType(1), "none");

  // checking evalue method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd.evalue(0, tvalues[i]), pvalues0[i], EPSILON);
    ASSERT_EQUALS_EPSILON(pd.evalue(1, tvalues[i]), 1.0, EPSILON)

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double t = tvalues[i-1] + j*(tvalues[i]-tvalues[i-1])/100.0;
        // monotone
        ASSERT(pd.evalue(0, tvalues[i-1]) < pd.evalue(0, t));
        ASSERT(pd.evalue(0, t) < pd.evalue(0, tvalues[i]));
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
    ASSERT_EQUALS_EPSILON(pd.inverse(0, pvalues0[i]), tvalues[i], EPSILON);
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
      <dprob rating='A' t='0' value='0.02'/>\n\
      <dprob rating='A' t='2' value='0.50'/>\n\
      <dprob rating='A' t='3' value='0.75'/>\n\
      <dprob rating='A' t='5' value='0.90'/>\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd(ratings);
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
      <dprob rating='A' t='0' value='0.00'/>\n\
      <dprob rating='A' t='2' value='0.50'/>\n\
      <dprob rating='A' t='3' value='0.25'/>\n\
      <dprob rating='A' t='5' value='0.50'/>\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd(ratings);
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
      <dprob rating='A' t='0' value='0.00'/>\n\
      <dprob rating='A' t='2' value='-0.50'/>\n\
      <dprob rating='A' t='3' value='-0.75'/>\n\
      <dprob rating='A' t='5' value='-0.85'/>\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &pd));
}

//===========================================================================
// test5 (equals than test1, but with diferent constructor)
//===========================================================================
void ccruncher_test::DefaultProbabilitiesTest::test5()
{
  int tvalues[] = { 0, 2, 3, 5 };
  double pvalues0[] = { 0.0, 0.5, 0.75, 0.9 };
  double pvalues1[] = { 1.0, 1.0, 1.00, 1.0 };

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  vector<vector<double> > mvalues(2);
  mvalues[0] = vector<double>(pvalues0, pvalues0+4);
  mvalues[1] = vector<double>(pvalues1, pvalues1+4);
  DefaultProbabilities pd(ratings, vector<int>(tvalues, tvalues+4), mvalues);

  // checking methods
  ASSERT_EQUALS(pd.size(), 2);
  ASSERT_EQUALS(pd.getMinCommonTime(), 5);
  ASSERT_EQUALS(pd.getIndexDefault(), 1);
  ASSERT_EQUALS(pd.getRatings().size(), 2);
  ASSERT_EQUALS(pd.getInterpolationType(0), "cspline");
  ASSERT_EQUALS(pd.getInterpolationType(1), "none");

  // checking evalue method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd.evalue(0, tvalues[i]), pvalues0[i], EPSILON);
    ASSERT_EQUALS_EPSILON(pd.evalue(1, tvalues[i]), 1.0, EPSILON)

    if (i > 0)
    {
      for(int j=1; j<100; j++)
      {
        double t = tvalues[i-1] + j*(tvalues[i]-tvalues[i-1])/100.0;
        // monotone
        ASSERT(pd.evalue(0, tvalues[i-1]) < pd.evalue(0, t));
        ASSERT(pd.evalue(0, t) < pd.evalue(0, tvalues[i]));
        // default = 1
        ASSERT_EQUALS_EPSILON(pd.evalue(1, t), 1.0, EPSILON);
        // inv(eval(t)) = t
        ASSERT_EQUALS_EPSILON(pd.inverse(0, pd.evalue(0, t)), t, 1e-6);
      }
    }
  }

  // checking inverse method
  for(int i=0; i<4; i++)
  {
    ASSERT_EQUALS_EPSILON(pd.inverse(0, pvalues0[i]), tvalues[i], EPSILON);
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
  int imonths[] = {0, 12};
  int ivalues[] = {0, 0};

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  vector<vector<double> > mvalues(2);
  mvalues[0] = vector<double>(mvalues1, mvalues1+2);
  mvalues[1] = vector<double>(mvalues2, mvalues2+2);
  DefaultProbabilities pd(ratings, vector<int>(imonths, imonths+2), mvalues);

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
      <dprob rating='A' t='0'  value='0.000'/>\n\
      <dprob rating='A' t='12' value='0.001'/>\n\
      <dprob rating='A' t='24' value='0.005'/>\n\
      <dprob rating='A' t='48' value='0.03'/>\n\
      <!-- optionally you can add default rating info (value=0 always) -->\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // dprob function creation
  DefaultProbabilities pd(ratings);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &pd));

  double pvalues[] = { 0.00005, 0.00010, 0.00100, 0.00200, 0.00300, 0.00400,
                       0.00500, 0.00600, 0.00700, 0.00800, 0.00900, 0.01000,
                       0.02000 };

  double tvalues[] = { 1.01708, 2.00505, 12     , 16.8421, 19.8848, 22.1566,
                       24     , 25.5733, 26.9742, 28.2563, 29.4507, 30.5772,
                       39.9419 };

  // checking values
  for(int i=0; i<13; i++)
  {
    ASSERT_EQUALS_EPSILON(pd.inverse(0, pvalues[i]), tvalues[i], 1e-5);
  }
}

