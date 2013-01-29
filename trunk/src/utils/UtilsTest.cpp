
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
