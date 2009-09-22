
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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
// getSegmentations
//===========================================================================
Segmentations ccruncher_test::AssetTest::getSegmentations()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
  <segmentations>\n\
    <segmentation name='portfolio' components='asset'/>\n\
    <segmentation name='borrower' components='borrower'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='asset' components='asset'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='sector' components='borrower'>\n\
      <segment name='S1'/>\n\
      <segment name='S2'/>\n\
    </segmentation>\n\
    <segmentation name='size' components='borrower'>\n\
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
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
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
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
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
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  makeAssertions(&asset);
}

//===========================================================================
// test2. crash test (xml with errors, data 01/01/2000 repeated)
//===========================================================================
void ccruncher_test::AssetTest::test2()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
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
  Asset asset(&segs);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test3. crash test (xml with errors, events previous to asset creation date)
//===========================================================================
void ccruncher_test::AssetTest::test3()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
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
  Asset asset(&segs);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test4 (xml with errors, events previous to asset date)
//===========================================================================
void ccruncher_test::AssetTest::test4()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
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
  Asset asset(&segs);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::AssetTest::test5()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='10/01/2005'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='0.0' />\n\
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
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  Date time0 = Date("1/1/2004");
  Date timeT = Date("1/7/2011");
  ASSERT_NO_THROW(asset.precomputeLosses(time0, timeT, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2004")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/07/2004")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2005")), EPSILON);
  ASSERT_EQUALS_EPSILON(2000.0, asset.getLoss(Date("01/07/2005")), EPSILON);
  ASSERT_EQUALS_EPSILON(2000.0, asset.getLoss(Date("01/01/2006")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("01/07/2006")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("01/01/2007")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("01/07/2007")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("01/01/2008")), EPSILON);
  ASSERT_EQUALS_EPSILON(1250.0, asset.getLoss(Date("01/07/2008")), EPSILON);
  ASSERT_EQUALS_EPSILON(1250.0, asset.getLoss(Date("01/01/2009")), EPSILON);
  ASSERT_EQUALS_EPSILON(750.0, asset.getLoss(Date("01/07/2009")), EPSILON);
  ASSERT_EQUALS_EPSILON(750.0, asset.getLoss(Date("01/01/2010")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/07/2010")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2011")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/07/2011")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2012")), EPSILON);
}

