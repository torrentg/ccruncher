
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

#include "utils/MacrosBuffer.hpp"
#include "utils/MacrosBufferTest.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::MacrosBufferTest::test1()
{
  MacrosBuffer macros(10);
  macros.values["var1"] = "abc";
  macros.values["var2"] = "def";
  macros.values["var3"] = "123456789012345";
  macros.values["var_4"] = "coco";

  const char *str0 = "string without defines";
  ASSERT(str0 == macros.apply(str0));

  const char *str1 = macros.apply("$var1");
  ASSERT_EQUALS(string("abc"), string(str1));

  const char *str2 = macros.apply("$var2");
  ASSERT_EQUALS(string("def"), string(str2));
  ASSERT(str1+4 == str2);

  const char *str3 = macros.apply("$var2");
  ASSERT_EQUALS(string("def"), string(str3));
  ASSERT(str1 == str3);

  const char *str4 = macros.apply("$var1$var2");
  ASSERT_EQUALS(string("abcdef"), string(str4));

  const char *str5 = macros.apply("123456$var1");
  ASSERT_EQUALS(string("123456abc"), string(str5));

  const char *str6 = macros.apply("12$var1 34");
  ASSERT_EQUALS(string("12abc 34"), string(str6));

  const char *str7 = macros.apply("abc $var_4");
  ASSERT_EQUALS(string("abc coco"), string(str7));

  // exception throw because exceeds buffer size
  ASSERT_THROW(macros.apply("1234567$var1"));
  ASSERT_THROW(macros.apply("$var3"));

  // exception because define not found
  ASSERT_THROW(macros.apply("$var1a"));
}

