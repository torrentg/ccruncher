
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

#include <iostream>
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

  ASSERT(11UL == tokens.size());

  ASSERT(tokens[0] == "");
  ASSERT(tokens[1] == "");
  ASSERT(tokens[2] == "I");
  ASSERT(tokens[3] == "Am");
  ASSERT(tokens[4] == "A");
  ASSERT(tokens[5] == "String");
  ASSERT(tokens[6] == "Ready");
  ASSERT(tokens[7] == "For");
  ASSERT(tokens[8] == "Sacrifice");
  ASSERT(tokens[9] == "");
  ASSERT(tokens[10] == "");

  ASSERT("I Am A String Ready For Sacrifice" == Utils::trim(str1));
  ASSERT("  I AM A STRING READY FOR SACRIFICE  " == Utils::uppercase(str1));
  ASSERT("  i am a string ready for sacrifice  " == Utils::lowercase(str1));
  ASSERT("" == Utils::trim(" "));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::UtilsTest::test3()
{
#ifdef _WIN32

  // testing toNativeSeparators function
  ASSERT_EQUALS("\\\\\\\\", Utils::toNativeSeparators("/\\/\\"));
  ASSERT_EQUALS("\\dir1\\.\\..\\.\\dir2", Utils::toNativeSeparators("/dir1/./.././dir2"));

#endif

  // testing getWorkDir function
  string str2;
  ASSERT_NO_THROW(str2 = Utils::getWorkDir());
  ASSERT(str2.substr(0,1) != ".");
  ASSERT(str2.substr(str2.length()-1,1) != Utils::pathSeparator);

  // testing existDir function
  ASSERT(Utils::existDir("."));
  ASSERT(!Utils::existDir("nonexistentdir"));

  // makeDir function not tested

#ifndef _WIN32

  // test checkFile function (run as non-root user)
  ASSERT_NO_THROW(Utils::checkFile("/etc/passwd", "r"));
  ASSERT_THROW(Utils::checkFile("/etc/passwd", "w"));
  ASSERT_THROW(Utils::checkFile("/etc/passwd", "rw"));
  ASSERT_THROW(Utils::checkFile("/etc/passwd", "falsemode"));

  ASSERT_EQUALS("/etc", Utils::dirname("/etc/passwd"));
  ASSERT_EQUALS("/", Utils::dirname("/etc/"));
  ASSERT_EQUALS(".", Utils::dirname("readme.txt"));
  ASSERT_EQUALS("..", Utils::dirname("../readme.txt"));

  ASSERT_EQUALS("passwd", Utils::filename("/etc/passwd"));
  ASSERT_EQUALS("etc", Utils::filename("/etc/"));
  ASSERT_EQUALS("readme.txt", Utils::filename("readme.txt"));
  ASSERT_EQUALS("readme.txt", Utils::filename("../readme.txt"));

  ASSERT_EQUALS("/etc/passwd", Utils::realpath("/etc//passwd"));
  ASSERT_EQUALS("/etc/passwd", Utils::realpath("/etc/./passwd"));
  ASSERT_EQUALS("/etc/passwd", Utils::realpath("/etc/../etc/passwd"));
  ASSERT_EQUALS(Utils::getWorkDir(), Utils::realpath("."));

#endif
}

//===========================================================================
// test4. test format function
//===========================================================================
void ccruncher_test::UtilsTest::test4()
{
  ASSERT(Utils::millisToString(510) == string("00:00:00.510"));
  ASSERT(Utils::millisToString(50510) == string("00:00:50.510"));
  ASSERT(Utils::millisToString(500510) == string("00:08:20.510"));
  ASSERT(Utils::millisToString(5000510) == string("01:23:20.510"));
  ASSERT(Utils::millisToString(1555000510) == string("431:56:40.510"));
}

