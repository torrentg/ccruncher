
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
// AssetTest.hpp - AssetTest header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//===========================================================================

#ifndef _AssetTest_
#define _AssetTest_

//---------------------------------------------------------------------------

#include <MiniCppUnit.hxx>
#include "segmentations/Segmentations.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;

//---------------------------------------------------------------------------

class AssetTest : public TestFixture<AssetTest>
{

  private:
  
    Segmentations getSegmentations();
    void makeAssertions(Asset *asset);

    void test1(void);
    void test2(void);


  public:

    TEST_FIXTURE(AssetTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
    }

    void setUp();
    void tearDown();
    
};

REGISTER_FIXTURE(AssetTest);

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
