
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include "utils/ExpatUserData.hpp"
#include "utils/ExpatUserDataTest.hpp"

using namespace ccruncher;

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::ExpatUserDataTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::ExpatUserDataTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::ExpatUserDataTest::test1(void)
{
  ExpatUserData eud(10);
  eud.defines["var1"] = "abc";
  eud.defines["var2"] = "def";
  eud.defines["var3"] = "123456789012345";

  const char *str0 = "string without defines";
  ASSERT(str0 == eud.applyDefines(str0));

  const char *str1 = eud.applyDefines("$var1");
  ASSERT_EQUALS(string("abc"), string(str1));

  const char *str2 = eud.applyDefines("$var2");
  ASSERT_EQUALS(string("def"), string(str2));
  ASSERT(str1+4 == str2);

  const char *str3 = eud.applyDefines("$var2");
  ASSERT_EQUALS(string("def"), string(str3));
  ASSERT(str1 == str3);

  const char *str4 = eud.applyDefines("$var1$var2");
  ASSERT_EQUALS(string("abcdef"), string(str4));

  const char *str5 = eud.applyDefines("123456$var1");
  ASSERT_EQUALS(string("123456abc"), string(str5));

  const char *str6 = eud.applyDefines("12$var1 34");
  ASSERT_EQUALS(string("12abc 34"), string(str6));

  ASSERT_THROW(eud.applyDefines("1234567$var1"));
  ASSERT_THROW(eud.applyDefines("$var3"));
}

