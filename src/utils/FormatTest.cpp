
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

#include <vector>
#include "utils/Format.hpp"
#include "utils/FormatTest.hpp"

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// toString
//===========================================================================
void ccruncher_test::FormatTest::test_toString()
{
  Date date1 = Date("1/1/2005");
  vector<int> v;

  ASSERT("35" == Format::toString(35));
  ASSERT("50000" == Format::toString(50000));
  ASSERT("3.1415" == Format::toString(3.1415));
  ASSERT("01/01/2005" == Format::toString(date1));
  ASSERT("true" == Format::toString(true));
  ASSERT("false" == Format::toString(false));
  ASSERT("0" == Format::toString(v.size()));
}

