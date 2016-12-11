
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

#include "portfolio/Asset.hpp"
#include "portfolio/AssetTest.hpp"
#include "utils/Date.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 0.001

//===========================================================================
// getSegmentations
//===========================================================================
Segmentations ccruncher_test::AssetTest::getSegmentations()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
  <segmentations>\n\
    <segmentation name='portfolio' components='asset'/>\n\
    <segmentation name='sectors' components='obligor'>\n\
      <segment name='S1'/>\n\
      <segment name='S2'/>\n\
    </segmentation>\n\
    <segmentation name='size' components='obligor'>\n\
      <segment name='big'/>\n\
      <segment name='medium'/>\n\
    </segmentation>\n\
    <segmentation name='products' components='asset'>\n\
      <segment name='bond'/>\n\
    </segmentation>\n\
    <segmentation name='offices' components='asset'>\n\
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
// getInterest
//===========================================================================
Interest ccruncher_test::AssetTest::getInterest(const Date &date)
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='compound'>\n\
        <rate t='0M' r='0.0'/>\n\
        <rate t='1M' r='0.0'/>\n\
        <rate t='2M' r='0.0'/>\n\
        <rate t='3M' r='0.0'/>\n\
        <rate t='6M' r='0.0'/>\n\
        <rate t='1Y' r='0.0'/>\n\
        <rate t='2Y' r='0.0'/>\n\
        <rate t='5Y' r='0.0'/>\n\
        <rate t='10Y' r='0.0'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  // segmentation object creation
  Interest ret(date);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ret));

  return ret;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::AssetTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0003'/>\n\
        <data>\n\
          <values t='01/01/2000' ead='570.0' lgd='20%' />\n\
          <values t='01/07/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='550.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='530.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2020' ead='10.0' lgd='20%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  // doing assertions
  ASSERT(!asset.requiresObligorLGD());
  
  ASSERT(asset.belongsTo(0, 0)); // portfolio-rest
  ASSERT(asset.belongsTo(3, 1)); // product-bond
  ASSERT(asset.belongsTo(4, 3)); // office-0003

  ASSERT(asset.getSegment(4) == 3);
  asset.setSegment(4, 10);
  ASSERT(asset.getSegment(4) == 10);

  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("01/01/1995")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0, asset.getValues(Date("01/01/1995")).lgd.getValue(), EPSILON);
  
  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("01/01/1999")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0, asset.getValues(Date("01/01/1999")).lgd.getValue(), EPSILON);
  
  ASSERT_EQUALS_EPSILON(570.0, asset.getValues(Date("31/12/1999")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("31/12/1999")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(570.0, asset.getValues(Date("01/01/2000")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/01/2000")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(560.0, asset.getValues(Date("02/01/2000")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("02/01/2000")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(560.0, asset.getValues(Date("01/07/2000")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/07/2000")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(550.0, asset.getValues(Date("01/01/2001")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/01/2001")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(540.0, asset.getValues(Date("01/07/2001")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/07/2001")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(530.0, asset.getValues(Date("01/01/2002")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/01/2002")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(520.0, asset.getValues(Date("01/07/2002")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/07/2002")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(10.0, asset.getValues(Date("01/01/2003")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/01/2003")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(10.0, asset.getValues(Date("01/01/2010")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/01/2010")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(10.0, asset.getValues(Date("01/07/2020")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("01/07/2020")).lgd.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("01/07/2021")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0, asset.getValues(Date("01/07/2021")).lgd.getValue(), EPSILON);
}

//===========================================================================
// test2. crash test (xml with errors, data 01/01/2000 repeated)
//===========================================================================
void ccruncher_test::AssetTest::test2()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0001'/>\n\
        <data>\n\
          <values t='01/01/2000' ead='570.0' lgd='20%' />\n\
          <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='550.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='530.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2020' ead='10.0' lgd='20%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test3. crash test (xml with errors, events previous to asset creation date)
//===========================================================================
void ccruncher_test::AssetTest::test3()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='01/01/2005'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0001'/>\n\
        <data>\n\
          <values t='-5Y' ead='570.0' lgd='20%' />\n\
          <values t='01/07/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='550.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='530.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2020' ead='10.0' lgd='20%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test4.
//===========================================================================
void ccruncher_test::AssetTest::test4()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='01/01/2007'>\n\
        <data>\n\
           <values t='15/01/2007' ead='+1000.0' />\n\
           <values t='15/01/2008' ead='+1000.0' lgd='20%' />\n\
           <values t='15/01/2009' ead='+1000.0' />\n\
           <values t='15/01/2010' ead='+1000.0' lgd='20%' />\n\
           </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  Date time0("01/01/2005");
  Date timeT("01/01/2011");

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  
  Interest interest = getInterest(time0);
  asset.prepare(time0, timeT, interest);
  
  // doing assertions
  ASSERT(asset.requiresObligorLGD());
  ASSERT(!LGD::isvalid(asset.getValues(Date("10/01/2007")).lgd));
  ASSERT(!LGD::isvalid(asset.getValues(Date("15/01/2007")).lgd));
  ASSERT(LGD::isvalid(asset.getValues(Date("10/01/2008")).lgd));
  ASSERT(!LGD::isvalid(asset.getValues(Date("10/01/2009")).lgd));
  ASSERT(!LGD::isvalid(asset.getValues(Date("15/01/2009")).lgd));
  ASSERT(LGD::isvalid(asset.getValues(Date("10/01/2010")).lgd));
}

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::AssetTest::test5()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='10/01/2005' lgd='50%'>\n\
        <data>\n\
           <values t='15/01/2005' ead='5000.0' lgd='70%' />\n\
           <values t='15/01/2006' ead='5000.0' lgd='60%' />\n\
           <values t='15/01/2007' ead='4000.0' lgd='50%' />\n\
           <values t='15/01/2008' ead='3000.0' lgd='40%' />\n\
           <values t='15/01/2009' ead='2000.0' lgd='30%' />\n\
           <values t='15/01/2010' ead='1000.0' lgd='20%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  Date time0 = Date("1/1/2004");
  Date timeT = Date("1/7/2011");

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interest interest = getInterest(time0);

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  
  ASSERT(!asset.requiresObligorLGD());

  ASSERT_NO_THROW(asset.prepare(time0, timeT, interest));
  
  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("01/01/2004")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(5000.0, asset.getValues(Date("12/01/2005")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.7, asset.getValues(Date("12/01/2005")).lgd.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(5000.0, asset.getValues(Date("01/07/2005")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.6, asset.getValues(Date("01/07/2005")).lgd.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(1000.0, asset.getValues(Date("10/01/2010")).ead.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.2, asset.getValues(Date("10/01/2010")).lgd.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("16/01/2010")).ead.getValue(), EPSILON);
}

//===========================================================================
// test6 (datevalues not found)
//===========================================================================
void ccruncher_test::AssetTest::test6()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='10/01/2005' lgd='40%'>\n\
        <data>\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test7. crash test (lgd not found)
//===========================================================================
void ccruncher_test::AssetTest::test7()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0003'/>\n\
        <data>\n\
          <values t='01/01/2000' ead='570.0' />\n\
          <values t='01/07/2000' ead='560.0' />\n\
          <values t='01/01/2001' ead='550.0' />\n\
          <values t='01/07/2001' ead='540.0' />\n\
          <values t='01/01/2002' ead='530.0' />\n\
          <values t='01/07/2002' ead='520.0' />\n\
          <values t='01/07/2020' ead='10.0' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  ASSERT(asset.requiresObligorLGD());
}

//===========================================================================
// test8 (getValues)
//===========================================================================
void ccruncher_test::AssetTest::test8()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0003'/>\n\
        <data>\n\
          <values t='01/01/2000' ead='570.0' lgd='20%' />\n\
          <values t='01/07/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='550.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='530.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2020' ead='10.0' lgd='20%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  Date time0 = Date("1/1/1997");
  Date timeT = Date("1/7/2040");

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interest interest = getInterest(time0);

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  ASSERT_NO_THROW(asset.prepare(time0, timeT, interest));

  // assertions
  ASSERT_EQUALS(NAD, asset.getValues(Date("01/01/1995")).date);
  
  ASSERT_EQUALS(NAD, asset.getValues(Date("31/12/1998")).date);
  ASSERT_EQUALS(NAD, asset.getValues(Date("01/01/1999")).date);
  ASSERT_EQUALS(Date("01/01/2000"), asset.getValues(Date("02/01/1999")).date);
  
  ASSERT_EQUALS(Date("01/01/2000"), asset.getValues(Date("31/12/1999")).date);
  ASSERT_EQUALS(Date("01/01/2000"), asset.getValues(Date("01/01/2000")).date);
  ASSERT_EQUALS(Date("01/07/2000"), asset.getValues(Date("02/01/2000")).date);

  ASSERT_EQUALS(Date("01/07/2000"), asset.getValues(Date("01/06/2000")).date);
  ASSERT_EQUALS(Date("01/07/2000"), asset.getValues(Date("01/07/2000")).date);

  ASSERT_EQUALS(Date("01/01/2001"), asset.getValues(Date("01/12/2000")).date);
  ASSERT_EQUALS(Date("01/01/2001"), asset.getValues(Date("01/01/2001")).date);
  
  ASSERT_EQUALS(Date("01/07/2001"), asset.getValues(Date("01/06/2001")).date);
  ASSERT_EQUALS(Date("01/07/2001"), asset.getValues(Date("01/07/2001")).date);

  ASSERT_EQUALS(Date("01/01/2002"), asset.getValues(Date("01/12/2001")).date);
  ASSERT_EQUALS(Date("01/01/2002"), asset.getValues(Date("01/01/2002")).date);

  ASSERT_EQUALS(Date("01/07/2002"), asset.getValues(Date("01/06/2002")).date);
  ASSERT_EQUALS(Date("01/07/2002"), asset.getValues(Date("01/07/2002")).date);

  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("01/06/2020")).date);
  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("01/07/2020")).date);

  ASSERT_EQUALS(NAD, asset.getValues(Date("01/01/2035")).date);
}


//===========================================================================
// test9 (getValues)
//===========================================================================
void ccruncher_test::AssetTest::test9()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0003'/>\n\
        <data>\n\
          <values t='01/01/2000' ead='570.0' lgd='20%' />\n\
          <values t='01/07/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='550.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='530.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2020' ead='10.0' lgd='20%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  Date time0 = Date("1/1/2004");
  Date timeT = Date("1/7/2011");

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interest interest = getInterest(time0);

  // asset object creation
  Asset asset;
  xmlparser.UserData().segmentations = &segs;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  ASSERT_NO_THROW(asset.prepare(time0, timeT, interest));

  // assertions
  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("01/01/2003")).date);
  
  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("31/12/2003")).date);
  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("01/01/2004")).date);
  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("02/01/2004")).date);

  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("01/01/2009")).date);
  
  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("01/06/2011")).date);
  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("01/07/2011")).date);
  ASSERT_EQUALS(Date("01/07/2020"), asset.getValues(Date("02/07/2011")).date);
}
