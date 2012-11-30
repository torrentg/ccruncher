
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
#include "kernel/IData.hpp"
#include "kernel/IDataTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::IDataTest::test1()
{
  // simple creditcruncher XML
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
  <ccruncher>\n\
    <title>set your title here</title>\n\
    <description><![CDATA[set your description here]]></description>\n\
    <parameters>\n\
      <parameter name='time.0' value='18/02/2003'/>\n\
      <parameter name='time.T' value='18/02/2005'/>\n\
      <parameter name='stopcriteria.maxiterations' value='3000'/>\n\
      <parameter name='stopcriteria.maxseconds' value='30000000'/>\n\
      <parameter name='copula.type' value='gaussian'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='montecarlo.antithetic' value='true'/>\n\
    </parameters>\n\
    <interest type='compound'>\n\
      <rate t='0M' r='0.0'/>\n\
      <rate t='1M' r='0.04'/>\n\
      <rate t='2M' r='0.041'/>\n\
      <rate t='3M' r='0.045'/>\n\
      <rate t='6M' r='0.0455'/>\n\
      <rate t='1Y' r='0.048'/>\n\
      <rate t='2Y' r='0.049'/>\n\
      <rate t='5Y' r='0.05'/>\n\
      <rate t='10Y' r='0.052'/>\n\
    </interest>\n\
    <ratings>\n\
      <rating name='A' description='very good'/>\n\
      <rating name='B' description='good'/>\n\
      <rating name='D' description='bad'/>\n\
      <rating name='C' description='very bad'/>\n\
      <rating name='E' description='default'/>\n\
    </ratings>\n\
    <transitions period='12'>\n\
      <transition from='A' to='A' value='0.80'/>\n\
      <transition from='A' to='B' value='0.10'/>\n\
      <transition from='A' to='C' value='0.07'/>\n\
      <transition from='A' to='D' value='0.02'/>\n\
      <transition from='A' to='E' value='0.01'/>\n\
      <transition from='B' to='A' value='0.05'/>\n\
      <transition from='B' to='B' value='0.75'/>\n\
      <transition from='B' to='C' value='0.10'/>\n\
      <transition from='B' to='D' value='0.07'/>\n\
      <transition from='B' to='E' value='0.03'/>\n\
      <transition from='C' to='A' value='0.03'/>\n\
      <transition from='C' to='B' value='0.07'/>\n\
      <transition from='C' to='C' value='0.70'/>\n\
      <transition from='C' to='D' value='0.14'/>\n\
      <transition from='C' to='E' value='0.06'/>\n\
      <transition from='D' to='A' value='0.05'/>\n\
      <transition from='D' to='B' value='0.05'/>\n\
      <transition from='D' to='C' value='0.15'/>\n\
      <transition from='D' to='D' value='0.60'/>\n\
      <transition from='D' to='E' value='0.15'/>\n\
      <transition from='E' to='A' value='0.00'/>\n\
      <transition from='E' to='B' value='0.00'/>\n\
      <transition from='E' to='C' value='0.00'/>\n\
      <transition from='E' to='D' value='0.00'/>\n\
      <transition from='E' to='E' value='1.00'/>\n\
    </transitions>\n\
    <dprobs>\n\
      <dprob rating='A' t='0M' value='0.0'/>\n\
      <dprob rating='B' t='0M' value='0.0'/>\n\
      <dprob rating='C' t='0M' value='0.0'/>\n\
      <dprob rating='D' t='0M' value='0.0'/>\n\
      <dprob rating='E' t='0M' value='1.0'/>\n\
      <dprob rating='A' t='12M' value='1%'/>\n\
      <dprob rating='B' t='12M' value='2%'/>\n\
      <dprob rating='C' t='12M' value='3%'/>\n\
      <dprob rating='D' t='12M' value='4%'/>\n\
      <dprob rating='E' t='12M' value='100%'/>\n\
    </dprobs>\n\
    <factors>\n\
      <factor name='S1' description='retail'/>\n\
      <factor name='S2' description='others'/>\n\
    </factors>\n\
    <correlations>\n\
      <correlation factor1='S1' factor2='S1' value='0.25'/>\n\
      <correlation factor1='S1' factor2='S2' value='0.05'/>\n\
      <correlation factor1='S2' factor2='S2' value='0.3'/>\n\
    </correlations>\n\
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
    </segmentations>\n\
    <portfolio>\n\
      <obligor rating='A' factor='S2' id='cif1'>\n\
        <asset id='op1' date='01/01/1999'>\n\
          <belongs-to segmentation='products' segment='bond'/>\n\
          <belongs-to segmentation='offices' segment='0001'/>\n\
          <data>\n\
            <values t='01/01/2000' exposure='560.0' recovery='80%' />\n\
            <values t='01/07/2000' exposure='550.0' recovery='80%' />\n\
            <values t='01/01/2001' exposure='540.0' recovery='80%' />\n\
            <values t='01/07/2001' exposure='530.0' recovery='80%' />\n\
            <values t='01/01/2002' exposure='520.0' recovery='80%' />\n\
            <values t='01/07/2002' exposure='510.0' recovery='80%' />\n\
          </data>\n\
        </asset>\n\
        <asset id='op2' date='01/01/2000'>\n\
          <belongs-to segmentation='products' segment='bond'/>\n\
          <belongs-to segmentation='offices' segment='0001'/>\n\
          <data>\n\
            <values t='01/01/2001' exposure='590.0' recovery='70%' />\n\
            <values t='01/07/2001' exposure='575.0' recovery='70%' />\n\
            <values t='01/01/2002' exposure='560.0' recovery='70%' />\n\
            <values t='01/07/2002' exposure='545.0' recovery='70%' />\n\
            <values t='01/01/2003' exposure='530.0' recovery='70%' />\n\
            <values t='01/07/2003' exposure='515.0' recovery='70%' />\n\
          </data>\n\
        </asset>\n\
      </obligor>\n\
    </portfolio>\n\
  </ccruncher>";

  // creating xml
  ExpatParser xmlparser;

  // obligor creation
  IData idata;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &idata));
  ASSERT_EQUALS("set your title here", idata.getTitle());
  ASSERT_EQUALS("set your description here", idata.getDescription());
}
