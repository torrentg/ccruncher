
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include "params/Sectors.hpp"
#include "params/SectorsTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::SectorsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::SectorsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::SectorsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <sectors>\n\
      <sector name='S1' description='retail'/>\n\
      <sector name='S2' description='others'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors creation
  Sectors sectors;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sectors));

  ASSERT(2 == sectors.size());

  ASSERT_EQUALS(0, sectors.getIndex("S1"));
  ASSERT_EQUALS(1, sectors.getIndex("S2"));

  ASSERT("S1" == sectors[0].name);
  ASSERT("S2" == sectors[1].name);

  ASSERT("retail" == sectors[0].desc);
  ASSERT("others" == sectors[1].desc);

  ASSERT(sectors[0].name == "S1");
  ASSERT(sectors[1].name == "S2");
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::SectorsTest::test2()
{
  // xml with error (repeated sector)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <sectors>\n\
      <sector name='S1' description='retail'/>\n\
      <sector name='S1' description='others'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors creation
  Sectors sectors;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sectors));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::SectorsTest::test3()
{
  // xml with error (repeated description)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <sectors>\n\
      <sector name='S1' description='retail'/>\n\
      <sector name='S2' description='retail'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors creation
  Sectors sectors;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sectors));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::SectorsTest::test4()
{
  // xml with error (tag sectir)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <sectors>\n\
      <sector name='S1' description='retail'/>\n\
      <sectir name='S2' description='others'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors creation
  Sectors sectors;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sectors));
}
