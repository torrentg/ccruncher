
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

#include <gsl/gsl_cdf.h>
#include "kernel/InversesTest.hpp"
#include "kernel/Inverses.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::InversesTest::getRatings()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='alive'/>\n\
      <rating name='E' description='default'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;
  Ratings ret;
  xmlparser.parse(xmlcontent, &ret);

  return ret;
}

//===========================================================================
// getRatings
//===========================================================================
DefaultProbabilities ccruncher_test::InversesTest::getDefaultProbabilities(const Date &date)
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <dprobs>\n\
      <dprob rating='A' t='0D' value='0.00'/>\n\
      <dprob rating='A' t='1Y' value='0.10'/>\n\
    </dprobs>";

  // creating xml
  ExpatParser xmlparser;
  Ratings ratings = getRatings();
  DefaultProbabilities ret(ratings, date);
  xmlparser.parse(xmlcontent, &ret);

  return ret;
}

//===========================================================================
// test1
// gaussian case
//===========================================================================
void ccruncher_test::InversesTest::test1()
{
  Date date0("1/1/2012");
  Date date1("1/1/2013");
  DefaultProbabilities dprobs = getDefaultProbabilities(date0);
  Inverses inverses(0.0, date1, dprobs);

  double minday = inverses.evalue(0, -1e100);
  double maxday = inverses.evalue(0, +1e100);
  double minval = gsl_cdf_ugaussian_Pinv(dprobs.evalue(0, minday));
  double maxval = gsl_cdf_ugaussian_Pinv(dprobs.evalue(0, maxday));

  for(int i=1; i<10000; i++)
  {
    double x = minval + i*(maxval-minval)/10000.0;
    double u = gsl_cdf_ugaussian_P(x);
    double days1 = dprobs.inverse(0, u);
    double days2 = inverses.evalue(0, x);
    ASSERT_EQUALS_EPSILON(days1, days2, 1.0/24.0);
  }

  for(int i=1; i<=(date1-date0); i++)
  {
    Date date = date0 + i;
    double u = dprobs.evalue(0, date);
    double x = gsl_cdf_ugaussian_Pinv(u);
    double d = inverses.evalue(0, x);
    ASSERT_EQUALS((date-date0), (long)(d+0.5));
  }
}

//===========================================================================
// test2
// t-Student case
//===========================================================================
void ccruncher_test::InversesTest::test2()
{
  double ndf = 3.0;
  Date date0("1/1/2012");
  Date date1("1/1/2013");
  DefaultProbabilities dprobs = getDefaultProbabilities(date0);
  Inverses inverses(ndf, date1, dprobs);

  double minday = inverses.evalue(0, -1e100);
  double maxday = inverses.evalue(0, +1e100);
  double minval = gsl_cdf_tdist_Pinv(dprobs.evalue(0, minday), ndf);
  double maxval = gsl_cdf_tdist_Pinv(dprobs.evalue(0, maxday), ndf);

  for(int i=1; i<10000; i++)
  {
    double x = minval + i*(maxval-minval)/10000.0;
    double u = gsl_cdf_tdist_P(x, ndf);
    double days1 = dprobs.inverse(0, u);
    double days2 = inverses.evalue(0, x);
    ASSERT_EQUALS_EPSILON(days1, days2, 1.0/24.0);
  }

  for(int i=1; i<=(date1-date0); i++)
  {
    Date date = date0 + i;
    double u = dprobs.evalue(0, date);
    double x = gsl_cdf_tdist_Pinv(u, ndf);
    double d = inverses.evalue(0, x);
    ASSERT_EQUALS((date-date0), (long)(d+0.5));
  }
}

//===========================================================================
// test3
// fails because maxdate bigger than dprob::maxDate
//===========================================================================
void ccruncher_test::InversesTest::test3()
{
  double ndf = 3.0;
  Date date0("1/1/2012");
  Date date1("1/1/2014");
  DefaultProbabilities dprobs = getDefaultProbabilities(date0);
  Inverses inverses;
  ASSERT_THROW(inverses.init(ndf, date1, dprobs));
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

