
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
// AggregatorsTest.cpp - AggregatorsTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#include <iostream>
#include "aggregators/Aggregator.hpp"
#include "aggregators/Aggregators.hpp"
#include "aggregators/AggregatorsTest.hpp"
#include "utils/ExpatParser.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00001

//===========================================================================
// setUp
//===========================================================================
void AggregatorsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void AggregatorsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// getSegmentations1
//===========================================================================
Segmentations AggregatorsTest::getSegmentations()
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
  Segmentations ret;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &ret));

  return ret;
}

//===========================================================================
// test1
//===========================================================================
void AggregatorsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
  <aggregators>\n\
    <aggregator name='aggregator1' segmentation='portfolio' type='values' full='true'/>\n\
    <aggregator name='aggregator2' segmentation='client' type='values' full='false'/>\n\
    <aggregator name='aggregator3' segmentation='client' type='ratings' full='false'/>\n\
    <aggregator name='aggregator4' segmentation='sector' type='values' full='false'/>\n\
    <aggregator name='aggregator5' segmentation='sector' type='ratings' full='false'/>\n\
  </aggregators>";

  // creating xml
  ExpatParser xmlparser;

  // segmentations creation object
  Segmentations segs = getSegmentations();

  // aggregators creation object
  Aggregators isobj(&segs);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &isobj));

  vector<Aggregator> *v = isobj.getAggregators();

  ASSERT((*v)[0].getName() == "aggregator1");
  ASSERT((*v)[1].getName() == "aggregator2");
  ASSERT((*v)[2].getName() == "aggregator3");
  ASSERT((*v)[3].getName() == "aggregator4");
  ASSERT((*v)[4].getName() == "aggregator5");

  ASSERT_EQUALS(isobj.getNumSegments(), 9L);
}