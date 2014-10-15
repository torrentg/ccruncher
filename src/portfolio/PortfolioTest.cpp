
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#include "portfolio/Portfolio.hpp"
#include "portfolio/PortfolioTest.hpp"
#include "utils/ExpatParser.hpp"
#include "params/Ratings.hpp"
#include "params/Factors.hpp"

#define EPSILON 1e-14

using namespace std;
using namespace ccruncher;

//===========================================================================
// getSegmentations
//===========================================================================
Segmentations ccruncher_test::PortfolioTest::getSegmentations()
{
  Segmentations segmentations;
  segmentations.push_back(Segmentation("portfolio", Segmentation::ComponentsType::asset));
  segmentations.push_back(Segmentation("sectors", Segmentation::ComponentsType::obligor));
  segmentations.back().addSegment("S1");
  segmentations.back().addSegment("S2");
  segmentations.push_back(Segmentation("size", Segmentation::ComponentsType::obligor));
  segmentations.back().addSegment("big");
  segmentations.back().addSegment("medium");
  segmentations.push_back(Segmentation("products", Segmentation::ComponentsType::asset));
  segmentations.back().addSegment("bond");
  segmentations.push_back(Segmentation("offices", Segmentation::ComponentsType::asset));
  segmentations.back().addSegment("0001");
  segmentations.back().addSegment("0002");
  segmentations.back().addSegment("0003");
  segmentations.back().addSegment("0004");
  return segmentations;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::PortfolioTest::test1()
{
  // simple CCruncher XML
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='C' factor='S2' id='cif1' lgd='80%'>\n\
        <belongs-to segmentation='sectors' segment='S2'/>\n\
        <asset id='op1' date='01/01/1999' lgd='80%'>\n\
          <belongs-to segmentation='products' segment='bond'/>\n\
          <belongs-to segmentation='offices' segment='0003'/>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
            <values t='01/07/2000' ead='550.0' lgd='20%' />\n\
            <values t='01/01/2001' ead='540.0' />\n\
            <values t='01/07/2001' ead='530.0' lgd='20%' />\n\
            <values t='01/01/2002' ead='520.0' lgd='20%' />\n\
            <values t='01/07/2002' ead='510.0' lgd='20%' />\n\
          </data>\n\
        </asset>\n\
        <asset id='op2' date='01/01/2000'>\n\
          <belongs-to segmentation='offices' segment='0001'/>\n\
          <data>\n\
            <values t='01/01/2001' ead='590.0' lgd='30%' />\n\
            <values t='01/07/2001' ead='575.0' lgd='30%' />\n\
            <values t='01/01/2002' ead='560.0' />\n\
            <values t='01/07/2002' ead='545.0' lgd='30%' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
      <obligor rating='B' factor='S1' id='cif2' lgd='30%'>\n\
        <belongs-to segmentation='size' segment='medium'/>\n\
        <asset id='op3' date='01/07/1999'>\n\
          <belongs-to segmentation='offices' segment='0002'/>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' lgd='25%' />\n\
            <values t='01/01/2001' ead='560.0' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  // previous objects
  Date date1("01/01/1998");
  Date date2("01/01/2050");
  Interest interest(date1);
  Ratings ratings = {"A", "B", "C", "D", "E"};
  Factors factors = {"S1", "S2"};
  Segmentations segmentations = getSegmentations();

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().date1 = date1;
  xmlparser.getUserData().date2 = date2;
  xmlparser.getUserData().interest = &interest;
  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().factors = &factors;
  xmlparser.getUserData().segmentations = &segmentations;

  // portfolio creation
  Portfolio portfolio;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &portfolio));
  const vector<Obligor> &obligors = portfolio.getObligors();
  ASSERT(obligors.size() == 2);

  ASSERT(obligors[0].irating == 2);
  ASSERT(obligors[0].ifactor == 1);
  ASSERT_EQUALS_EPSILON(obligors[0].lgd.getValue1(), 0.8, EPSILON);
  ASSERT(obligors[0].assets.size() == 2);

  ASSERT(obligors[0].assets[0].values.size() == 7);
  ASSERT(obligors[0].assets[0].values[0] == DateValues(Date("01/01/1999"),0.0,0.0));
  ASSERT(obligors[0].assets[0].values[1] == DateValues(Date("01/01/2000"),560.0,0.2));
  ASSERT(obligors[0].assets[0].values[2] == DateValues(Date("01/07/2000"),550.0,0.2));
  ASSERT(obligors[0].assets[0].values[3] == DateValues(Date("01/01/2001"),540.0,0.8)); // lgd inherited from asset
  ASSERT(obligors[0].assets[0].values[4] == DateValues(Date("01/07/2001"),530.0,0.2));
  ASSERT(obligors[0].assets[0].values[5] == DateValues(Date("01/01/2002"),520.0,0.2));
  ASSERT(obligors[0].assets[0].values[6] == DateValues(Date("01/07/2002"),510.0,0.2));
  ASSERT(obligors[0].assets[0].segments.size() == segmentations.size());
  ASSERT(obligors[0].assets[0].segments[0] == 0);
  ASSERT(obligors[0].assets[0].segments[1] == 0);
  ASSERT(obligors[0].assets[0].segments[2] == 1);
  ASSERT(obligors[0].assets[0].segments[3] == 0);
  ASSERT(obligors[0].assets[0].segments[4] == 2);

  ASSERT(obligors[0].assets[1].values.size() == 5);
  ASSERT(obligors[0].assets[1].values[0] == DateValues(Date("01/01/2000"),0.0,0.0));
  ASSERT(obligors[0].assets[1].values[1] == DateValues(Date("01/01/2001"),590.0,0.3));
  ASSERT(obligors[0].assets[1].values[2] == DateValues(Date("01/07/2001"),575.0,0.3));
  ASSERT(obligors[0].assets[1].values[3] == DateValues(Date("01/01/2002"),560.0,0.8)); // lgd inherited from asset
  ASSERT(obligors[0].assets[1].values[4] == DateValues(Date("01/07/2002"),545.0,0.3));
  ASSERT(obligors[0].assets[1].segments.size() == segmentations.size());
  ASSERT(obligors[0].assets[1].segments[0] == 0);
  ASSERT(obligors[0].assets[1].segments[1] == 0);
  ASSERT(obligors[0].assets[1].segments[2] == 1);
  ASSERT(obligors[0].assets[1].segments[3] == 1);
  ASSERT(obligors[0].assets[1].segments[4] == 0);

  ASSERT(obligors[1].irating == 1);
  ASSERT(obligors[1].ifactor == 0);
  ASSERT_EQUALS_EPSILON(obligors[1].lgd.getValue1(), 0.3, EPSILON);
  ASSERT(obligors[1].assets.size() == 1);

  ASSERT(obligors[1].assets[0].values.size() == 3);
  ASSERT(obligors[1].assets[0].values[0] == DateValues(Date("01/07/1999"),0.0,0.0));
  ASSERT(obligors[1].assets[0].values[1] == DateValues(Date("01/01/2000"),560.0,0.25));
  ASSERT(obligors[1].assets[0].values[2] == DateValues(Date("01/01/2001"),560.0,NAN));
  ASSERT(obligors[1].assets[0].segments.size() == segmentations.size());
  ASSERT(obligors[1].assets[0].segments[0] == 0);
  ASSERT(obligors[1].assets[0].segments[1] == 1);
  ASSERT(obligors[1].assets[0].segments[2] == 0);
  ASSERT(obligors[1].assets[0].segments[3] == 1);
  ASSERT(obligors[1].assets[0].segments[4] == 1);
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::PortfolioTest::test2()
{
  // previous objects
  Date date1("01/01/1998");
  Date date2("01/01/2050");
  Interest interest(date1);
  Ratings ratings = {"A", "B", "C", "D", "E"};
  Factors factors = {"S1", "S2"};
  Segmentations segmentations;
  segmentations.push_back(Segmentation("portfolio", Segmentation::ComponentsType::asset));

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().date1 = date1;
  xmlparser.getUserData().date2 = date2;
  xmlparser.getUserData().interest = &interest;
  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().factors = &factors;
  xmlparser.getUserData().segmentations = &segmentations;

  // repeated obligor identifiers
  string xmlcontent1 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='C' factor='S2' id='cif1' lgd='80%'>\n\
        <asset id='op1' date='01/01/1999' lgd='80%'>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
      <obligor rating='B' factor='S1' id='cif1'>\n\
        <asset id='op2' date='01/07/1999'>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' lgd='25%' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  Portfolio portfolio1;
  ASSERT_THROW(xmlparser.parse(xmlcontent1, &portfolio1));

  // repeated asset identifiers
  string xmlcontent2 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='C' factor='S2' id='cif1' lgd='80%'>\n\
        <asset id='op1' date='01/01/1999' lgd='80%'>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
      <obligor rating='B' factor='S1' id='cif2'>\n\
        <asset id='op1' date='01/07/1999'>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' lgd='25%' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  Portfolio portfolio2;
  ASSERT_THROW(xmlparser.parse(xmlcontent2, &portfolio2));

  // malformed xml (lacks data tag)
  string xmlcontent3 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='C' factor='S2' id='cif1' lgd='80%'>\n\
        <asset id='op1' date='01/01/1999' lgd='80%'>\n\
          <values t='01/01/2000' ead='560.0' lgd='20%' />\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  Portfolio portfolio3;
  ASSERT_THROW(xmlparser.parse(xmlcontent3, &portfolio3));

  // datevalue previous or equal to asset creation date
  string xmlcontent4 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='C' factor='S2' id='cif1' lgd='80%'>\n\
        <asset id='op1' date='01/01/1999' lgd='80%'>\n\
          <data>\n\
            <values t='01/01/1999' ead='560.0' lgd='25%' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  Portfolio portfolio4;
  ASSERT_THROW(xmlparser.parse(xmlcontent4, &portfolio4));

  // lgd not set (nor datavalue, nor asset, nor obligor)
  string xmlcontent5 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='C' factor='S2' id='cif1'>\n\
        <asset id='op1' date='01/01/1999'>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  Portfolio portfolio5;
  ASSERT_THROW(xmlparser.parse(xmlcontent5, &portfolio5));

  // unrecognized segmentation
  string xmlcontent6 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='C' factor='S2' id='cif1' lgd='80%'>\n\
        <belongs-to segmentation='XXX' segment='S1'/>\n\
        <asset id='op1' date='01/01/1999'>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  Portfolio portfolio6;
  ASSERT_THROW(xmlparser.parse(xmlcontent6, &portfolio6));

  // unrecognized segment
  string xmlcontent7 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='C' factor='S2' id='cif1' lgd='80%'>\n\
        <belongs-to segmentation='sectors' segment='XXX'/>\n\
        <asset id='op1' date='01/01/1999'>\n\
          <data>\n\
            <values t='01/01/2000' ead='560.0' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  Portfolio portfolio7;
  ASSERT_THROW(xmlparser.parse(xmlcontent7, &portfolio7));
}

