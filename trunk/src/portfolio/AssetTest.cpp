
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

  asset.values.push_back(DateValues(Date("01/01/2014"), NAN, 1.00));
  asset.values.push_back(DateValues(Date("01/07/2014"), 100.0, 0.95));
  asset.values.push_back(DateValues(Date("01/01/2015"), 100.0, 0.85));
  asset.values.push_back(DateValues(Date("01/07/2015"), 100.0, 0.80));
  asset.values.push_back(DateValues(Date("01/01/2016"), 100.0, 0.75));
  asset.values.push_back(DateValues(Date("01/07/2016"), 100.0, 0.70));

  ASSERT(asset.isActive(Date("01/01/2010"), Date("01/07/2014")));
  ASSERT(asset.isActive(Date("01/01/2016"), Date("01/07/2020")));
  ASSERT(asset.isActive(Date("01/07/2014"), Date("01/01/2015")));
  ASSERT(!asset.isActive(Date("01/01/2010"), Date("01/07/2010")));
  ASSERT(!asset.isActive(Date("01/01/2017"), Date("01/07/2017")));
}

//===========================================================================
// test2.
//===========================================================================
void ccruncher_test::AssetTest::test2()
{
  Asset asset;

  asset.values.push_back(DateValues(Date("01/01/2007"), NAN, 1.00));
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

