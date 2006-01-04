
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
      <asset name='generic' id='op1'>\n\
        <belongs-to segmentation='product' segment='bond'/>\n\
        <belongs-to segmentation='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' netting='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' netting='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' netting='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' netting='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' netting='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' netting='450.0' />\n\
          <values at='01/07/2020' cashflow='10.0' netting='10.0' />\n\
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
      <asset name='generic' id='op1'>\n\
        <belongs-to segmentation='product' segment='bond'/>\n\
        <belongs-to segmentation='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' netting='450.0' />\n\
          <values at='01/01/2000' cashflow='10.0' netting='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' netting='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' netting='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' netting='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' netting='450.0' />\n\
          <values at='01/07/2020' cashflow='10.0' netting='10.0' />\n\
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
  ASSERT_EQUALS(7, data.size());

  ASSERT(Date("01/01/2000") == data[0].date);
  ASSERT_DOUBLES_EQUAL(+10.0, data[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(+450.0, data[0].netting, EPSILON);

  ASSERT(Date("01/07/2000") == data[1].date);
  ASSERT_DOUBLES_EQUAL(+10.0 , data[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(+450.0, data[1].netting, EPSILON);

  ASSERT(Date("01/01/2001") == data[2].date);
  ASSERT_DOUBLES_EQUAL(+10.0 , data[2].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(+450.0, data[2].netting, EPSILON);

  ASSERT(Date("01/07/2001") == data[3].date);
  ASSERT_DOUBLES_EQUAL(+10.0 , data[3].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(+450.0, data[3].netting, EPSILON);

  ASSERT(Date("01/01/2002") == data[4].date);
  ASSERT_DOUBLES_EQUAL(+10.0 , data[4].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(+450.0, data[4].netting, EPSILON);

  ASSERT(Date("01/07/2002") == data[5].date);
  ASSERT_DOUBLES_EQUAL(+510.0, data[5].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(+450.0, data[5].netting, EPSILON);

  ASSERT(Date("01/07/2020") == data[6].date);
  ASSERT_DOUBLES_EQUAL(+10.0, data[6].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(+10.0, data[6].netting, EPSILON);

  DateValues *vertexes = new DateValues[4];
  Date dates[] = { Date("1/1/1999"), Date("1/1/2000"), Date("1/6/2002"), Date("1/1/2010") };
  ASSERT_NO_THROW(asset->getVertexes(dates, 4, interests, vertexes));

  ASSERT(Date("01/01/1999") == vertexes[0].date);
  ASSERT_DOUBLES_EQUAL(0.0   , vertexes[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(0.0   , vertexes[0].netting, EPSILON);

  ASSERT(Date("01/01/2000") == vertexes[1].date);
  ASSERT_DOUBLES_EQUAL(10.0  , vertexes[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(450.0 , vertexes[1].netting, EPSILON);

  ASSERT(Date("01/06/2002") == vertexes[2].date);
  ASSERT_DOUBLES_EQUAL(50.0  , vertexes[2].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(450.0 , vertexes[2].netting, EPSILON);

  ASSERT(Date("01/01/2010") == vertexes[3].date);
  ASSERT_DOUBLES_EQUAL(570.0 , vertexes[3].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(266.572 , vertexes[3].netting, EPSILON);

  delete [] vertexes;
}