//===========================================================================
// test3
// date values after ending date
//===========================================================================
void ccruncher_test::PortfolioTest::test3()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <portfolio>\n\
      <obligor rating='A' factor='S1' id='001'>\n\
        <belongs-to segmentation='sectors' segment='S1'/>\n\
        <asset id='op1' date='01/01/2005'>\n\
          <belongs-to segmentation='offices' segment='0001'/>\n\
          <data>\n\
            <values t='01/01/2007' ead='1.0' lgd='100%' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>";

  // previous objects
  Date date1("01/01/2005");
  Date date2("01/01/2006");
  Interest interest(date1);
  Ratings ratings = {"A", "E"};
  Factors factors = {"S1"};
  Segmentations segmentations = getSegmentations();
  segmentations.back().setEnabled(false);

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().date1 = date1;
  xmlparser.getUserData().date2 = date2;
  xmlparser.getUserData().interest = &interest;
  xmlparser.getUserData().ratings = &ratings;
  xmlparser.getUserData().factors = &factors;
  xmlparser.getUserData().segmentations = &segmentations;

  // portfolio creation
  Portfolio portfolio;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &portfolio));
  const vector<Obligor> &obligors = portfolio.getObligors();
  ASSERT(obligors.size() == 1);
  ASSERT(segmentations.size() == 4);
}

