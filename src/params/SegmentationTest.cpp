
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
  Segmentation segmentation("office", Segmentation::Type::asset, true);
  segmentation.addSegment("0001");
  segmentation.addSegment("0002");
  segmentation.addSegment("0003");
  segmentation.addSegment("0004");

  ASSERT(segmentation.isEnabled());
  ASSERT("office" == segmentation.getName());
  ASSERT(Segmentation::Type::asset == segmentation.getType());

  ASSERT(5 == segmentation.size());

  ASSERT(0 == segmentation.indexOf("unassigned"));
  ASSERT(1 == segmentation.indexOf("0001"));
  ASSERT(2 == segmentation.indexOf("0002"));
  ASSERT(3 == segmentation.indexOf("0003"));
  ASSERT(4 == segmentation.indexOf("0004"));
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
  ASSERT_THROW(segmentation.setType("obligors")); // 'obligors' instead of 'obligor'
  segmentation.setType("obligor");
  ASSERT(segmentation.getType() == Segmentation::Type::obligor);
  segmentation.setEnabled(true);

  ASSERT(segmentation.getName() == "portfolio");
  ASSERT_THROW(segmentation.indexOf("XXX"));
  ASSERT(segmentation.isEnabled());
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::SegmentationTest::test3()
{
  vector<Segmentation> segmentations;
  segmentations.push_back(Segmentation("segmentation1",Segmentation::Type::asset, true));
  segmentations.push_back(Segmentation("segmentation2",Segmentation::Type::obligor, false));
  segmentations.push_back(Segmentation("segmentation3",Segmentation::Type::asset, true));
  segmentations.push_back(Segmentation("segmentation4",Segmentation::Type::obligor, false));
  segmentations.push_back(Segmentation("segmentation5",Segmentation::Type::asset, true));
  segmentations.push_back(Segmentation("segmentation6",Segmentation::Type::obligor, true));

  stable_sort(segmentations.begin(), segmentations.end());

  ASSERT(segmentations[0].getName() == "segmentation1");
  ASSERT(segmentations[1].getName() == "segmentation3");
  ASSERT(segmentations[2].getName() == "segmentation5");
  ASSERT(segmentations[3].getName() == "segmentation6");
  ASSERT(segmentations[4].getName() == "segmentation2");
  ASSERT(segmentations[5].getName() == "segmentation4");
}

