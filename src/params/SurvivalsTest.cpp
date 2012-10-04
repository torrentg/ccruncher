
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
#include "params/Survivals.hpp"
#include "params/SurvivalsTest.hpp"
#include "utils/ExpatParser.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-6

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::SurvivalsTest::getRatings()
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
void ccruncher_test::SurvivalsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <survivals>\n\
      <svalue rating='A' t='0' value='1.00'/>\n\
      <svalue rating='A' t='2' value='0.50'/>\n\
      <svalue rating='A' t='3' value='0.25'/>\n\
      <svalue rating='A' t='5' value='0.10'/>\n\
      <!-- optionally you can add default rating info (value=0 always) -->\n\
    </survivals>";
  double svalues[] = { 1.00, 0.75, 0.50, 0.25, 0.175, 0.10, 1.0};
  double ivalues[] = { 17.0, 5.0, 3.66666666, 2.8, 2.4, 2.0, 1.6, 1.2, 0.8, 0.4, 0.0};

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survivals sf(ratings);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sf));

  // checking values
  for(int i=0;i<7;i++)
  {
    ASSERT_EQUALS_EPSILON(svalues[i], sf.evalue(0, i), EPSILON);
    ASSERT_EQUALS_EPSILON(sf.evalue(1, i), 0.0, EPSILON)
  }

  // checking inverse values
  for(int i=0;i<=10;i++)
  {
    ASSERT_EQUALS_EPSILON(ivalues[i], sf.inverse(0, i/10.0), EPSILON);
    ASSERT_EQUALS_EPSILON(0.0, sf.inverse(1, i/10.0), EPSILON);
  }
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::SurvivalsTest::test2()
{
  // non valid survival function (value at t=0 distinct that 1)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <survivals>\n\
      <svalue rating='A' t='0' value='0.98'/>\n\
      <svalue rating='A' t='2' value='0.50'/>\n\
      <svalue rating='A' t='3' value='0.25'/>\n\
      <svalue rating='A' t='5' value='0.10'/>\n\
    </survivals>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survivals sf(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sf));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::SurvivalsTest::test3()
{
  // non valid survival function, non monotone
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <survivals>\n\
      <svalue rating='A' t='0' value='1.00'/>\n\
      <svalue rating='A' t='2' value='0.50'/>\n\
      <svalue rating='A' t='3' value='0.75'/>\n\
      <svalue rating='A' t='5' value='0.10'/>\n\
    </survivals>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survivals sf(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sf));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::SurvivalsTest::test4()
{
  // non valid transition matrix (values out of range [0,1])
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <survivals>\n\
      <svalue rating='A' t='0' value='1.00'/>\n\
      <svalue rating='A' t='2' value='-0.50'/>\n\
      <svalue rating='A' t='3' value='-0.75'/>\n\
      <svalue rating='A' t='5' value='-0.85'/>\n\
    </survivals>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survivals sf(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sf));
}

//===========================================================================
// test5 (equals than test1, but with diferent constructor)
//===========================================================================
void ccruncher_test::SurvivalsTest::test5()
{
  double mvalues1[] = {1.00, 0.75, 0.50, 0.25, 0.175, 0.10};
  double mvalues2[] = {0.00, 0.00, 0.00, 0.00, 0.000, 0.00};
  int imonths[] = {0, 1, 2, 3, 4, 5};
  double svalues[] = { 1.00, 0.75, 0.50, 0.25, 0.175, 0.10, 1.0};
  double ivalues[] = { 17.0, 5.0, 3.66666666, 2.8, 2.4, 2.0, 1.6, 1.2, 0.8, 0.4, 0.0};

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  vector<vector<double> > mvalues(2);
  mvalues[0] = vector<double>(mvalues1, mvalues1+6);
  mvalues[1] = vector<double>(mvalues2, mvalues2+6);
  Survivals sf(ratings, vector<int>(imonths, imonths+6), mvalues);

  // checking values
  for(int i=0;i<7;i++)
  {
    ASSERT_EQUALS_EPSILON(svalues[i], sf.evalue(0,i), EPSILON);
    ASSERT_EQUALS_EPSILON(sf.evalue(1, i), 0.0, EPSILON)
  }

  // checking inverse values
  for(int i=0;i<=10;i++)
  {
    ASSERT_EQUALS_EPSILON(ivalues[i], sf.inverse(0, i/10.0), EPSILON);
    ASSERT_EQUALS_EPSILON(0.0, sf.inverse(1, i/10.0), EPSILON);
  }
}

//===========================================================================
// test6 (checks distribution assumptions)
//===========================================================================
void ccruncher_test::SurvivalsTest::test6()
{
  double mvalues1[] = {1.00, 0.90};
  double mvalues2[] = {0.00, 0.00};
  int imonths[] = {0, 12};
  int ivalues[] = {0, 0};

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  vector<vector<double> > mvalues(2);
  mvalues[0] = vector<double>(mvalues1, mvalues1+2);
  mvalues[1] = vector<double>(mvalues2, mvalues2+2);
  Survivals sf(ratings, vector<int>(imonths, imonths+2), mvalues);

  // creating randomizer
  gsl_rng *rng = gsl_rng_alloc(gsl_rng_mt19937);

  // checking values
  for(int i=0;i<20000;i++)
  {
    double u = gsl_ran_flat (rng, 0.0, 1.0);
    if (sf.inverse(0,u) > 12) {
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
// check precision when survival values are near to 100% and ISURVFNUMBINS=100
//===========================================================================
void ccruncher_test::SurvivalsTest::test7()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <survivals>\n\
      <svalue rating='A' t='0' value='1.00'/>\n\
      <svalue rating='A' t='12' value='0.999'/>\n\
      <svalue rating='A' t='24' value='0.995'/>\n\
      <svalue rating='A' t='48' value='0.97'/>\n\
      <!-- optionally you can add default rating info (value=0 always) -->\n\
    </survivals>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survivals sf(ratings);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sf));

  // checking values
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99995), 0.6, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99990), 1.2, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99900), 12.0, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99800), 15.0, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99700), 18.0, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99600), 21.0, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99500), 24.0, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99400), 24.96, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99300), 25.92, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99200), 26.88, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99100), 27.84, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.99000), 28.80, EPSILON);
  ASSERT_EQUALS_EPSILON(sf.inverse(0, 0.98000), 38.40, EPSILON);
}

