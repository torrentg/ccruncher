
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

#include "utils/Strings.hpp"
#include "utils/StringsTest.hpp"

//---------------------------------------------------------------------------

#define XLENGTH 5
#define YLENGTH 7
#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::StringsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::StringsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::StringsTest::test1()
{
  vector<string> tokens;
  string str1 = "  I Am A String Ready For Sacrifice  ";

  Strings::tokenize(str1, tokens, " ");

  ASSERT(7 == tokens.size());

  ASSERT(tokens[0] == "I");
  ASSERT(tokens[1] == "Am");
  ASSERT(tokens[2] == "A");
  ASSERT(tokens[3] == "String");
  ASSERT(tokens[4] == "Ready");
  ASSERT(tokens[5] == "For");
  ASSERT(tokens[6] == "Sacrifice");

  ASSERT("  I Am A String Ready For Sacrifice" == Strings::rtrim(str1));
  ASSERT("I Am A String Ready For Sacrifice  " == Strings::ltrim(str1));
  ASSERT("I Am A String Ready For Sacrifice" == Strings::trim(str1));
  ASSERT("  I AM A STRING READY FOR SACRIFICE  " == Strings::uppercase(str1));
  ASSERT("  i am a string ready for sacrifice  " == Strings::lowercase(str1));
  ASSERT("   " == Strings::blanks(3));
}

