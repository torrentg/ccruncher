
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

#include <iostream>
#include <cmath>
#include "utils/LookupTable.hpp"
#include "utils/LookupTableTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-7

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::LookupTableTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::LookupTableTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::LookupTableTest::test1()
{
  vector<double> v;

  // fails because xmin >= xmax
  v.push_back(0.0);
  v.push_back(1.0);
  ASSERT_THROW(LookupTable(1.0, 0.0, v));

  // fails because v.size() < 2
  v.clear();
  v.push_back(1.0);
  ASSERT_THROW(LookupTable(0.0, 1.0, v));
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::LookupTableTest::test2()
{
  LookupTable lut;
  vector<double> v;

  for(int i=0; i<=2000; i++)
  {
    double x = (double)(i)/1000.0;
    double y = sin(x);
    v.push_back(y);
  }

  lut = LookupTable(0.0, 2.0, v);

  ASSERT_EQUALS(lut.size(), 2001);
  ASSERT_EQUALS_EPSILON(sin(0.0), lut.evalue(-1.0), EPSILON);
  ASSERT_EQUALS_EPSILON(sin(0.0), lut.evalue(0.0), EPSILON);
  ASSERT_EQUALS_EPSILON(sin(1.0), lut.evalue(1.0), EPSILON);
  ASSERT_EQUALS_EPSILON(sin(2.0), lut.evalue(2.0), EPSILON);
  ASSERT_EQUALS_EPSILON(sin(2.0), lut.evalue(3.0), EPSILON);
  for(int i=0; i<58; i++)
  {
    double x = (double)(i) * 2.0/57.0;
    double y = sin(x);
    ASSERT_EQUALS_EPSILON(y, lut.evalue(x), EPSILON);
  }
}

