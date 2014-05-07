
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

#include "portfolio/Obligor.hpp"
#include "portfolio/ObligorTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 0.000001

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::ObligorTest::getRatings()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='very good'/>\n\
      <rating name='B' description='good'/>\n\
      <rating name='C' description='bad'/>\n\
      <rating name='D' description='very bad'/>\n\
      <rating name='E' description='defaulted'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ratings));

  return ratings;
}

//===========================================================================
// getFactors
//===========================================================================
Factors ccruncher_test::ObligorTest::getFactors()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <factors>\n\
      <factor name='S1' loading='10%' description='retail'/>\n\
      <factor name='S2' loading='20%' description='others'/>\n\
    </factors>";

  // creating xml
  ExpatParser xmlparser;

  // factors list creation
  Factors factors;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &factors));

  return factors;
}

//===========================================================================
// getSegmentations
//===========================================================================
Segmentations ccruncher_test::ObligorTest::getSegmentations()
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
  Segmentations segmentations;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &segmentations));

  return segmentations;
}

//===========================================================================
// getInterest
//===========================================================================
Interest ccruncher_test::ObligorTest::getInterest(const Date &date)
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

  // interest object creation
  Interest interest(date);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &interest));

  return interest;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::ObligorTest::test1()
{
  // obligor definition
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='A' factor='S2' id='cif1'>\n\
      <belongs-to segmentation='sectors' segment='S2'/>\n\
      <belongs-to segmentation='size' segment='big'/>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0001'/>\n\
        <data>\n\
          <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/07/2000' ead='550.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='530.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='510.0' lgd='20%' />\n\
        </data>\n\
      </asset>\n\
      <asset id='op2' date='01/01/2000'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0001'/>\n\
        <data>\n\
          <values t='01/01/2001' ead='590.0' lgd='30%' />\n\
          <values t='01/07/2001' ead='575.0' lgd='30%' />\n\
          <values t='01/01/2002' ead='560.0' lgd='30%' />\n\
          <values t='01/07/2002' ead='545.0' lgd='30%' />\n\
          <values t='01/01/2003' ead='530.0' lgd='30%' />\n\
          <values t='01/07/2003' ead='515.0' lgd='30%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  // obligor creation
  Date time0("01/01/2000");
  Date timeT("01/01/2005");
  Ratings ratings = getRatings();
  Factors factors = getFactors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest(time0);
  Obligor obligor(5);

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().segmentations = &segmentations;
  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().factors = &factors;
  xmlparser.getUserData().segmentations = &segmentations;
  xmlparser.getUserData().interest = &interest;
  xmlparser.getUserData().date1 = time0;
  xmlparser.getUserData().date2 = timeT;

  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &obligor));

  // assertions
  ASSERT(obligor.id == "cif1");
  ASSERT(obligor.irating == 0);
  ASSERT(obligor.ifactor == 1);
  ASSERT(!LGD::isvalid(obligor.lgd));
  ASSERT(!obligor.hasLGD());

  ASSERT(obligor.belongsTo(1, 2));
  ASSERT(obligor.belongsTo(2, 1));

  vector<Asset*> &assets = obligor.getAssets();

  ASSERT(2 == assets.size());
  ASSERT(assets[0]->getId() == "op1");
  ASSERT(assets[1]->getId() == "op2");
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::ObligorTest::test2()
{
  // obligor definition with invalid rating
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='K' factor='S2'id='cif1'>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <data>\n\
          <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/07/2000' ead='550.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='530.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='510.0' lgd='20%' />\n\
        </data>\n\
      </asset>\n\
      <asset id='op2' date='01/01/2000'>\n\
        <data>\n\
          <values t='01/01/2001' ead='590.0' lgd='30%' />\n\
          <values t='01/07/2001' ead='575.0' lgd='30%' />\n\
          <values t='01/01/2002' ead='560.0' lgd='30%' />\n\
          <values t='01/07/2002' ead='545.0' lgd='30%' />\n\
          <values t='01/01/2003' ead='530.0' lgd='30%' />\n\
          <values t='01/07/2003' ead='515.0' lgd='30%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  // obligor creation
  Date time0("01/01/2000");
  Date timeT("01/01/2005");
  Ratings ratings = getRatings();
  Factors factors = getFactors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest(time0);
  Obligor obligor(5);

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().segmentations = &segmentations;
  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().factors = &factors;
  xmlparser.getUserData().segmentations = &segmentations;
  xmlparser.getUserData().interest = &interest;
  xmlparser.getUserData().date1 = time0;
  xmlparser.getUserData().date2 = timeT;

  ASSERT_THROW(xmlparser.parse(xmlcontent, &obligor));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::ObligorTest::test3()
{
  // obligor definition with invalid asset (data repeated)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='A' factor='S2' id='cif1'>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <data>\n\
          <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/01/2000' ead='550.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='530.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='510.0' lgd='20%' />\n\
        </data>\n\
      </asset>\n\
      <asset id='op2' date='01/01/2000'>\n\
        <data>\n\
          <values t='01/01/2001' ead='590.0' lgd='30%' />\n\
          <values t='01/07/2001' ead='575.0' lgd='30%' />\n\
          <values t='01/01/2002' ead='560.0' lgd='30%' />\n\
          <values t='01/07/2002' ead='545.0' lgd='30%' />\n\
          <values t='01/01/2003' ead='530.0' lgd='30%' />\n\
          <values t='01/07/2003' ead='515.0' lgd='30%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  // obligor creation
  Date time0("01/01/2000");
  Date timeT("01/01/2005");
  Ratings ratings = getRatings();
  Factors factors = getFactors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest(time0);
  Obligor obligor(5);

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().segmentations = &segmentations;
  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().factors = &factors;
  xmlparser.getUserData().segmentations = &segmentations;
  xmlparser.getUserData().interest = &interest;
  xmlparser.getUserData().date1 = time0;
  xmlparser.getUserData().date2 = timeT;

  ASSERT_THROW(xmlparser.parse(xmlcontent, &obligor));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::ObligorTest::test4()
{
  // checks that obligor lgd works
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='A' factor='S2' id='cif1' lgd='50%'>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <data>\n\
          <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
          <values t='01/07/2000' ead='550.0' lgd='20%' />\n\
          <values t='01/01/2001' ead='540.0' lgd='20%' />\n\
          <values t='01/07/2001' ead='530.0' lgd='20%' />\n\
          <values t='01/01/2002' ead='520.0' lgd='20%' />\n\
          <values t='01/07/2002' ead='510.0' lgd='20%' />\n\
        </data>\n\
      </asset>\n\
      <asset id='op2' date='01/01/2000'>\n\
        <data>\n\
          <values t='01/01/2001' ead='590.0' lgd='30%' />\n\
          <values t='01/07/2001' ead='575.0' lgd='30%' />\n\
          <values t='01/01/2002' ead='560.0' lgd='30%' />\n\
          <values t='01/07/2002' ead='545.0' lgd='30%' />\n\
          <values t='01/01/2003' ead='530.0' lgd='30%' />\n\
          <values t='01/07/2003' ead='515.0' lgd='30%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  // obligor creation
  Date time0("01/01/2000");
  Date timeT("01/01/2005");
  Ratings ratings = getRatings();
  Factors factors = getFactors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest(time0);
  Obligor obligor(5);

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().segmentations = &segmentations;
  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().factors = &factors;
  xmlparser.getUserData().segmentations = &segmentations;
  xmlparser.getUserData().interest = &interest;
  xmlparser.getUserData().date1 = time0;
  xmlparser.getUserData().date2 = timeT;

  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &obligor));

  // assertions
  ASSERT_EQUALS_EPSILON(obligor.lgd.getValue(), 0.5, EPSILON);
}