//===========================================================================
// test6
//===========================================================================
void ccruncher_test::AssetTest::test6()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='10/01/2005'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='0.0' />\n\
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
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  Date time0 = Date("1/1/2004");
  Date timeT = Date("1/7/2011");
  ASSERT_NO_THROW(asset.precomputeLosses(time0, timeT, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2000")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2005")), EPSILON);
  ASSERT_EQUALS_EPSILON(1000.0, asset.getLoss(Date("15/01/2005")), EPSILON);
  ASSERT_EQUALS_EPSILON(2000.0, asset.getLoss(Date("15/01/2006")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("15/01/2007")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("15/01/2008")), EPSILON);
  ASSERT_EQUALS_EPSILON(1250.0, asset.getLoss(Date("15/01/2009")), EPSILON);
  ASSERT_EQUALS_EPSILON(750.0, asset.getLoss(Date("15/01/2010")), EPSILON);
  ASSERT_EQUALS_EPSILON(400.0, asset.getLoss(Date("15/03/2010")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("15/01/2011")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2020")), EPSILON);
}

//===========================================================================
// test7
//===========================================================================
void ccruncher_test::AssetTest::test7()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='10/01/2005'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='0.0' />\n\
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
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  Date time0 = Date("1/1/2004");
  Date timeT = Date("1/1/2012");
  ASSERT_NO_THROW(asset.precomputeLosses(time0, timeT, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2004")), EPSILON);
  ASSERT_EQUALS_EPSILON(2000.0, asset.getLoss(Date("01/01/2006")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("01/01/2008")), EPSILON);
  ASSERT_EQUALS_EPSILON(750.0, asset.getLoss(Date("01/01/2010")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2012")), EPSILON);
}

//===========================================================================
// test8
//===========================================================================
void ccruncher_test::AssetTest::test8()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='10/01/2005'>\n\
        <data>\n\
          <values at='15/01/2005' cashflow='-4000.0' recovery='0.0' />\n\
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
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  Date time0 = Date("15/1/2004");
  Date timeT = Date("15/1/2011");
  ASSERT_NO_THROW(asset.precomputeLosses(time0, timeT, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("15/01/2004")), EPSILON);
  ASSERT_EQUALS_EPSILON(1000.0, asset.getLoss(Date("15/01/2005")), EPSILON);
  ASSERT_EQUALS_EPSILON(2000.0, asset.getLoss(Date("15/01/2006")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("15/01/2007")), EPSILON);
  ASSERT_EQUALS_EPSILON(1500.0, asset.getLoss(Date("15/01/2008")), EPSILON);
  ASSERT_EQUALS_EPSILON(1250.0, asset.getLoss(Date("15/01/2009")), EPSILON);
  ASSERT_EQUALS_EPSILON(750.0, asset.getLoss(Date("15/01/2010")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getLoss(Date("01/01/2011")), EPSILON);
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
  ASSERT_EQUALS(7, (int) data.size()); // num events

  ASSERT(Date("01/01/2000") == data[0].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[0].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[0].recovery, EPSILON);

  ASSERT(Date("01/07/2000") == data[1].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[1].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[1].recovery, EPSILON);

  ASSERT(Date("01/01/2001") == data[2].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[2].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[2].recovery, EPSILON);

  ASSERT(Date("01/07/2001") == data[3].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[3].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[3].recovery, EPSILON);

  ASSERT(Date("01/01/2002") == data[4].date);
  ASSERT_EQUALS_EPSILON(+10.0 , data[4].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[4].recovery, EPSILON);

  ASSERT(Date("01/07/2002") == data[5].date);
  ASSERT_EQUALS_EPSILON(+510.0, data[5].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+450.0, data[5].recovery, EPSILON);

  ASSERT(Date("01/07/2020") == data[6].date);
  ASSERT_EQUALS_EPSILON(+10.0, data[6].cashflow, EPSILON);
  ASSERT_EQUALS_EPSILON(+3.0, data[6].recovery, EPSILON);

  Date time0 = Date("1/1/1999");
  Date timeT = Date("1/1/2010");
  ASSERT_NO_THROW(asset->precomputeLosses(time0, timeT, interests));

  ASSERT_EQUALS_EPSILON(0.0, asset->getLoss(Date("01/01/1995")), EPSILON);
  ASSERT_EQUALS_EPSILON(120.0, asset->getLoss(Date("01/01/1999")), EPSILON);
  ASSERT_EQUALS_EPSILON(120.0, asset->getLoss(Date("31/12/1999")), EPSILON);
  ASSERT_EQUALS_EPSILON(120.0, asset->getLoss(Date("01/01/2000")), EPSILON);
  ASSERT_EQUALS_EPSILON(110.0, asset->getLoss(Date("02/01/2000")), EPSILON);
  ASSERT_EQUALS_EPSILON(110.0, asset->getLoss(Date("01/07/2000")), EPSILON);
  ASSERT_EQUALS_EPSILON(100.0, asset->getLoss(Date("01/01/2001")), EPSILON);
  ASSERT_EQUALS_EPSILON(90.0, asset->getLoss(Date("01/07/2001")), EPSILON);
  ASSERT_EQUALS_EPSILON(80.0, asset->getLoss(Date("01/01/2002")), EPSILON);
  ASSERT_EQUALS_EPSILON(70.0, asset->getLoss(Date("01/07/2002")), EPSILON);
  ASSERT_EQUALS_EPSILON(7.0, asset->getLoss(Date("01/01/2003")), EPSILON);
  ASSERT_EQUALS_EPSILON(7.0, asset->getLoss(Date("01/01/2010")), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset->getLoss(Date("01/07/2021")), EPSILON);
}

