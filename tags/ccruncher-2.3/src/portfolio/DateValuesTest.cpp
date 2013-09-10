
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

#include "portfolio/DateValuesTest.hpp"
#include "portfolio/DateValues.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;

//===========================================================================
// comparators tests
//===========================================================================
void ccruncher_test::DateValuesTest::test1()
{
  DateValues val1(Date(10,10,2001), EAD(EAD::Fixed,+1.0), LGD(LGD::Fixed,0.50));
  DateValues val2(Date(01,01,2006), EAD(EAD::Fixed,+0.5), LGD(LGD::Fixed,0.75));

  // checking that sorts by date
  ASSERT(val1 < val2);
}
