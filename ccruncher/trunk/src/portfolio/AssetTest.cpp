
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// AssetTest.cpp - AssetTest code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/03/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . asset refactoring
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/07/09 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed exposure/recovery by netting
//
// 2005/08/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . tag concept renamed to segmentation
//
// 2005/09/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . update to t[0] -> update to t[n-1]
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Asset refactoring
//   . modified asset value algorithm
//
// 2006/01/05 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed simulate=values option
//   . netting replaced by recovery
//
// 2007/07/30 - Gerard Torrent [gerard@mail.generacio.com]
//   . added asset creation date
//   . added new tests
//   . added precomputeLosses function
//
//===========================================================================

#include <iostream>
#include <algorithm>
#include "portfolio/Asset.hpp"
#include "portfolio/AssetTest.hpp"
#include "utils/Date.hpp"
#include "utils/ExpatParser.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.001

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::AssetTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::AssetTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// getSegmentations1
//===========================================================================
Segmentations ccruncher_test::AssetTest::getSegmentations()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
  <segmentations>\n\
    <segmentation name='portfolio' components='asset'/>\n\
    <segmentation name='client' components='client'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='asset' components='asset'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='sector' components='client'>\n\
      <segment name='S1'/>\n\
      <segment name='S2'/>\n\
    </segmentation>\n\
    <segmentation name='size' components='client'>\n\
      <segment name='big'/>\n\
      <segment name='medium'/>\n\
    </segmentation>\n\
    <segmentation name='product' components='asset'>\n\
      <segment name='bond'/>\n\
    </segmentation>\n\
    <segmentation name='office' components='asset'>\n\
      <segment name='0001'/>\n\
      <segment name='0002'/>\n\
      <segment name='0003'/>\n\
      <segment name='0004'/>\n\
    </segmentation>\n\
  </segmentations>";

  // creating xml
  ExpatParser xmlparser;

  // segmentation object creation
  Segmentations ret;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ret));

  return ret;
}

//===========================================================================
// getInterests
//===========================================================================
Interests ccruncher_test::AssetTest::getInterests()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <interests>\n\
      <interest name='spot' date='18/02/2003'>\n\
        <rate t='0' r='0.0'/>\n\
        <rate t='1' r='0.0'/>\n\
        <rate t='2' r='0.0'/>\n\
        <rate t='3' r='0.0'/>\n\
        <rate t='6' r='0.0'/>\n\
        <rate t='12' r='0.0'/>\n\
        <rate t='24' r='0.0'/>\n\
        <rate t='60' r='0.0'/>\n\
        <rate t='120' r='0.0'/>\n\
      </interest>\n\
      <interest name='discount' date='18/02/2003'>\n\
        <rate t='0' r='0.0'/>\n\
        <rate t='1' r='0.04'/>\n\
        <rate t='2' r='0.041'/>\n\
        <rate t='3' r='0.045'/>\n\
        <rate t='6' r='0.0455'/>\n\
        <rate t='12' r='0.048'/>\n\
        <rate t='24' r='0.049'/>\n\
        <rate t='60' r='0.05'/>\n\
        <rate t='120' r='0.052'/>\n\
      </interest>\n\
    </interests>";

  // creating xml
  ExpatParser xmlparser;

  // segmentation object creation
  Interests ret;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ret));

  return ret;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::AssetTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='product' segment='bond'/>\n\
        <belongs-to segmentation='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
          <values at='01/07/2020' cashflow='10.0' recovery='3.0' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset(segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  makeAssertions(&asset);
}

//===========================================================================
// test2. crash test (xml with errors, data 01/01/2000 repeated)
//===========================================================================
void ccruncher_test::AssetTest::test2()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='product' segment='bond'/>\n\
        <belongs-to segmentation='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
          <values at='01/07/2020' cashflow='10.0' recovery='10.0' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset(segs);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test3. crash test (xml with errors, events previous to asset creation date)
//===========================================================================
void ccruncher_test::AssetTest::test3()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset name='generic' id='op1' date='01/01/2005'>\n\
        <belongs-to segmentation='product' segment='bond'/>\n\
        <belongs-to segmentation='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
          <values at='01/07/2020' cashflow='10.0' recovery='10.0' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset(segs);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test4 (xml with errors, events previous to asset date)
