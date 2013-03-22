
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

