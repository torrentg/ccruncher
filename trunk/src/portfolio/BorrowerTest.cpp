
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#include "portfolio/Borrower.hpp"
#include "portfolio/BorrowerTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::BorrowerTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::BorrowerTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::BorrowerTest::getRatings()
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
// getSectors
//===========================================================================
Sectors ccruncher_test::BorrowerTest::getSectors()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <sectors>\n\
      <sector name='S1' description='retail'/>\n\
      <sector name='S2' description='others'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sectors));

  return sectors;
}

//===========================================================================
// getSegmentations
//===========================================================================
Segmentations ccruncher_test::BorrowerTest::getSegmentations()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
  <segmentations>\n\
    <segmentation name='portfolio' components='asset'/>\n\
    <segmentation name='borrowers' components='borrower'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='assets' components='asset'>\n\
      <segment name='*'/>\n\
    </segmentation>\n\
    <segmentation name='sectors' components='borrower'>\n\
      <segment name='S1'/>\n\
      <segment name='S2'/>\n\
    </segmentation>\n\
    <segmentation name='size' components='borrower'>\n\
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
Interest ccruncher_test::BorrowerTest::getInterest()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
      <interest date='18/02/2003'>\n\
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

  // interest object creation
  Interest ret;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ret));

  return ret;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::BorrowerTest::test1()
{
  // borrower definition
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <borrower rating='A' sector='S2' name='Borrower1' id='cif1'>\n\
      <belongs-to segmentation='sectors' segment='S2'/>\n\
      <belongs-to segmentation='size' segment='big'/>\n\
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2' date='01/01/2000'>\n\
        <belongs-to segmentation='products' segment='bond'/>\n\
        <belongs-to segmentation='offices' segment='0001'/>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </borrower>";

  // creating xml
  ExpatParser xmlparser;

  // borrower creation
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest();
  Borrower borrower(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));

  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &borrower));

  // assertions
  ASSERT(borrower.id == "cif1");
  ASSERT(borrower.name == "Borrower1");
  ASSERT(borrower.irating == 0);
  ASSERT(borrower.isector == 1);

  ASSERT(borrower.belongsTo(1, 1));
  ASSERT(borrower.belongsTo(3, 2));
  ASSERT(borrower.belongsTo(4, 1));

  vector<Asset*> &assets = borrower.getAssets();

  ASSERT(2 == assets.size());
  ASSERT(assets[0]->getId() == "op1");
  ASSERT(assets[1]->getId() == "op2");
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::BorrowerTest::test2()
{
  // borrower definition with invalid rating
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <borrower rating='K' sector='S2' name='Borrower1' id='cif1'>\n\
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2' date='01/01/2000'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </borrower>";

  // creating xml
  ExpatParser xmlparser;

  // borrower creation
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest();
  Borrower borrower(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_THROW(xmlparser.parse(xmlcontent, &borrower));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::BorrowerTest::test3()
{
  // borrower definition with invalid asset (data repeated)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <borrower rating='A' sector='S2' name='borrower1' id='cif1'>\n\
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2000' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2001' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/01/2002' cashflow='10.0' recovery='450.0' />\n\
          <values at='01/07/2002' cashflow='510.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
      <asset name='generic' id='op2' date='01/01/2000'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2001' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2002' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/01/2003' cashflow='15.0' recovery='400.0' />\n\
          <values at='01/07/2003' cashflow='515.0' recovery='400.0' />\n\
        </data>\n\
      </asset>\n\
    </borrower>";

  // creating xml
  ExpatParser xmlparser;

  // borrower creation
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest();
  Borrower borrower(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_THROW(xmlparser.parse(xmlcontent, &borrower));
}
