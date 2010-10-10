
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
// test1. constructors
//===========================================================================
void ccruncher_test::RecoveryTest::test1(void)
{
  Recovery r0 = Recovery::getNAN();
  ASSERT(r0.type == Fixed && isnan(r0.value1) && isnan(r0.value2));
  
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
  ASSERT_THROW(r = new Recovery("beto(0.5,0.25)"));      // beto != beta
  ASSERT_THROW(r = new Recovery("beta[0.5,0.25]"));      // invalid delimiters
}

//===========================================================================
// test2. ranges
//===========================================================================
void ccruncher_test::RecoveryTest::test2(void)
{
  Recovery *r = NULL;
  ASSERT_THROW(r = new Recover(-0.5)); // fixed & distinct than [0,1]
  ASSERT_THROW(r = new Recover(+1.5)); // fixed & distinct than [0,1]
  ASSERT_THROW(r = new Recover(NAN));  // fixed & distinct than [0,1]
  ASSERT_THROW(r = new Recover(-0.5,+0.5));  // beta & value1 <= 0.0
  ASSERT_THROW(r = new Recover(+0.5,-0.5));  // beta & value1 <= 0.0
  
  Recovery r1(0.1);
  ASSERT(r1.getType() == Fixed && r1.getValue1() == 0.1);

  Recovery r2(0.1,0.7);
  ASSERT(r2.getType() == Beta && r2.getValue1() == 0.1 && r2.getValue2() === 0.7);
}

