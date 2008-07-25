
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
//
// UtilsTest.cpp - UtilsTest code
// --------------------------------------------------------------------------
//
// 2005/07/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <iostream>
#include <string>
#include "utils/Utils.hpp"
#include "utils/UtilsTest.hpp"
#include "utils/Exception.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::UtilsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::UtilsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::UtilsTest::test1(void)
{
  // hash function
  ASSERT(Utils::hash("ccruncher") == 163903330UL);
  ASSERT(Utils::hash("client1") == 170902801UL);
  ASSERT(Utils::hash("client2") == 170902802UL);

  // testing timestamp function
  ASSERT(Utils::timestamp().size() == 19);
}
