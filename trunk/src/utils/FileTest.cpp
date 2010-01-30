
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#include "utils/File.hpp"
#include "utils/FileTest.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::FileTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::FileTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::FileTest::test1(void)
{
  // testing normalizePath function
  string str1;
  ASSERT_NO_THROW(str1= File::normalizePath("./dir1/dir2"));
  ASSERT(str1.substr(0,1) != ".");
  ASSERT(str1.substr(str1.length()-1,1) == "/");

  // testing getWorkDir function
  string str2;
  ASSERT_NO_THROW(str2 = File::getWorkDir());
  ASSERT(str2.substr(0,1) != ".");
  ASSERT(str2.substr(str2.length()-1,1) == "/");

  // testing existDir function
  ASSERT(File::existDir("."));
  ASSERT(!File::existDir("nonexistentdir"));

  // makeDir function not tested

  // test checkFile function (run as non-root user)
  ASSERT_NO_THROW(File::checkFile("/etc/passwd", "r"));
  ASSERT_THROW(File::checkFile("/etc/passwd", "w"));
  ASSERT_THROW(File::checkFile("/etc/passwd", "rw"));
  ASSERT_THROW(File::checkFile("/etc/passwd", "falsemode"));
}
