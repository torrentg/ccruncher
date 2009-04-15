
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
// BorrowerTest.hpp - BorrowerTest header - $Rev$
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
// 2007/08/03 - Gerard Torrent [gerard@mail.generacio.com]
//   . Client class renamed to Borrower
//
// 2009/02/10 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed from discrete time to continuous time
//
//===========================================================================

#ifndef _BorrowerTest_
#define _BorrowerTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <MiniCppUnit.hxx>
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "interests/Interests.hpp"
#include "utils/Date.hpp"
#include "segmentations/Segmentations.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class BorrowerTest : public TestFixture<BorrowerTest>
{

  private:

    Ratings getRatings();
    Sectors getSectors();
    Segmentations getSegmentations();
    Interests getInterests();

    void test1(void);
    void test2(void);
    void test3(void);


  public:

    TEST_FIXTURE(BorrowerTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(BorrowerTest);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
