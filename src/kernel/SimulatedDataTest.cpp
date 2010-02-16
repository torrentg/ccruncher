
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

#include <iostream>
#include <vector>
#include <algorithm>
#include "utils/ExpatParser.hpp"
#include "kernel/SimulatedData.hpp"
#include "kernel/SimulatedDataTest.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::SimulatedDataTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::SimulatedDataTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::SimulatedDataTest::getRatings()
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
Sectors ccruncher_test::SimulatedDataTest::getSectors()
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
Segmentations ccruncher_test::SimulatedDataTest::getSegmentations()
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
Interest ccruncher_test::SimulatedDataTest::getInterest()
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
// getBorrowers
//===========================================================================
vector<Borrower*> ccruncher_test::SimulatedDataTest::getBorrowers()
{
  string xmlcontent1 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <borrower rating='C' sector='S1' name='Borrower1' id='cif1'>\n\
      <asset name='generic' id='op1' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
    </borrower>";

  string xmlcontent2 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <borrower rating='D' sector='S2' name='Borrower2' id='cif2'>\n\
      <asset name='generic' id='op2' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
    </borrower>";

  string xmlcontent3 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <borrower rating='A' sector='S1' name='Borrower3' id='cif3'>\n\
      <asset name='generic' id='op3' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
    </borrower>";

  string xmlcontent4 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <borrower rating='B' sector='S1' name='Borrower4' id='cif4'>\n\
      <asset name='generic' id='op4' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
    </borrower>";

  string xmlcontent5 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <borrower rating='A' sector='S2' name='Borrower5' id='cif5'>\n\
      <asset name='generic' id='op5' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' cashflow='10.0' recovery='450.0' />\n\
        </data>\n\
      </asset>\n\
    </borrower>";

  Borrower *bp = NULL;
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest();
  ExpatParser xmlparser;
  vector<Borrower*> borrowers;
  
  bp = new Borrower(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent1, bp));
  borrowers.push_back(bp);
  
  bp = new Borrower(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent2, bp));
  borrowers.push_back(bp);
  
  bp = new Borrower(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent3, bp));
  borrowers.push_back(bp);
  
  bp = new Borrower(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent4, bp));
  borrowers.push_back(bp);
  
  bp = new Borrower(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent5, bp));
  borrowers.push_back(bp);

  return borrowers;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::SimulatedDataTest::test1()
{
  vector<Borrower*> borrowers = getBorrowers();
  vector<SimulatedBorrower> sborrowers;

  for(unsigned int i=0; i<borrowers.size(); i++)
  {
    sborrowers.push_back(SimulatedBorrower(borrowers[i]));
  }
  
  ASSERT_EQUALS(sborrowers[0].irating, 2);
  ASSERT_EQUALS(sborrowers[1].irating, 3);
  ASSERT_EQUALS(sborrowers[2].irating, 0);
  ASSERT_EQUALS(sborrowers[3].irating, 1);
  ASSERT_EQUALS(sborrowers[4].irating, 0);

  ASSERT_EQUALS(sborrowers[0].ref->isector, 0);
  ASSERT_EQUALS(sborrowers[1].ref->isector, 1);
  ASSERT_EQUALS(sborrowers[2].ref->isector, 0);
  ASSERT_EQUALS(sborrowers[3].ref->isector, 0);
  ASSERT_EQUALS(sborrowers[4].ref->isector, 1);
  
  sort(sborrowers.begin(), sborrowers.end());

  ASSERT_EQUALS(sborrowers[0].ref->isector, 0);
  ASSERT_EQUALS(sborrowers[1].ref->isector, 0);
  ASSERT_EQUALS(sborrowers[2].ref->isector, 0);
  ASSERT_EQUALS(sborrowers[3].ref->isector, 1);
  ASSERT_EQUALS(sborrowers[4].ref->isector, 1);
  
  ASSERT_EQUALS(sborrowers[0].irating, 0);
  ASSERT_EQUALS(sborrowers[1].irating, 1);
  ASSERT_EQUALS(sborrowers[2].irating, 2);
  ASSERT_EQUALS(sborrowers[3].irating, 0);
  ASSERT_EQUALS(sborrowers[4].irating, 3);

  for(unsigned int i=0; i<borrowers.size(); i++)
  {
    delete borrowers[i];
  }
}

