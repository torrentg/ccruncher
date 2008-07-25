
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
// SegmentationTest.cpp - SegmentationTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . created ccruncher_test namespace
//
//===========================================================================

#include <iostream>
#include "segmentations/Segmentation.hpp"
#include "segmentations/SegmentationTest.hpp"
#include "utils/ExpatParser.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.00001

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::SegmentationTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::SegmentationTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::SegmentationTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <segmentation name='office' components='asset'>\n\
      <segment name='0001'/>\n\
      <segment name='0002'/>\n\
      <segment name='0003'/>\n\
      <segment name='0004'/>\n\
    </segmentation>";

  // creating xml
  ExpatParser xmlparser;

  // correlation matrix creation
  Segmentation sobj;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sobj));

  ASSERT("office" == sobj.name);
  ASSERT(asset == sobj.components);

  vector<Segment> vsegments = sobj.getSegments();
  ASSERT(5 == vsegments.size());

  ASSERT(0 == sobj.getSegment("rest"));
  ASSERT(1 == sobj.getSegment("0001"));
  ASSERT(2 == sobj.getSegment("0002"));
  ASSERT(3 == sobj.getSegment("0003"));
  ASSERT(4 == sobj.getSegment("0004"));
}
