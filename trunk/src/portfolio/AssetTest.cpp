
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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
    <segmentation name='obligors' components='obligor'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='assets' components='asset'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
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
Interest ccruncher_test::AssetTest::getInterest()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest type='compound'>\n\
        <rate t='0' r='0.0'/>\n\
        <rate t='1' r='0.0'/>\n\
        <rate t='2' r='0.0'/>\n\
        <rate t='3' r='0.0'/>\n\
        <rate t='6' r='0.0'/>\n\
        <rate t='12' r='0.0'/>\n\
        <rate t='24' r='0.0'/>\n\
        <rate t='60' r='0.0'/>\n\
        <rate t='120' r='0.0'/>\n\
      </interest>";

  // creating xml
  ExpatParser xmlparser;

  // segmentation object creation
  Interest ret;
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
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0003'/>\n\
        <data>\n\
          <values at='01/01/2000' exposure='570.0' recovery='80%' />\n\
          <values at='01/07/2000' exposure='560.0' recovery='80%' />\n\
          <values at='01/01/2001' exposure='550.0' recovery='80%' />\n\
          <values at='01/07/2001' exposure='540.0' recovery='80%' />\n\
          <values at='01/01/2002' exposure='530.0' recovery='80%' />\n\
          <values at='01/07/2002' exposure='520.0' recovery='80%' />\n\
          <values at='01/07/2020' exposure='10.0' recovery='80%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));

  // doing assertions
  ASSERT(!asset.hasObligorRecovery());
  
  ASSERT(asset.belongsTo(0, 0)); // portfolio-rest
  ASSERT(asset.belongsTo(2, 1)); // asset-op1
  ASSERT(asset.belongsTo(5, 1)); // product-bond
  ASSERT(asset.belongsTo(6, 3)); // office-0003

  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("01/01/1995")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0, asset.getValues(Date("01/01/1995")).recovery.getValue(), EPSILON);
  
  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("01/01/1999")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0, asset.getValues(Date("01/01/1999")).recovery.getValue(), EPSILON);
  
  ASSERT_EQUALS_EPSILON(570.0, asset.getValues(Date("31/12/1999")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("31/12/1999")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(570.0, asset.getValues(Date("01/01/2000")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/01/2000")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(560.0, asset.getValues(Date("02/01/2000")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("02/01/2000")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(560.0, asset.getValues(Date("01/07/2000")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/07/2000")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(550.0, asset.getValues(Date("01/01/2001")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/01/2001")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(540.0, asset.getValues(Date("01/07/2001")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/07/2001")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(530.0, asset.getValues(Date("01/01/2002")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/01/2002")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(520.0, asset.getValues(Date("01/07/2002")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/07/2002")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(10.0, asset.getValues(Date("01/01/2003")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/01/2003")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(10.0, asset.getValues(Date("01/01/2010")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/01/2010")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(10.0, asset.getValues(Date("01/07/2020")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("01/07/2020")).recovery.getValue(), EPSILON);

  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("01/07/2021")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0, asset.getValues(Date("01/07/2021")).recovery.getValue(), EPSILON);
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
          <values at='01/01/2000' exposure='570.0' recovery='80%' />\n\
          <values at='01/01/2000' exposure='560.0' recovery='80%' />\n\
          <values at='01/01/2001' exposure='550.0' recovery='80%' />\n\
          <values at='01/07/2001' exposure='540.0' recovery='80%' />\n\
          <values at='01/01/2002' exposure='530.0' recovery='80%' />\n\
          <values at='01/07/2002' exposure='520.0' recovery='80%' />\n\
          <values at='01/07/2020' exposure='10.0' recovery='80%' />\n\
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
          <values at='-5Y' exposure='570.0' recovery='80%' />\n\
          <values at='01/07/2000' exposure='560.0' recovery='80%' />\n\
          <values at='01/01/2001' exposure='550.0' recovery='80%' />\n\
          <values at='01/07/2001' exposure='540.0' recovery='80%' />\n\
          <values at='01/01/2002' exposure='530.0' recovery='80%' />\n\
          <values at='01/07/2002' exposure='520.0' recovery='80%' />\n\
          <values at='01/07/2020' exposure='10.0' recovery='80%' />\n\
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
// test4.
//===========================================================================
void ccruncher_test::AssetTest::test4()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='01/01/2007'>\n\
        <data>\n\
           <values at='15D' exposure='+1000.0' />\n\
           <values at='15/01/2008' exposure='+1000.0' recovery='80%' />\n\
           <values at='15/01/2009' exposure='+1000.0' />\n\
           <values at='15/01/2010' exposure='+1000.0' recovery='80%' />\n\
           </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  
  Interest interest = getInterest();
  asset.prepare(Date("01/01/2005"), Date("01/01/2011"), interest);
  
  // doing assertions
  ASSERT(asset.hasObligorRecovery());
  ASSERT(!Recovery::valid(asset.getValues(Date("10/01/2007")).recovery));
  ASSERT(!Recovery::valid(asset.getValues(Date("15/01/2007")).recovery));
  ASSERT(Recovery::valid(asset.getValues(Date("10/01/2008")).recovery));
  ASSERT(!Recovery::valid(asset.getValues(Date("10/01/2009")).recovery));
  ASSERT(!Recovery::valid(asset.getValues(Date("15/01/2009")).recovery));
  ASSERT(Recovery::valid(asset.getValues(Date("10/01/2010")).recovery));
}

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::AssetTest::test5()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='10/01/2005' recovery='50%'>\n\
        <data>\n\
           <values at='+5D' exposure='5000.0' recovery='30%' />\n\
           <values at='15/01/2006' exposure='5000.0' recovery='40%' />\n\
           <values at='15/01/2007' exposure='4000.0' recovery='50%' />\n\
           <values at='15/01/2008' exposure='3000.0' recovery='60%' />\n\
           <values at='15/01/2009' exposure='2000.0' recovery='70%' />\n\
           <values at='15/01/2010' exposure='1000.0' recovery='80%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interest interest = getInterest();

  // asset object creation
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  
  ASSERT(!asset.hasObligorRecovery());

  Date time0 = Date("1/1/2004");
  Date timeT = Date("1/7/2011");
  ASSERT_NO_THROW(asset.prepare(time0, timeT, interest));
  
  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("01/01/2004")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(5000.0, asset.getValues(Date("12/01/2005")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.3, asset.getValues(Date("12/01/2005")).recovery.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(5000.0, asset.getValues(Date("01/07/2005")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.4, asset.getValues(Date("01/07/2005")).recovery.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(1000.0, asset.getValues(Date("10/01/2010")).exposure.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.8, asset.getValues(Date("10/01/2010")).recovery.getValue(), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0, asset.getValues(Date("16/01/2010")).exposure.getValue(), EPSILON);
}

//===========================================================================
// test6 (datavalues not found)
//===========================================================================
void ccruncher_test::AssetTest::test6()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='10/01/2005' recovery='60%'>\n\
        <data>\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interest interest = getInterest();

  // asset object creation
  Asset asset(&segs);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &asset));
}

//===========================================================================
// test7. crash test (recovery not found)
//===========================================================================
void ccruncher_test::AssetTest::test7()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0003'/>\n\
        <data>\n\
          <values at='1Y' exposure='570.0' />\n\
          <values at='01/07/2000' exposure='560.0' />\n\
          <values at='2Y' exposure='550.0' />\n\
          <values at='01/07/2001' exposure='540.0' />\n\
          <values at='3Y' exposure='530.0' />\n\
          <values at='01/07/2002' exposure='520.0' />\n\
          <values at='01/07/2020' exposure='10.0' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();

  // asset object creation
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  ASSERT(asset.hasObligorRecovery());
}

//===========================================================================
// test8 (getValues)
//===========================================================================
void ccruncher_test::AssetTest::test8()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0003'/>\n\
        <data>\n\
          <values at='01/01/2000' exposure='570.0' recovery='80%' />\n\
          <values at='01/07/2000' exposure='560.0' recovery='80%' />\n\
          <values at='01/01/2001' exposure='550.0' recovery='80%' />\n\
          <values at='01/07/2001' exposure='540.0' recovery='80%' />\n\
          <values at='01/01/2002' exposure='530.0' recovery='80%' />\n\
          <values at='01/07/2002' exposure='520.0' recovery='80%' />\n\
          <values at='01/07/2020' exposure='10.0' recovery='80%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interest interest = getInterest();

  // asset object creation
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  Date time0 = Date("1/1/1997");
  Date timeT = Date("1/7/2040");
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
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0003'/>\n\
        <data>\n\
          <values at='01/01/2000' exposure='570.0' recovery='80%' />\n\
          <values at='01/07/2000' exposure='560.0' recovery='80%' />\n\
          <values at='01/01/2001' exposure='550.0' recovery='80%' />\n\
          <values at='01/07/2001' exposure='540.0' recovery='80%' />\n\
          <values at='01/01/2002' exposure='530.0' recovery='80%' />\n\
          <values at='01/07/2002' exposure='520.0' recovery='80%' />\n\
          <values at='01/07/2020' exposure='10.0' recovery='80%' />\n\
        </data>\n\
      </asset>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations object creation
  Segmentations segs = getSegmentations();
  Interest interest = getInterest();

  // asset object creation
  Asset asset(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &asset));
  Date time0 = Date("1/1/2004");
  Date timeT = Date("1/7/2011");
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

