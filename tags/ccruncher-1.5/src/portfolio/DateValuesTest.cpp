
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

#include "portfolio/DateValuesTest.hpp"
#include "portfolio/DateValues.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;

// --------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::DateValuesTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::DateValuesTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// comparators tests
//===========================================================================
void ccruncher_test::DateValuesTest::test1()
{
  DateValues val1(Date(10,10,2001), +1.0, +2.0);
  DateValues val2(Date(01,01,2006), -1.0, -2.0);

  // checking that sorts by date
  ASSERT(val1 < val2);
}
