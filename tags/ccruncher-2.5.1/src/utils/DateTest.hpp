
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#pragma once

#include "utils/MiniCppUnit.hxx"

namespace ccruncher_test {

class DateTest : public TestFixture<DateTest>
{

  private:

    void test_nad();
    void test_constructors();
    void test_valid();
    void test_gets();
    void test_rollers();
    void test_misc();
    void test_comparators();
    void test_dayofweek();
    void test_distances();
    void test_intervals();

  public:

    TEST_FIXTURE(DateTest)
    {
      TEST_CASE(test_nad);
      TEST_CASE(test_constructors);
      TEST_CASE(test_valid);
      TEST_CASE(test_gets);
      TEST_CASE(test_rollers);
      TEST_CASE(test_misc);
      TEST_CASE(test_comparators);
      TEST_CASE(test_dayofweek);
      TEST_CASE(test_distances);
      TEST_CASE(test_intervals);
    }

};

REGISTER_FIXTURE(DateTest)

} // namespace
