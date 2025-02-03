
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#include <vector>
#include <algorithm>
#include "params/Segmentation.hpp"
#include "params/SegmentationTest.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::SegmentationTest::test1()
{
  /*
    <segmentation name='office' components='asset'>
      <segment name='0001'/>
      <segment name='0002'/>
      <segment name='0003'/>
      <segment name='0004'/>
    </segmentation>
  */
  Segmentation segmentation("office", true);
  segmentation.addSegment("0001");
  segmentation.addSegment("0002");
  segmentation.addSegment("0003");
  segmentation.addSegment("0004");

  ASSERT(segmentation.isEnabled());
  ASSERT("office" == segmentation.getName());

  ASSERT(5 == segmentation.size());

  ASSERT(0 == segmentation.indexOfSegment("unassigned"));
  ASSERT(1 == segmentation.indexOfSegment("0001"));
  ASSERT(2 == segmentation.indexOfSegment("0002"));
  ASSERT(3 == segmentation.indexOfSegment("0003"));
  ASSERT(4 == segmentation.indexOfSegment("0004"));
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::SegmentationTest::test2()
{
  /*
    <segmentation name='portfolio' components='obligor'/>
  */
  Segmentation segmentation("portfolio");
  segmentation.setEnabled(true);

  ASSERT(segmentation.getName() == "portfolio");
  ASSERT_THROW(segmentation.indexOfSegment("XXX"));
  ASSERT(segmentation.isEnabled());
}