//===========================================================================
void ccruncher_test::AssetTest::test4()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset name='generic' id='op1' date='01/01/2007'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='2500.0' />\n\
          <values at='15/01/2006' cashflow='+1000.0' recovery='3000.0' />\n\
          <values at='15/01/2007' cashflow='+1000.0' recovery='2500.0' />\n\
          <values at='15/01/2008' cashflow='+1000.0' recovery='3500.0' />\n\
          <values at='15/01/2009' cashflow='+1000.0' recovery='750' />\n\
          <values at='15/01/2010' cashflow='+1000.0' recovery='250' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset(segs);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::AssetTest::test5()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset name='generic' id='op1' date='15/01/2005'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='2500.0' />\n\
          <values at='15/01/2006' cashflow='+1000.0' recovery='3000.0' />\n\
          <values at='15/01/2007' cashflow='+1000.0' recovery='2500.0' />\n\
          <values at='15/01/2008' cashflow='+1000.0' recovery='1500.0' />\n\
          <values at='15/01/2009' cashflow='+1000.0' recovery='750' />\n\
          <values at='15/01/2010' cashflow='+1000.0' recovery='250' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interests interests = getInterests();

  // asset object creation
  Asset asset(segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  Date dates[] = { Date("1/1/2004"), Date("1/7/2004"), Date("1/1/2005"), 
                   Date("1/7/2005"), Date("1/1/2006"), Date("1/7/2006"),
                   Date("1/1/2007"), Date("1/7/2007"), Date("1/1/2008"), 
                   Date("1/7/2008"), Date("1/1/2009"), Date("1/7/2009"), 
                   Date("1/1/2010"), Date("1/7/2010"), Date("1/1/2011"),
                   Date("1/7/2011") };
  ASSERT_NO_THROW(asset.precomputeLosses(dates, 16, interests));

  // observe that losses at 01/07/xx contains 15 days of one type of 
  // recovery and the rest of other type of recovery
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(0), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(1), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(2), EPSILON);
  ASSERT_EQUALS_EPSILON(1845.3, asset.getLoss(3), EPSILON);
  ASSERT_EQUALS_EPSILON(2000.0, asset.getLoss(4), EPSILON);
  ASSERT_EQUALS_EPSILON(1538.67, asset.getLoss(5), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(6), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(7), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(8), EPSILON);
  ASSERT_EQUALS_EPSILON(1269.23, asset.getLoss(9), EPSILON);
  ASSERT_EQUALS_EPSILON(1250.0, asset.getLoss(10), EPSILON);
  ASSERT_EQUALS_EPSILON(788.674, asset.getLoss(11), EPSILON);
  ASSERT_EQUALS_EPSILON(750.0, asset.getLoss(12), EPSILON);
  ASSERT_EQUALS_EPSILON(58.011, asset.getLoss(13), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(14), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(15), EPSILON);
}

