
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#include "portfolio/Asset.hpp"
#include "portfolio/AssetTest.hpp"
#include "params/Interest.hpp"
#include "utils/Date.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::AssetTest::test1()
{
  Asset asset(3);

  ASSERT(asset.segments.size() == 3);
  for(size_t i=0; i<asset.segments.size(); i++) {
    ASSERT(asset.segments[i] == 0);
  }

  asset.values.push_back(DateValues(Date("01/01/2014"), 0.0, 0.0));
  asset.values.push_back(DateValues(Date("01/07/2014"), 100.0, 0.95));
  asset.values.push_back(DateValues(Date("01/01/2015"), 100.0, 0.85));
  asset.values.push_back(DateValues(Date("01/07/2015"), 100.0, 0.80));
  asset.values.push_back(DateValues(Date("01/01/2016"), 100.0, 0.75));
  asset.values.push_back(DateValues(Date("01/07/2016"), 100.0, 0.70));

  ASSERT(asset.isActive(Date("01/01/2010"), Date("01/07/2014")));
  ASSERT(asset.isActive(Date("01/01/2016"), Date("01/07/2020")));
  ASSERT(asset.isActive(Date("01/07/2014"), Date("01/01/2015")));
  ASSERT(asset.isActive(Date("01/01/2010"), Date("01/07/2010")));
  ASSERT(!asset.isActive(Date("01/01/2017"), Date("01/07/2017")));
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::AssetTest::test2()
{
  Asset asset;

  asset.values.push_back(DateValues(Date("01/01/2007"), 0.0, 0.0));
  asset.values.push_back(DateValues(Date("15/01/2007"), 1000.0));
  asset.values.push_back(DateValues(Date("15/01/2008"), 1000.0, 0.2));
  asset.values.push_back(DateValues(Date("15/01/2009"), 1000.0));
  asset.values.push_back(DateValues(Date("15/01/2010"), 1000.0, 0.2));

  Date time0("01/01/2005");
  Date timeT("01/01/2011");
  Interest interest(time0);

  asset.prepare(time0, timeT, interest);

  // doing assertions
  ASSERT(asset.values.size() == 5);
  ASSERT(asset.values[0].date == Date("01/01/2007"));
  ASSERT(asset.values[1].date == Date("15/01/2007"));
  ASSERT(asset.values[2].date == Date("15/01/2008"));
  ASSERT(asset.values[3].date == Date("15/01/2009"));
  ASSERT(asset.values[4].date == Date("15/01/2010"));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::AssetTest::test3()
{
  Asset asset;

  asset.values.push_back(DateValues(Date("15/01/2005"), 5000.0, 0.7));
  asset.values.push_back(DateValues(Date("15/01/2006"), 5000.0, 0.6));
  asset.values.push_back(DateValues(Date("15/01/2007"), 4000.0, 0.5));
  asset.values.push_back(DateValues(Date("15/01/2008"), 3000.0, 0.4));
  asset.values.push_back(DateValues(Date("15/01/2009"), 2000.0, 0.3));
  asset.values.push_back(DateValues(Date("15/01/2010"), 1000.0, 0.2));

  Date time0("10/01/2005");
  Date timeT("01/07/2007");
  Interest interest(time0);

  asset.prepare(time0, timeT, interest);

  // doing assertions
  ASSERT(asset.values.size() == 4);
  ASSERT(asset.values[0].date == Date("15/01/2005"));
  ASSERT(asset.values[3].date == Date("15/01/2008"));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::AssetTest::test4()
{
  Asset asset;

  asset.values.push_back(DateValues(Date("15/01/2005"), 5000.0, 0.7));
  asset.values.push_back(DateValues(Date("15/01/2006"), 5000.0, 0.6));
  asset.values.push_back(DateValues(Date("15/01/2007"), 4000.0, 0.5));
  asset.values.push_back(DateValues(Date("15/01/2008"), 3000.0, 0.4));
  asset.values.push_back(DateValues(Date("15/01/2009"), 2000.0, 0.3));
  asset.values.push_back(DateValues(Date("15/01/2010"), 1000.0, 0.2));

  Date time0("01/07/2006");
  Date timeT("01/07/2008");
  Interest interest(time0);

  asset.prepare(time0, timeT, interest);

  // doing assertions
  ASSERT(asset.values.size() == 3);
  ASSERT(asset.values[0].date == Date("15/01/2007"));
  ASSERT(asset.values[2].date == Date("15/01/2009"));
}

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::AssetTest::test5()
{
  Asset asset;

  asset.values.push_back(DateValues(Date("10/01/2006"), 5000.0, 0.7));

  Date time0("10/01/2005");
  Date timeT("01/07/2008");
  Interest interest(time0);

  asset.prepare(time0, timeT, interest);

  // doing assertions
  ASSERT(asset.values.size() == 1);
  ASSERT(asset.values[0].date == Date("10/01/2006"));
}

