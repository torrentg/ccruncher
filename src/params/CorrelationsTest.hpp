
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

#ifndef _CorrelationsTest_
#define _CorrelationsTest_

#include "utils/MiniCppUnit.hxx"
#include "params/Factors.hpp"

namespace ccruncher_test {

class CorrelationsTest : public TestFixture<CorrelationsTest>
{

  private:

    ccruncher::Factors getFactors();

    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();


  public:

    TEST_FIXTURE(CorrelationsTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
      TEST_CASE(test4);
      TEST_CASE(test5);
      TEST_CASE(test6);
    }

};

REGISTER_FIXTURE(CorrelationsTest)

} // namespace

#endif

