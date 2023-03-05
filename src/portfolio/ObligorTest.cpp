
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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

#include "portfolio/Obligor.hpp"
#include "portfolio/ObligorTest.hpp"
#include "utils/Date.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::ObligorTest::test1()
{
  Asset asset1(2);
  asset1.values.push_back(DateValues(Date("01/01/2014"), NAN, 1.0));
  asset1.values.push_back(DateValues(Date("01/01/2015"), 1000.0, 0.90));
  asset1.values.push_back(DateValues(Date("01/01/2016"), 500.0, 0.80));

  Asset asset2(2);
  asset2.values.push_back(DateValues(Date("01/07/2014"), NAN, 1.0));
  asset2.values.push_back(DateValues(Date("01/07/2015"), 1500.0, 0.90));
  asset2.values.push_back(DateValues(Date("01/07/2016"), 1500.0, 0.75));

  Obligor obligor1;
  obligor1.irating = 1;
  obligor1.ifactor = 3;
  obligor1.assets.push_back(asset1);
  obligor1.assets.push_back(asset2);

  Obligor obligor2;
  obligor2.irating = 0;
  obligor2.ifactor = 4;

  ASSERT(obligor1.isActive(Date("01/07/2013"), Date("01/07/2014")));
  ASSERT(!obligor2.isActive(Date("01/07/2013"), Date("01/07/2014")));
}

