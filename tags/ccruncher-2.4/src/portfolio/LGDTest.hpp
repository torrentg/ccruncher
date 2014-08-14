
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

#ifndef _LGDTest_
#define _LGDTest_

#include "utils/MiniCppUnit.hxx"

namespace ccruncher_test {

class LGDTest : public TestFixture<LGDTest>
{

  private:

    void test1();
    void test2();


  public:

    TEST_FIXTURE(LGDTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
    }

};

REGISTER_FIXTURE(LGDTest)

} // namespace

#endif
