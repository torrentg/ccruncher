
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#include <iostream>
#include <cmath>
#include <ctime>
#include "utils/Utils.hpp"
#include "utils/UtilsTest.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::UtilsTest::test1()
{
  // testing timestamp function
  ASSERT(Utils::timestamp().size() == 19);

  // testing random value based on time
  long val1 = Utils::trand();
  // waiting 1 second
  time_t time1 = time(0) + 1;
  while (time(0) < time1);
  // 1 second ellapsed
  long val2 = Utils::trand();
  ASSERT(val1 != val2);
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::UtilsTest::test2()
{
  vector<string> tokens;
  string str1 = "  I Am A String Ready For Sacrifice  ";

  Utils::tokenize(str1, tokens, " ");

  ASSERT(7 == tokens.size());

  ASSERT(tokens[0] == "I");
  ASSERT(tokens[1] == "Am");
  ASSERT(tokens[2] == "A");
  ASSERT(tokens[3] == "String");
  ASSERT(tokens[4] == "Ready");
  ASSERT(tokens[5] == "For");
  ASSERT(tokens[6] == "Sacrifice");

  ASSERT("I Am A String Ready For Sacrifice" == Utils::trim(str1));
  ASSERT("  I AM A STRING READY FOR SACRIFICE  " == Utils::uppercase(str1));
  ASSERT("  i am a string ready for sacrifice  " == Utils::lowercase(str1));
  ASSERT("" == Utils::trim(" "));
}

