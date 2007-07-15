
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
// ClientTest.cpp - ClientTest code - $Rev$
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
// 2005/04/03 - Gerard Torrent [gerard@fobos.generacio.com]
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
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Client refactoring
//
// 2006/01/05 - Gerard Torrent [gerard@fobos.generacio.com]
//   . netting replaced by recovery
//
// 2007/07/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed rating.order tag
//   . removed sector.order tag
//
//===========================================================================

#include <iostream>
#include "portfolio/Client.hpp"
#include "portfolio/ClientTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::ClientTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::ClientTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::ClientTest::getRatings()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <ratings>\n\
      <rating name='A' desc='muy bueno'/>\n\
      <rating name='B' desc='bueno'/>\n\
      <rating name='D' desc='malo'/>\n\
      <rating name='C' desc='regular'/>\n\
      <rating name='E' desc='fallido'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ratings));

  return ratings;
}

//===========================================================================
// getSectors
//===========================================================================
Sectors ccruncher_test::ClientTest::getSectors()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <sectors>\n\
      <sector name='S1' desc='calzado'/>\n\
      <sector name='S2' desc='otros sectores'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sectors));

  return sectors;
}

//===========================================================================
// getSegmentations1
//===========================================================================
Segmentations ccruncher_test::ClientTest::getSegmentations()
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
  Segmentations segmentations;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &segmentations));

  return segmentations;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::ClientTest::test1()
{
  // client definition
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <client rating='A' sector='S2' name='cliente1' id='cif1'>\n\
      <belongs-to segmentation='sector' segment='S2'/>\n\
      <belongs-to segmentation='size' segment='big'/>\n\
      <asset name='generic' id='op1'>\n\
        <belongs-to segmentation='product' segment='bond'/>\n\
        <belongs-to segmentation='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2'>\n\
        <belongs-to segmentation='product' segment='bond'/>\n\
        <belongs-to segmentation='office' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </client>";

  // creating xml
  ExpatParser xmlparser;

  // client creation
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  Interests interests;
  Client client(ratings, sectors, segmentations, interests);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &client));

  // assertions
  ASSERT(client.id == "cif1");
  ASSERT(client.name == "cliente1");
  ASSERT(client.irating == 0);
  ASSERT(client.isector == 1);

  ASSERT(client.belongsTo(1, 1));
  ASSERT(client.belongsTo(3, 2));
  ASSERT(client.belongsTo(4, 1));

  vector<Asset> &assets = client.getAssets();

  ASSERT(2 == assets.size());
  ASSERT(assets[0].getId() == "op1");
  ASSERT(assets[1].getId() == "op2");
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::ClientTest::test2()
{
  // client definition with invalid rating
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <client rating='K' sector='S2' name='cliente1' id='cif1'>\n\
      <asset name='generic' id='op1'>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </client>";

  // creating xml
  ExpatParser xmlparser;

  // client creation
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  Interests interests;
  Client client(ratings, sectors, segmentations, interests);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &client));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::ClientTest::test3()
{
  // client definition with invalid asset (data repeated)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <client rating='A' sector='S2' name='cliente1' id='cif1'>\n\
      <asset name='generic' id='op1'>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </client>";

  // creating xml
  ExpatParser xmlparser;

  // client creation
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  Interests interests;
  Client client(ratings, sectors, segmentations, interests);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &client));
}
