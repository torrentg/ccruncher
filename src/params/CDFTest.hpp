
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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

#ifndef _CDFTest_
#define _CDFTest_

#include "utils/MiniCppUnit.hxx"
#include "params/Ratings.hpp"

namespace ccruncher_test {

class CDFTest : public TestFixture<CDFTest>
{

  private:

    void test1();
    void test2();
    void test3();
    void test4();
    void test5();


  public:

    TEST_FIXTURE(CDFTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
      TEST_CASE(test4);
      TEST_CASE(test5);
    }

};

REGISTER_FIXTURE(CDFTest)

} // namespace

#endif

