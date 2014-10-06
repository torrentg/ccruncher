
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

#include "params/Segmentations.hpp"
#include "params/SegmentationsTest.hpp"
#include "portfolio/Asset.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::SegmentationsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
  <segmentations>\n\
    <segmentation name='portfolio' components='asset' enabled='false'/>\n\
    <segmentation name='sectors' components='obligor' enabled='true'>\n\
      <segment name='S1'/>\n\
      <segment name='S2'/>\n\
    </segmentation>\n\
    <segmentation name='size' components='obligor' enabled='false'>\n\
      <segment name='big'/>\n\
      <segment name='medium'/>\n\
    </segmentation>\n\
    <segmentation name='products' components='asset'>\n\
      <segment name='bond'/>\n\
    </segmentation>\n\
    <segmentation name='offices' components='asset' enabled='false'>\n\
      <segment name='0001'/>\n\
      <segment name='0002'/>\n\
      <segment name='0003'/>\n\
      <segment name='0004'/>\n\
    </segmentation>\n\
  </segmentations>";

  Segmentations sobj;

  // empty list
  ASSERT(!Segmentations::isValid(sobj));

  // parse xml
  ExpatParser xmlparser;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sobj));
  ASSERT(Segmentations::isValid(sobj));

  ASSERT(5 == sobj.size());
  ASSERT(Segmentations::numEnabledSegmentations(sobj) == 2);

  ASSERT(0 == sobj.indexOf("portfolio"));
  ASSERT(1 == sobj.indexOf("sectors"));
  ASSERT(2 == sobj.indexOf("size"));
  ASSERT(3 == sobj.indexOf("products"));
  ASSERT(4 == sobj.indexOf("offices"));

  ASSERT(sobj[0].getName() == "portfolio");
  ASSERT(sobj[1].getName() == "sectors");
  ASSERT(sobj[2].getName() == "size");
  ASSERT(sobj[3].getName() == "products");
  ASSERT(sobj[4].getName() == "offices");

  // repeated segmentation name
  sobj.push_back(Segmentation("offices", Segmentation::ComponentsType::obligor));
  ASSERT(!Segmentations::isValid(sobj));
}

