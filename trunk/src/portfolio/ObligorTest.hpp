
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

#ifndef _ObligorTest_
#define _ObligorTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <MiniCppUnit.hxx>
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "interests/Interest.hpp"
#include "utils/Date.hpp"
#include "segmentations/Segmentations.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class ObligorTest : public TestFixture<ObligorTest>
{

  private:

    Ratings getRatings();
    Sectors getSectors();
    Segmentations getSegmentations();
    Interest getInterest();

    void test1(void);
    void test2(void);
    void test3(void);
    void test4(void);


  public:

    TEST_FIXTURE(ObligorTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
      TEST_CASE(test4);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(ObligorTest);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
