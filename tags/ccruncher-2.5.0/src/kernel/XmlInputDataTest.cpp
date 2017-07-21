
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#include "kernel/XmlInputData.hpp"
#include "kernel/XmlInputDataTest.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-14

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::XmlInputDataTest::test1()
{
  // nominal case
  string xmlcontent = R"XMLCONTENT(<?xml version='1.0' encoding='UTF-8'?>
  <ccruncher>
    <title>sample ccruncher input file</title>
    <description>
      <![CDATA[
      set your additional information here
      ]]>
    </description>
    <defines>
      <define name='numsims' value='30000'/>
      <define name='ndf' value='5'/>
    </defines>
    <parameters>
      <parameter name='time.0' value='18/10/2009'/>
      <parameter name='time.T' value='15/07/2015'/>
      <parameter name='maxiterations' value='$numsims'/>
      <parameter name='maxseconds' value='$maxseconds'/>
      <parameter name='copula' value='t($ndf)'/>
      <parameter name='rng.seed' value='$seed'/>
      <parameter name='antithetic' value='true'/>
      <parameter name='blocksize' value='128'/>
    </parameters>
    <interest type='compound'>
      <rate t='1D' r='3.99%'/>
      <rate t='1M' r='4.0%'/>
      <rate t='2M' r='4.1%'/>
      <rate t='3M' r='4.5%'/>
      <rate t='6M' r='4.55%'/>
      <rate t='1Y' r='4.8%'/>
      <rate t='2Y' r='4.9%'/>
      <rate t='5Y' r='5.0%'/>
      <rate t='10Y' r='5.2%'/>
    </interest>
    <ratings>
      <rating name='A' description='good'/>
      <rating name='B' description='medium'/>
      <rating name='C' description='bad'/>
      <rating name='D' description='in default'/>
    </ratings>
    <transitions period='12'>
      <transition from='A' to='A' value='90.0%' />
      <transition from='A' to='B' value='8.0%' />
      <transition from='A' to='C' value='2.0%' />
      <transition from='A' to='D' value='0.0%' />
      <transition from='B' to='A' value='10.0%' />
      <transition from='B' to='B' value='70.0%' />
      <transition from='B' to='C' value='10.0%' />
      <transition from='B' to='D' value='10.0%' />
      <transition from='C' to='A' value='5.0%' />
      <transition from='C' to='B' value='15.0%' />
      <transition from='C' to='C' value='60.0%' />
      <transition from='C' to='D' value='20.0%' />
      <transition from='D' to='A' value='0.0%' />
      <transition from='D' to='B' value='0.0%' />
      <transition from='D' to='C' value='0.0%' />
      <transition from='D' to='D' value='100%' />
    </transitions>
    <factors>
      <factor name='S1' loading='15.1%' description='Consumer/service sector'/>
      <factor name='S2' loading='17.9%' description='Forest/building products'/>
      <factor name='S3' loading='17.5%' description='High technology'/>
    </factors>
    <correlations>
      <correlation factor1='S1' factor2='S3' value='8.6%'/>
      <correlation factor1='S2' factor2='S3' value='6.3%'/>
    </correlations>
    <segmentations>
      <segmentation name='portfolio'/>
      <segmentation name='sectors'>
        <segment name='S1'/>
        <segment name='S2'/>
        <segment name='S3'/>
      </segmentation>
      <segmentation name='size'>
        <segment name='large'/>
        <segment name='medium'/>
        <segment name='small'/>
      </segmentation>
      <segmentation name='products' enabled='false'>
        <segment name='bond'/>
        <segment name='loan'/>
        <segment name='lease'/>
      </segmentation>
      <segmentation name='offices'>
        <segment name='0001'/>
        <segment name='0002'/>
        <segment name='0003'/>
        <segment name='0004'/>
      </segmentation>
    </segmentations>
    <portfolio>
      <obligor rating='C' factor='S2' id='cif1' lgd='80%'>
        <belongs-to segmentation='sectors' segment='S2'/>
        <asset id='op1' date='01/01/2014' lgd='80%'>
          <belongs-to segmentation='products' segment='bond'/>
          <belongs-to segmentation='offices' segment='0003'/>
          <data>
            <values t='01/07/2014' ead='550.0' lgd='20%' />
            <values t='01/01/2015' ead='540.0' />
            <values t='01/07/2015' ead='530.0' lgd='20%' />
            <values t='01/01/2016' ead='520.0' lgd='20%' />
            <values t='01/07/2016' ead='510.0' lgd='20%' />
          </data>
        </asset>
        <asset id='op2' date='01/01/2013'>
          <belongs-to segmentation='offices' segment='0001'/>
          <data>
            <values t='01/01/2014' ead='590.0' lgd='30%' />
            <values t='01/07/2014' ead='575.0' lgd='30%' />
            <values t='01/01/2015' ead='560.0' />
            <values t='01/07/2015' ead='545.0' lgd='30%' />
          </data>
        </asset>
      </obligor>
      <obligor rating='B' factor='S1' id='cif2'>
        <belongs-to segmentation='size' segment='medium'/>
        <asset id='op3' date='01/07/2012'>
          <belongs-to segmentation='offices' segment='0002'/>
          <data>
            <values t='01/01/2013' ead='560.0' lgd='25%' />
            <values t='01/01/2014' ead='560.0' />
          </data>
        </asset>
      </obligor>
    </portfolio>
  </ccruncher>
  )XMLCONTENT";

  map<string,string> defines;
  defines["maxseconds"] = "3600";
  defines["seed"] = "10";

  XmlInputData input(nullptr);
  ASSERT_NO_THROW(input.readString(xmlcontent.c_str(), defines));

  ASSERT_EQUALS(10UL, input.getParams().getRngSeed());
  ASSERT_EQUALS((size_t)3600, input.getParams().getMaxSeconds());
  ASSERT_EQUALS((size_t)30000, input.getParams().getMaxIterations());
  ASSERT_EQUALS_EPSILON(5.0, input.getParams().getNdf(), EPSILON);
  ASSERT_EQUALS("sample ccruncher input file", input.getTitle());
  ASSERT_EQUALS("set your additional information here", input.getDescription());
  ASSERT_EQUALS(Date("18/10/2009"), input.getParams().getTime0());
  ASSERT_EQUALS(Date("15/07/2015"), input.getParams().getTimeT());
  ASSERT_EQUALS(true, input.getParams().getAntithetic());
  ASSERT_EQUALS((unsigned short)(128), input.getParams().getBlockSize());

  Date time0 = input.getParams().getTime0();

  Interest interest = input.getInterest();
  ASSERT_EQUALS_EPSILON(0.0399, interest.getRate(add(time0, 1 , 'D')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0400, interest.getRate(add(time0, 1 , 'M')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0410, interest.getRate(add(time0, 2 , 'M')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0450, interest.getRate(add(time0, 3 , 'M')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0455, interest.getRate(add(time0, 6 , 'M')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0480, interest.getRate(add(time0, 1 , 'Y')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0490, interest.getRate(add(time0, 2 , 'Y')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0500, interest.getRate(add(time0, 5 , 'Y')), EPSILON);
  ASSERT_EQUALS_EPSILON(0.0520, interest.getRate(add(time0, 10 , 'Y')), EPSILON);

  vector<Rating> ratings = input.getRatings();
  ASSERT_EQUALS("A", ratings[0].name);
  ASSERT_EQUALS("B", ratings[1].name);
  ASSERT_EQUALS("C", ratings[2].name);
  ASSERT_EQUALS("D", ratings[3].name);
  ASSERT_EQUALS("good", ratings[0].description);
  ASSERT_EQUALS("medium", ratings[1].description);
  ASSERT_EQUALS("bad", ratings[2].description);
  ASSERT_EQUALS("in default", ratings[3].description);

  Transitions transitions = input.getTransitions();
  ASSERT_EQUALS(12, transitions.getPeriod());
  ASSERT_EQUALS((unsigned char)3, transitions.getIndexDefault());
  ASSERT_EQUALS_EPSILON(0.90, transitions[0][0], EPSILON);
  ASSERT_EQUALS_EPSILON(0.08, transitions[0][1], EPSILON);
  ASSERT_EQUALS_EPSILON(0.02, transitions[0][2], EPSILON);
  ASSERT_EQUALS_EPSILON(0.00, transitions[0][3], EPSILON);
  ASSERT_EQUALS_EPSILON(0.10, transitions[1][0], EPSILON);
  ASSERT_EQUALS_EPSILON(0.70, transitions[1][1], EPSILON);
  ASSERT_EQUALS_EPSILON(0.10, transitions[1][2], EPSILON);
  ASSERT_EQUALS_EPSILON(0.10, transitions[1][3], EPSILON);
  ASSERT_EQUALS_EPSILON(0.05, transitions[2][0], EPSILON);
  ASSERT_EQUALS_EPSILON(0.15, transitions[2][1], EPSILON);
  ASSERT_EQUALS_EPSILON(0.60, transitions[2][2], EPSILON);
  ASSERT_EQUALS_EPSILON(0.20, transitions[2][3], EPSILON);
  ASSERT_EQUALS_EPSILON(0.00, transitions[3][0], EPSILON);
  ASSERT_EQUALS_EPSILON(0.00, transitions[3][1], EPSILON);
  ASSERT_EQUALS_EPSILON(0.00, transitions[3][2], EPSILON);
  ASSERT_EQUALS_EPSILON(1.00, transitions[3][3], EPSILON);

  vector<Factor> factors = input.getFactors();
  ASSERT_EQUALS("S1", factors[0].name);
  ASSERT_EQUALS("S2", factors[1].name);
  ASSERT_EQUALS("S3", factors[2].name);
  ASSERT_EQUALS("Consumer/service sector", factors[0].description);
  ASSERT_EQUALS("Forest/building products", factors[1].description);
  ASSERT_EQUALS("High technology", factors[2].description);

  vector<double> loadings = input.getFactorLoadings();
  ASSERT_EQUALS_EPSILON(0.151, loadings[0], EPSILON);
  ASSERT_EQUALS_EPSILON(0.179, loadings[1], EPSILON);
  ASSERT_EQUALS_EPSILON(0.175, loadings[2], EPSILON);

  vector<vector<double>> correlations = {
    {1.000, 0.000, 0.086},
    {0.000, 1.000, 0.063},
    {0.086, 0.063, 1.000}};

  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) {
      ASSERT_EQUALS_EPSILON(correlations[i][j], input.getCorrelations()[i][j], EPSILON);
    }
  }

  // note: segmentations pruned by InputData::removeUnusedSegments()
  vector<Segmentation> segmentations = input.getSegmentations();
  ASSERT_EQUALS((size_t)4, segmentations.size());

  ASSERT_EQUALS("portfolio", segmentations[0].getName());
  ASSERT(segmentations[0].isEnabled());
  ASSERT_EQUALS((unsigned short)1, segmentations[0].size());
  ASSERT_EQUALS("portfolio", segmentations[0].getSegment(0));

  ASSERT_EQUALS("sectors", segmentations[1].getName());
  ASSERT(segmentations[1].isEnabled());
  ASSERT_EQUALS((unsigned short)2, segmentations[1].size());
  ASSERT_EQUALS("S2", segmentations[1].getSegment(0));
  ASSERT_EQUALS("unassigned", segmentations[1].getSegment(1));
  ASSERT_EQUALS((unsigned short)0, segmentations[1].indexOfSegment("S2"));
  ASSERT_EQUALS((unsigned short)1, segmentations[1].indexOfSegment("unassigned"));
  ASSERT_THROW(segmentations[1].indexOfSegment("XXX"));

  ASSERT_EQUALS("size", segmentations[2].getName());
  ASSERT(segmentations[2].isEnabled());
  ASSERT_EQUALS((unsigned short)2, segmentations[2].size());
  ASSERT_EQUALS("medium", segmentations[2].getSegment(0));
  ASSERT_EQUALS("unassigned", segmentations[2].getSegment(1));

  ASSERT_EQUALS("offices", segmentations[3].getName());
  ASSERT(segmentations[3].isEnabled());
  ASSERT_EQUALS((unsigned short)3, segmentations[3].size());
  ASSERT_EQUALS("0001", segmentations[3].getSegment(0));
  ASSERT_EQUALS("0002", segmentations[3].getSegment(1));
  ASSERT_EQUALS("0003", segmentations[3].getSegment(2));

  vector<Obligor> &obligors = input.getPortfolio();
  ASSERT_EQUALS((size_t)2, obligors.size());

  ASSERT_EQUALS((unsigned char)2, obligors[0].irating);
  ASSERT_EQUALS((unsigned char)1, obligors[0].ifactor);
  ASSERT_EQUALS((size_t)2, obligors[0].assets.size());
  ASSERT(obligors[0].lgd == LGD(0.8));

  ASSERT_EQUALS((size_t)4, obligors[0].assets[0].segments.size());
  ASSERT_EQUALS((unsigned short)0, obligors[0].assets[0].segments[0]);
  ASSERT_EQUALS((unsigned short)0, obligors[0].assets[0].segments[1]);
  ASSERT_EQUALS((unsigned short)1, obligors[0].assets[0].segments[2]);
  ASSERT_EQUALS((unsigned short)2, obligors[0].assets[0].segments[3]);

  ASSERT_EQUALS((size_t)5, obligors[0].assets[0].values.size());
  ASSERT(DateValues(Date("01/01/2014"), 0.0, 0.0) == obligors[0].assets[0].values[0]);
  ASSERT(DateValues(Date("01/07/2014"), 550.0*interest.getFactor(Date("01/07/2014")), 0.2) == obligors[0].assets[0].values[1]);
  ASSERT(DateValues(Date("01/01/2015"), 540.0*interest.getFactor(Date("01/01/2015")), 0.8) == obligors[0].assets[0].values[2]);
  ASSERT(DateValues(Date("01/07/2015"), 530.0*interest.getFactor(Date("01/07/2015")), 0.2) == obligors[0].assets[0].values[3]);
  ASSERT(DateValues(Date("01/01/2016"), 520.0*interest.getFactor(Date("01/01/2016")), 0.2) == obligors[0].assets[0].values[4]);

  ASSERT_EQUALS((size_t)4, obligors[0].assets[1].segments.size());
  ASSERT_EQUALS((unsigned short)0, obligors[0].assets[1].segments[0]);
  ASSERT_EQUALS((unsigned short)0, obligors[0].assets[1].segments[1]);
  ASSERT_EQUALS((unsigned short)1, obligors[0].assets[1].segments[2]);
  ASSERT_EQUALS((unsigned short)0, obligors[0].assets[1].segments[3]);

  ASSERT_EQUALS((size_t)5, obligors[0].assets[1].values.size());
  ASSERT(DateValues(Date("01/01/2013"), 0.0, 0.0) == obligors[0].assets[1].values[0]);
  ASSERT(DateValues(Date("01/01/2014"), 590.0*interest.getFactor(Date("01/01/2014")), 0.3) == obligors[0].assets[1].values[1]);
  ASSERT(DateValues(Date("01/07/2014"), 575.0*interest.getFactor(Date("01/07/2014")), 0.3) == obligors[0].assets[1].values[2]);
  ASSERT(DateValues(Date("01/01/2015"), 560.0*interest.getFactor(Date("01/01/2015")), 0.8) == obligors[0].assets[1].values[3]);
  ASSERT(DateValues(Date("01/07/2015"), 545.0*interest.getFactor(Date("01/07/2015")), 0.3) == obligors[0].assets[1].values[4]);

  ASSERT_EQUALS((unsigned char)1, obligors[1].irating);
  ASSERT_EQUALS((unsigned char)0, obligors[1].ifactor);
  ASSERT_EQUALS((size_t)1, obligors[1].assets.size());
  ASSERT(obligors[1].lgd == LGD(1.0));

  ASSERT_EQUALS((size_t)4, obligors[1].assets[0].segments.size());
  ASSERT_EQUALS((unsigned short)0, obligors[1].assets[0].segments[0]);
  ASSERT_EQUALS((unsigned short)1, obligors[1].assets[0].segments[1]);
  ASSERT_EQUALS((unsigned short)0, obligors[1].assets[0].segments[2]);
  ASSERT_EQUALS((unsigned short)1, obligors[1].assets[0].segments[3]);

  ASSERT_EQUALS((size_t)3, obligors[1].assets[0].values.size());
  ASSERT(DateValues(Date("01/07/2012"), 0.0, 0.0) == obligors[1].assets[0].values[0]);
  ASSERT(DateValues(Date("01/01/2013"), 560.0*interest.getFactor(Date("01/01/2013")), 0.25) == obligors[1].assets[0].values[1]);
  ASSERT(DateValues(Date("01/01/2014"), 560.0*interest.getFactor(Date("01/01/2014")), 1.0) == obligors[1].assets[0].values[2]);
}

