
//***************************************************************************
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
// AssetTest.cpp - AssetTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//***************************************************************************

#include <iostream>
#include "Asset.hpp"
#include "AssetTest.hpp"
#include "utils/Date.hpp"
#include "utils/XMLUtils.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.001

//***************************************************************************
// setUp
//***************************************************************************
void AssetTest::setUp()
{
  XMLUtils::initialize();
}

//***************************************************************************
// setUp
//***************************************************************************
void AssetTest::tearDown()
{
  XMLUtils::terminate();
}

//***************************************************************************
// getSegmentations1
//***************************************************************************
Segmentations AssetTest::getSegmentations()
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
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // segmentation object creation
  Segmentations ret;
  ASSERT_NO_THROW(ret = Segmentations(*(doc->getDocumentElement())));

  delete wis;
  delete parser;
  return ret;
}

//***************************************************************************
// test1
//***************************************************************************
void AssetTest::test1()
{
  Asset *asset = NULL;

  ASSERT_NO_THROW(asset = Asset::getInstanceByClassName("bond"));
  ASSERT_NO_THROW(asset->setId("idop1"));
  ASSERT_NO_THROW(asset->addBelongsTo(2, 1));
  ASSERT_NO_THROW(asset->addBelongsTo(5, 1));
  ASSERT_NO_THROW(asset->addBelongsTo(6, 2));
  ASSERT_NO_THROW(asset->setProperty("issuedate", "1/1/2005"));
  ASSERT_NO_THROW(asset->setProperty("term", "24"));
  ASSERT_NO_THROW(asset->setProperty("nominal", "1000.0"));
  ASSERT_NO_THROW(asset->setProperty("rate", "0.04"));
  ASSERT_NO_THROW(asset->setProperty("ncoupons", "4"));
  ASSERT_NO_THROW(asset->setProperty("adquisitiondate", "1/6/2004"));
  ASSERT_NO_THROW(asset->setProperty("adquisitionprice", "995.0"));

  ASSERT_NO_THROW(asset->initialize());
  
  makeAssertions(asset);

  delete asset;
}

//***************************************************************************
// test1
//***************************************************************************
void AssetTest::test2()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
      <asset class='bond' id='idop1'>\n\
        <belongs-to concept='product' segment='bond'/>\n\
        <belongs-to concept='office' segment='0002'/>\n\
        <property name='issuedate' value='1/1/2005'/>\n\
        <property name='term' value='24'/>\n\
        <property name='nominal' value='1000.0'/>\n\
        <property name='rate' value='0.04'/>\n\
        <property name='ncoupons' value='4'/>\n\
        <property name='adquisitiondate' value='1/6/2004'/>\n\
        <property name='adquisitionprice' value='995.0'/>\n\
      </asset>";
      
  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // segmentations object creation
  Segmentations segs = getSegmentations();
  
  // asset object creation
  Asset *asset = NULL;
  ASSERT_NO_THROW(asset = Asset::parseDOMNode(*(doc->getDocumentElement()), &segs, NULL));

  makeAssertions(asset);
  
  if (asset != NULL) delete asset;
  delete wis;
  delete parser;
}

//***************************************************************************
// makeAssertions
//***************************************************************************
void AssetTest::makeAssertions(Asset *asset)
{  
  ASSERT(asset->validate() == true);
  
  DateValues *events = NULL;
  int size;

  ASSERT(asset->belongsTo(2, 1));
  ASSERT(asset->belongsTo(5, 1));
  ASSERT(asset->belongsTo(6, 2));
  
  ASSERT_NO_THROW(events = asset->getEvents());
  ASSERT_NO_THROW(size = asset->getSize());
  ASSERT_EQUALS(6, size);
  
  ASSERT(Date("01/06/2004") == events[0].date);
  ASSERT_DOUBLES_EQUAL(-995.0, events[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, events[0].exposure, EPSILON);
  
  ASSERT(Date("01/01/2005") == events[1].date);
  ASSERT_DOUBLES_EQUAL(0.0, events[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, events[1].exposure, EPSILON);

  ASSERT(Date("01/07/2005") == events[2].date);
  ASSERT_DOUBLES_EQUAL(20.0, events[2].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, events[2].exposure, EPSILON);

  ASSERT(Date("01/01/2006") == events[3].date);
  ASSERT_DOUBLES_EQUAL(20.0, events[3].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, events[3].exposure, EPSILON);

  ASSERT(Date("01/07/2006") == events[4].date);
  ASSERT_DOUBLES_EQUAL(20.0, events[4].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, events[4].exposure, EPSILON);

  ASSERT(Date("01/01/2007") == events[5].date);
  ASSERT_DOUBLES_EQUAL(1020.0, events[5].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, events[5].exposure, EPSILON);
  
  DateValues *vertexes = new DateValues[asset->getSize()];
  Date dates[] = { Date("1/1/2004"), Date("1/1/2006"), Date("1/8/2006"), Date("1/1/2010") };
  ASSERT_NO_THROW(asset->getVertexes(dates, 4, vertexes));

  ASSERT(Date("01/01/2004") == vertexes[0].date);
  ASSERT_DOUBLES_EQUAL(0.0, vertexes[0].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(0.0, vertexes[0].exposure, EPSILON);
  
  ASSERT(Date("01/01/2006") == vertexes[1].date);
  ASSERT_DOUBLES_EQUAL(-955.0, vertexes[1].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, vertexes[1].exposure, EPSILON);

  ASSERT(Date("01/08/2006") == vertexes[2].date);
  ASSERT_DOUBLES_EQUAL(20.0, vertexes[2].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(995.0, vertexes[2].exposure, EPSILON);

  ASSERT(Date("01/01/2010") == vertexes[3].date);
  ASSERT_DOUBLES_EQUAL(1020.0, vertexes[3].cashflow, EPSILON);
  ASSERT_DOUBLES_EQUAL(121.886, vertexes[3].exposure, EPSILON);

  delete [] vertexes;
}
