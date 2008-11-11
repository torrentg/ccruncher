
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
// SegmentationTest.cpp - SegmentationTest code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/07/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@mail.generacio.com]
//   . Segmentation class refactoring
//
//===========================================================================

#include "segmentations/Segmentation.hpp"
#include "segmentations/SegmentationTest.hpp"
#include "utils/ExpatParser.hpp"

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
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
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

  ASSERT(5 == sobj.size());

  ASSERT(0 == sobj["rest"].order);
  ASSERT(0 == sobj[0].order);
  ASSERT(1 == sobj["0001"].order);
  ASSERT(1 == sobj[1].order);
  ASSERT(2 == sobj["0002"].order);
  ASSERT(2 == sobj[2].order);
  ASSERT(3 == sobj["0003"].order);
  ASSERT(3 == sobj[3].order);
  ASSERT(4 == sobj["0004"].order);
  ASSERT(4 == sobj[4].order);
}
