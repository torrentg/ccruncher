
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
// IDataTest.cpp - IDataTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/03/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added logger
//
// 2005/04/03 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat

//===========================================================================

#include <iostream>
#include "kernel/IData.hpp"
#include "kernel/IDataTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// setUp
//===========================================================================
void IDataTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void IDataTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void IDataTest::test1()
{
  // simple creditcruncher XML
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
  <creditcruncher>\n\
    <params>\n\
      <property name='time.begindate' value='18/02/2003'/>\n\
      <property name='time.steps' value='12'/>\n\
      <property name='time.steplength' value='2'/>\n\
      <property name='stopcriteria.maxiterations' value='3000'/>\n\
      <property name='stopcriteria.maxseconds' value='30000000'/>\n\
      <property name='copula.type' value='normal'/>\n\
      <property name='copula.seed' value='38765874'/>\n\
      <property name='montecarlo.antithetic' value='true'/>\n\
    </params>\n\
    <interests>\n\
      <interest name='spot' date='18/02/2003'>\n\
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
    </interests>\n\
    <ratings>\n\
      <rating name='A' order='1' desc='muy bueno'/>\n\
      <rating name='B' order='2' desc='bueno'/>\n\
      <rating name='D' order='4' desc='malo'/>\n\
      <rating name='C' order='3' desc='regular'/>\n\
      <rating name='E' order='5' desc='fallido'/>\n\
    </ratings>\n\
    <mtransitions period='1' epsilon='1e-12'>\n\
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
    <sectors>\n\
      <sector name='S1' order='1' desc='calzado'/>\n\
      <sector name='S2' order='2' desc='otros sectores'/>\n\
    </sectors>\n\
    <mcorrels epsilon='1e-12'>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.05'/>\n\
      <sigma sector1='S2' sector2='S2' value='0.3'/>\n\
    </mcorrels>\n\
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
    </segmentations>\n\
    <aggregators>\n\
      <aggregator name='aggregator1' segmentation='portfolio' \n\
                  type='values' full='true'/>\n\
      <aggregator name='aggregator2' segmentation='client' \n\
                  type='values' full='false'/>\n\
      <aggregator name='aggregator3' segmentation='client' \n\
                  type='ratings' full='false'/>\n\
      <aggregator name='aggregator4' segmentation='sector' \n\
                  type='values' full='false'/>\n\
      <aggregator name='aggregator5' segmentation='sector' \n\
                  type='ratings' full='false'/>\n\
    </aggregators>\n\
    <portfolio>\n\
      <client rating='A' sector='S2' name='cliente1' id='cif1'>\n\
        <asset name='generic' id='op1'>\n\
          <belongs-to concept='product' segment='bond'/>\n\
          <belongs-to concept='office' segment='0001'/>\n\
          <data>\n\
            <values at='01/01/2000' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
            <values at='01/07/2000' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
            <values at='01/01/2001' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
            <values at='01/07/2001' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
            <values at='01/01/2002' cashflow='10.0' exposure='500.0' recovery='450.0' />\n\
            <values at='01/07/2002' cashflow='510.0' exposure='500.0' recovery='450.0' />\n\
          </data>\n\
        </asset>\n\
        <asset name='generic' id='op2'>\n\
          <belongs-to concept='product' segment='bond'/>\n\
          <belongs-to concept='office' segment='0001'/>\n\
          <data>\n\
            <values at='01/01/2001' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
            <values at='01/07/2001' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
            <values at='01/01/2002' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
            <values at='01/07/2002' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
            <values at='01/01/2003' cashflow='15.0' exposure='500.0' recovery='400.0' />\n\
            <values at='01/07/2003' cashflow='515.0' exposure='500.0' recovery='400.0' />\n\
          </data>\n\
        </asset>\n\
      </client>\n\
    </portfolio>\n\
  </creditcruncher>";

  // creating xml
  ExpatParser xmlparser;

  // client creation
  IData idata;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &idata));

  // assertions
  ASSERT(idata.params != NULL);
  ASSERT(idata.interests != NULL);
  ASSERT(idata.ratings != NULL);
  ASSERT(idata.transitions != NULL);
  ASSERT(idata.sectors != NULL);
  ASSERT(idata.correlations != NULL);
  ASSERT(idata.portfolio != NULL);
}
