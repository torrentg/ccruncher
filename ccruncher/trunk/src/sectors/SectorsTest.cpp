
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
// SectorsTest.cpp - SectorsTest code
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
#include "Sectors.hpp"
#include "SectorsTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// setUp
//===========================================================================
void SectorsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void SectorsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void SectorsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <sectors>\n\
      <sector name='S1' order='1' desc='calzado'/>\n\
      <sector name='S2' order='2' desc='otros sectores'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors creation
  Sectors sectors;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sectors));
  vector<Sector> &list = *(sectors.getSectors());

  ASSERT(2 == list.size());

  ASSERT_EQUALS(1, list[0].order);
  ASSERT_EQUALS(2, list[1].order);

  ASSERT("S1" == list[0].name);
  ASSERT("S2" == list[1].name);

  ASSERT("calzado" == list[0].desc);
  ASSERT("otros sectores" == list[1].desc);

  ASSERT(list[0] < list[1]);

  ASSERT(sectors.getName(0) == "S1");
  ASSERT(sectors.getName(1) == "S2");
}

//===========================================================================
// test2
//===========================================================================
void SectorsTest::test2()
{
  // xml with error (order 2 not exist)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <sectors>\n\
      <sector name='S1' order='1' desc='calzado'/>\n\
      <sector name='S2' order='3' desc='otros sectores'/>\n\
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
void SectorsTest::test3()
{
  // xml with error (repeated sector)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <sectors>\n\
      <sector name='S1' order='1' desc='calzado'/>\n\
      <sector name='S1' order='2' desc='otros sectores'/>\n\
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
void SectorsTest::test4()
{
  // xml with error (repeated description)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <sectors>\n\
      <sector name='S1' order='1' desc='calzado'/>\n\
      <sector name='S2' order='2' desc='calzado'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors creation
  Sectors sectors;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sectors));
}

//===========================================================================
// test5
//===========================================================================
void SectorsTest::test5()
{
  // xml with error (tag sectir)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <sectors>\n\
      <sector name='S1' order='1' desc='calzado'/>\n\
      <sectir name='S2' order='2' desc='otros sectores'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors creation
  Sectors sectors;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sectors));
}
