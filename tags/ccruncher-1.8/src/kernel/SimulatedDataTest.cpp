
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

  // interest object creation
  Interest ret;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ret));

  return ret;
}

//===========================================================================
// getObligors
//===========================================================================
vector<Obligor*> ccruncher_test::SimulatedDataTest::getObligors()
{
  string xmlcontent1 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='C' sector='S1' id='cif1'>\n\
      <asset id='op1' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' exposure='10.0' recovery='80%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  string xmlcontent2 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='D' sector='S2' id='cif2'>\n\
      <asset id='op2' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' exposure='10.0' recovery='80%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  string xmlcontent3 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='A' sector='S1' id='cif3'>\n\
      <asset id='op3' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' exposure='10.0' recovery='80%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  string xmlcontent4 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='B' sector='S1' id='cif4'>\n\
      <asset id='op4' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' exposure='10.0' recovery='80%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  string xmlcontent5 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <obligor rating='A' sector='S2' id='cif5'>\n\
      <asset id='op5' date='01/01/1999'>\n\
        <data>\n\
          <values at='01/01/2001' exposure='10.0' recovery='80%' />\n\
        </data>\n\
      </asset>\n\
    </obligor>";

  Obligor *bp = NULL;
  Ratings ratings = getRatings();
  Sectors sectors = getSectors();
  Segmentations segmentations = getSegmentations();
  Interest interest = getInterest();
  ExpatParser xmlparser;
  vector<Obligor*> obligors;
  
  bp = new Obligor(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent1, bp));
  obligors.push_back(bp);
  
  bp = new Obligor(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent2, bp));
  obligors.push_back(bp);
  
  bp = new Obligor(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent3, bp));
  obligors.push_back(bp);
  
  bp = new Obligor(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent4, bp));
  obligors.push_back(bp);
  
  bp = new Obligor(ratings, sectors, segmentations, interest, Date("01/01/2000"), Date("01/01/2005"));
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent5, bp));
  obligors.push_back(bp);

  return obligors;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::SimulatedDataTest::test1()
{
  vector<Obligor*> obligors = getObligors();
  vector<SimulatedObligor> sobligors;

  for(unsigned int i=0; i<obligors.size(); i++)
  {
    sobligors.push_back(SimulatedObligor(obligors[i]));
  }
  
  ASSERT_EQUALS(sobligors[0].irating, 2);
  ASSERT_EQUALS(sobligors[1].irating, 3);
  ASSERT_EQUALS(sobligors[2].irating, 0);
  ASSERT_EQUALS(sobligors[3].irating, 1);
  ASSERT_EQUALS(sobligors[4].irating, 0);

  ASSERT_EQUALS(sobligors[0].ref->isector, 0);
  ASSERT_EQUALS(sobligors[1].ref->isector, 1);
  ASSERT_EQUALS(sobligors[2].ref->isector, 0);
  ASSERT_EQUALS(sobligors[3].ref->isector, 0);
  ASSERT_EQUALS(sobligors[4].ref->isector, 1);
  
  sort(sobligors.begin(), sobligors.end());

  ASSERT_EQUALS(sobligors[0].ref->isector, 0);
  ASSERT_EQUALS(sobligors[1].ref->isector, 0);
  ASSERT_EQUALS(sobligors[2].ref->isector, 0);
  ASSERT_EQUALS(sobligors[3].ref->isector, 1);
  ASSERT_EQUALS(sobligors[4].ref->isector, 1);
  
  ASSERT_EQUALS(sobligors[0].irating, 0);
  ASSERT_EQUALS(sobligors[1].irating, 1);
  ASSERT_EQUALS(sobligors[2].irating, 2);
  ASSERT_EQUALS(sobligors[3].irating, 0);
  ASSERT_EQUALS(sobligors[4].irating, 3);

  for(unsigned int i=0; i<obligors.size(); i++)
  {
    delete obligors[i];
  }
}