//===========================================================================
// test6
//===========================================================================
void ccruncher_test::AssetTest::test6()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset name='generic' id='op1' date='15/01/2005'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='2500.0' />\n\
          <values at='15/01/2006' cashflow='+1000.0' recovery='3000.0' />\n\
          <values at='15/01/2007' cashflow='+1000.0' recovery='2500.0' />\n\
          <values at='15/01/2008' cashflow='+1000.0' recovery='1500.0' />\n\
          <values at='15/01/2009' cashflow='+1000.0' recovery='750' />\n\
          <values at='15/01/2010' cashflow='+500.0' recovery='250' />\n\
          <values at='15/03/2010' cashflow='+500.0' recovery='100' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interests interests = getInterests();

  // asset object creation
  Asset asset(segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  Date dates[] = { Date("1/1/2004"), Date("1/7/2004"), Date("1/1/2005"), 
                   Date("1/7/2005"), Date("1/1/2006"), Date("1/7/2006"),
                   Date("1/1/2007"), Date("1/7/2007"), Date("1/1/2008"), 
                   Date("1/7/2008"), Date("1/1/2009"), Date("1/7/2009"), 
                   Date("1/1/2010"), Date("1/7/2010"), Date("1/1/2011"),
                   Date("1/7/2011") };
  ASSERT_NO_THROW(asset.precomputeLosses(dates, 16, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(0), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(1), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(2), EPSILON);
  ASSERT_EQUALS_EPSILON(1845.3, asset.getLoss(3), EPSILON);
  ASSERT_EQUALS_EPSILON(2000.0, asset.getLoss(4), EPSILON);
  ASSERT_EQUALS_EPSILON(1538.67, asset.getLoss(5), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(6), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(7), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(8), EPSILON);
  ASSERT_EQUALS_EPSILON(1269.23, asset.getLoss(9), EPSILON);
  ASSERT_EQUALS_EPSILON(1250.0, asset.getLoss(10), EPSILON);
  ASSERT_EQUALS_EPSILON(788.674, asset.getLoss(11), EPSILON);
  ASSERT_EQUALS_EPSILON(750.0, asset.getLoss(12), EPSILON);
  ASSERT_EQUALS_EPSILON(188.398, asset.getLoss(13), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(14), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(15), EPSILON);
}

//===========================================================================
// test7
//===========================================================================
void ccruncher_test::AssetTest::test7()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset name='generic' id='op1' date='15/01/2005'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='2500.0' />\n\
          <values at='15/01/2006' cashflow='+1000.0' recovery='3000.0' />\n\
          <values at='15/01/2007' cashflow='+1000.0' recovery='2500.0' />\n\
          <values at='15/01/2008' cashflow='+1000.0' recovery='1500.0' />\n\
          <values at='15/01/2009' cashflow='+1000.0' recovery='750' />\n\
          <values at='15/01/2010' cashflow='+1000.0' recovery='250' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interests interests = getInterests();

  // asset object creation
  Asset asset(segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  Date dates[] = { Date("1/1/2004"), Date("1/1/2006"), Date("1/1/2008"), 
                   Date("1/1/2010"), Date("1/1/2012") };
  ASSERT_NO_THROW(asset.precomputeLosses(dates, 5, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(0), EPSILON);
  ASSERT_EQUALS_EPSILON(960.328, asset.getLoss(1), EPSILON);
  ASSERT_EQUALS_EPSILON(1509.59, asset.getLoss(2), EPSILON);
  ASSERT_EQUALS_EPSILON(1014.71, asset.getLoss(3), EPSILON);
  ASSERT_EQUALS_EPSILON(14.3836, asset.getLoss(4), EPSILON);
}

//===========================================================================
// test8
//===========================================================================
void ccruncher_test::AssetTest::test8()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset name='generic' id='op1' date='15/01/2005'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='2500.0' />\n\
          <values at='15/01/2006' cashflow='+1000.0' recovery='3000.0' />\n\
          <values at='15/01/2007' cashflow='+1000.0' recovery='2500.0' />\n\
          <values at='15/01/2008' cashflow='+1000.0' recovery='1500.0' />\n\
          <values at='15/01/2009' cashflow='+1000.0' recovery='750' />\n\
          <values at='15/01/2010' cashflow='+1000.0' recovery='250' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interests interests = getInterests();

  // asset object creation
  Asset asset(segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  Date dates[] = { Date("15/1/2004"), Date("15/1/2005"), Date("15/1/2006"), 
                   Date("15/1/2007"), Date("15/1/2008"), Date("15/1/2009"), 
                   Date("15/1/2010"), Date("15/1/2011") };
  ASSERT_NO_THROW(asset.precomputeLosses(dates, 8, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(0), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(1), EPSILON);
  ASSERT_EQUALS_EPSILON(2000.0, asset.getLoss(2), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(3), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(4), EPSILON);
  ASSERT_EQUALS_EPSILON(1250.0, asset.getLoss(5), EPSILON);
  ASSERT_EQUALS_EPSILON(750.0, asset.getLoss(6), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(7), EPSILON);
}

//===========================================================================
// makeAssertions
//===========================================================================
void ccruncher_test::AssetTest::makeAssertions(Asset *asset)
{
  Interests interests = getInterests();

  ASSERT(asset->belongsTo(0, 0)); // portfolio-rest
  ASSERT(asset->belongsTo(2, 1)); // asset-op1
  ASSERT(asset->belongsTo(5, 1)); // product-bond
  ASSERT(asset->belongsTo(6, 1)); // office-0001

  vector <DateValues> &data = asset->getData();
  ASSERT_EQUALS(8, (int) data.size()); // num events + asset creation date

  ASSERT(Date("01/01/1999") == data[0].date); // creation asset event
  ASSERT_EQUALS_EPSILON(+0.0, data[0].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+0.0, data[0].recovery, EPSILON);

  ASSERT(Date("01/01/2000") == data[1].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[1].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[1].recovery, EPSILON);

  ASSERT(Date("01/07/2000") == data[2].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[2].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[2].recovery, EPSILON);

  ASSERT(Date("01/01/2001") == data[3].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[3].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[3].recovery, EPSILON);

  ASSERT(Date("01/07/2001") == data[4].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[4].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[4].recovery, EPSILON);

  ASSERT(Date("01/01/2002") == data[5].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[5].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[5].recovery, EPSILON);

  ASSERT(Date("01/07/2002") == data[6].date);
  ASSERT_EQUALS_EPSILON(+510.0, data[6].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[6].recovery, EPSILON);

  ASSERT(Date("01/07/2020") == data[7].date);
  ASSERT_EQUALS_EPSILON(+10.0, data[7].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+3.0, data[7].recovery, EPSILON);

  Date dates[] = { Date("1/1/1999"), Date("1/1/2000"), Date("1/6/2002"), Date("1/1/2010") };
  ASSERT_NO_THROW(asset->precomputeLosses(dates, 4, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset->getLoss(0), EPSILON);
  ASSERT_EQUALS_EPSILON(120.0, asset->getLoss(1), EPSILON);
  ASSERT_EQUALS_EPSILON(90.681, asset->getLoss(2), EPSILON);
  ASSERT_EQUALS_EPSILON(7.68206, asset->getLoss(3), EPSILON);
}

