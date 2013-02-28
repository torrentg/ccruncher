
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#include "params/Segmentation.hpp"
#include "params/SegmentationTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

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

  ASSERT(sobj.isEnabled());
  ASSERT("office" == sobj.name);
  ASSERT(Segmentation::asset == sobj.components);

  ASSERT(5 == sobj.size());

  ASSERT(0 == sobj.indexOfSegment("unassigned"));
  ASSERT(1 == sobj.indexOfSegment("0001"));
  ASSERT(2 == sobj.indexOfSegment("0002"));
  ASSERT(3 == sobj.indexOfSegment("0003"));
  ASSERT(4 == sobj.indexOfSegment("0004"));
}
