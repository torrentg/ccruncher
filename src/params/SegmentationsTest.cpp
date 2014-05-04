
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
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

  // creating xml
  ExpatParser xmlparser;

  // segmentation object creation
  Segmentations sobj;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sobj));

  ASSERT(2 == sobj.size());

  ASSERT(-1 == sobj.indexOfSegmentation("portfolio"));
  ASSERT(0 == sobj.indexOfSegmentation("sectors"));
  ASSERT(-2 == sobj.indexOfSegmentation("size"));
  ASSERT(1 == sobj.indexOfSegmentation("products"));
  ASSERT(-3 == sobj.indexOfSegmentation("offices"));

  ASSERT(sobj.getSegmentation(-1).getName() == "portfolio");
  ASSERT(sobj.getSegmentation(-2).getName() == "size");
  ASSERT(sobj.getSegmentation(-3).getName() == "offices");
  ASSERT(sobj.getSegmentation(0).getName() == "sectors");
  ASSERT(sobj.getSegmentation(1).getName() == "products");

  Asset asset(2);

  asset.setSegment(0, 2);
  asset.setSegment(1, 0);
  sobj.addComponents(&asset);

  asset.setSegment(0, 1);
  asset.setSegment(1, 1);
  sobj.addComponents(&asset);

  sobj.removeUnusedSegments();

  sobj.recodeSegments(&asset);
  ASSERT(asset.getSegment(0) == 0);
  ASSERT(asset.getSegment(1) == 0);
}

