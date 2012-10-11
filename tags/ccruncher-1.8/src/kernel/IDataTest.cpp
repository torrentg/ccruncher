
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
#include "kernel/IData.hpp"
#include "kernel/IDataTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::IDataTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::IDataTest::tearDown()
{
  // nothing to do
}

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
      <property name='time.0' value='18/02/2003'/>\n\
      <property name='time.T' value='18/02/2005'/>\n\
      <property name='stopcriteria.maxiterations' value='3000'/>\n\
      <property name='stopcriteria.maxseconds' value='30000000'/>\n\
      <property name='copula.type' value='gaussian'/>\n\
      <property name='copula.seed' value='38765874'/>\n\
      <property name='montecarlo.antithetic' value='true'/>\n\
    </parameters>\n\
    <interest type='compound'>\n\
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
    <ratings>\n\
      <rating name='A' description='very good'/>\n\
      <rating name='B' description='good'/>\n\
      <rating name='D' description='bad'/>\n\
      <rating name='C' description='very bad'/>\n\
      <rating name='E' description='default'/>\n\
    </ratings>\n\
    <mtransitions period='12'>\n\
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
    </mtransitions>\n\
    <survival>\n\
      <svalue rating='A' t='0' value='1.0'/>\n\
      <svalue rating='B' t='0' value='1.0'/>\n\
      <svalue rating='C' t='0' value='1.0'/>\n\
      <svalue rating='D' t='0' value='1.0'/>\n\
      <svalue rating='E' t='0' value='0.0'/>\n\
    </survival>\n\
    <sectors>\n\
      <sector name='S1' description='retail'/>\n\
      <sector name='S2' description='others'/>\n\
    </sectors>\n\
    <mcorrels>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.05'/>\n\
      <sigma sector1='S2' sector2='S2' value='0.3'/>\n\
    </mcorrels>\n\
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
    </segmentations>\n\
    <portfolio>\n\
      <obligor rating='A' sector='S2' id='cif1'>\n\
        <asset id='op1' date='01/01/1999'>\n\
          <belongs-to segmentation='products' segment='bond'/>\n\
          <belongs-to segmentation='offices' segment='0001'/>\n\
          <data>\n\
            <values at='01/01/2000' exposure='560.0' recovery='80%' />\n\
            <values at='01/07/2000' exposure='550.0' recovery='80%' />\n\
            <values at='01/01/2001' exposure='540.0' recovery='80%' />\n\
            <values at='01/07/2001' exposure='530.0' recovery='80%' />\n\
            <values at='01/01/2002' exposure='520.0' recovery='80%' />\n\
            <values at='01/07/2002' exposure='510.0' recovery='80%' />\n\
          </data>\n\
        </asset>\n\
        <asset id='op2' date='01/01/2000'>\n\
          <belongs-to segmentation='products' segment='bond'/>\n\
          <belongs-to segmentation='offices' segment='0001'/>\n\
          <data>\n\
            <values at='01/01/2001' exposure='590.0' recovery='70%' />\n\
            <values at='01/07/2001' exposure='575.0' recovery='70%' />\n\
            <values at='01/01/2002' exposure='560.0' recovery='70%' />\n\
            <values at='01/07/2002' exposure='545.0' recovery='70%' />\n\
            <values at='01/01/2003' exposure='530.0' recovery='70%' />\n\
            <values at='01/07/2003' exposure='515.0' recovery='70%' />\n\
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