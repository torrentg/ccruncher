
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

#include "utils/Format.hpp"
#include "utils/FormatTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::FormatTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::FormatTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// toString
//===========================================================================
void ccruncher_test::FormatTest::test_toString(void)
{
  Date date1 = Date("1/1/2005");

  ASSERT("35" == Format::int2string(35));
  ASSERT("50000" == Format::long2string(50000));
  ASSERT("3.1415" == Format::double2string(3.1415));
  ASSERT("01/01/2005" == Format::date2string(date1));
  ASSERT("true" == Format::bool2string(true));
  ASSERT("false" == Format::bool2string(false));
}
