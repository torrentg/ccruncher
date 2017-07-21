
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#ifndef _ExprTest_
#define _ExprTest_

#include "utils/MiniCppUnit.hxx"

namespace ccruncher_test {

class ExprTest : public TestFixture<ExprTest>
{

private:

  struct row
  {
    std::string formula;
    double result;
    int num_tokens;
    int stack_size;
    row(const std::string &s, double v, int n1, int n2) : formula(s), result(v), num_tokens(n1), stack_size(n2) {}
  };

private:

  void test1();
  void test2();
  void test3();
  void test4();


public:

  TEST_FIXTURE(ExprTest)
  {
    TEST_CASE(test1);
    TEST_CASE(test2);
    TEST_CASE(test3);
    TEST_CASE(test4);
  }

};

REGISTER_FIXTURE(ExprTest)

} // namespace

#endif

