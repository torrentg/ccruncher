
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

#include <cmath>
#include "portfolio/Recovery.hpp"
#include "portfolio/RecoveryTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

using namespace ccruncher;

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::RecoveryTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::RecoveryTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::RecoveryTest::test1(void)
{
  Recovery r1;
  ASSERT(r1.type == Fixed && isnan(r1.value1) && isnan(r1.value2));
  
  Recovery r2(1.0);
  ASSERT(r2.type == Fixed && fabs(r2.value1-1.0) < EPSILON && isnan(r2.value2));

  Recovery r3(0.5, 0.25);
  ASSERT(r3.type == Beta && fabs(r3.value1-0.5) < EPSILON && fabs(r3.value2-0.25) < EPSILON);
  
  Recovery r4(Beta, 0.5, 0.25);
  ASSERT(r4.type == Beta && fabs(r4.value1-0.5) < EPSILON && fabs(r4.value2-0.25) < EPSILON);
  
  Recovery r5("beta(0.2,0.3)");
  ASSERT(r5.type == Beta && fabs(r5.value1-0.2) < EPSILON && fabs(r5.value2-0.3) < EPSILON);

  Recovery r6("beta( +0.2, +0.3 )");
  ASSERT(r6.type == Beta && fabs(r6.value1-0.2) < EPSILON && fabs(r6.value2-0.3) < EPSILON);

  Recovery r7("beta(0.2,0.3"); // warning: lacks ending ')' but works
  ASSERT(r7.type == Beta && fabs(r7.value1-0.2) < EPSILON && fabs(r7.value2-0.3) < EPSILON);

  Recovery *r = NULL;
  ASSERT_THROW(r = new Recovery("beta (0.2,0.3)"));      // additional space
  ASSERT_THROW(r = new Recovery("beta(a,0.3)"));         // invalid number
  ASSERT_THROW(r = new Recovery("beta(+0.2,-0.3)"));     // negative argument
  ASSERT_THROW(r = new Recovery("beta(-0.2,+0.3)"));     // negative argument
  ASSERT_THROW(r = new Recovery("beta(0.2,)"));          // kacks argument 2
  ASSERT_THROW(r = new Recovery("beta( +0.2 , +0.3 )")); // space after argument 1
}

