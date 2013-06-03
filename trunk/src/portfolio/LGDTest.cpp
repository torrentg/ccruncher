
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

#include <cmath>
#include "portfolio/LGD.hpp"
#include "portfolio/LGDTest.hpp"

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// test1. constructors
//===========================================================================
void ccruncher_test::LGDTest::test1()
{
  LGD r0 = LGD(LGD::Fixed,NAN);
  ASSERT(r0.getType() == LGD::Fixed && isnan(r0.getValue1()) && isnan(r0.getValue2()));
  
  LGD r1;
  ASSERT(r1.getType() == LGD::Fixed && isnan(r1.getValue1()) && isnan(r1.getValue2()));
  
  LGD r2(LGD::Fixed,1.0);
  ASSERT(r2.getType() == LGD::Fixed && fabs(r2.getValue1()-1.0) < EPSILON && isnan(r2.getValue2()));

  LGD r3(LGD::Beta, 0.5, 0.25);
  ASSERT(r3.getType() == LGD::Beta && fabs(r3.getValue1()-0.5) < EPSILON && fabs(r3.getValue2()-0.25) < EPSILON);
  
  LGD r4(LGD::Beta, 0.5, 0.25);
  ASSERT(r4.getType() == LGD::Beta && fabs(r4.getValue1()-0.5) < EPSILON && fabs(r4.getValue2()-0.25) < EPSILON);
  
  LGD r5("beta(0.2,0.3)");
  ASSERT(r5.getType() == LGD::Beta && fabs(r5.getValue1()-0.2) < EPSILON && fabs(r5.getValue2()-0.3) < EPSILON);

  LGD r6("beta( +0.2, +0.3 )");
  ASSERT(r6.getType() == LGD::Beta && fabs(r6.getValue1()-0.2) < EPSILON && fabs(r6.getValue2()-0.3) < EPSILON);

  LGD r7("beta(0.2,0.3"); // warning: lacks ending ')' but works
  ASSERT(r7.getType() == LGD::Beta && fabs(r7.getValue1()-0.2) < EPSILON && fabs(r7.getValue2()-0.3) < EPSILON);
  
  LGD r8(LGD::Uniform, 0.0, 1.0);
  ASSERT(r8.getType() == LGD::Uniform && fabs(r8.getValue1()-0.0) < EPSILON && fabs(r8.getValue2()-1.0) < EPSILON);
  
  LGD r9(LGD::Uniform, 0.25, 0.5);
  ASSERT(r9.getType() == LGD::Uniform && fabs(r9.getValue1()-0.25) < EPSILON && fabs(r9.getValue2()-0.5) < EPSILON);
  
  LGD r10("uniform(0.2,0.3)");
  ASSERT(r10.getType() == LGD::Uniform && fabs(r10.getValue1()-0.2) < EPSILON && fabs(r10.getValue2()-0.3) < EPSILON);

  LGD r11("uniform( +0.2, +0.3 )");
  ASSERT(r11.getType() == LGD::Uniform && fabs(r11.getValue1()-0.2) < EPSILON && fabs(r11.getValue2()-0.3) < EPSILON);

  LGD r12("uniform(0.2,0.3"); // warning: lacks ending ')' but works
  ASSERT(r12.getType() == LGD::Uniform && fabs(r12.getValue1()-0.2) < EPSILON && fabs(r12.getValue2()-0.3) < EPSILON);
  
  ASSERT_THROW(LGD("beta (0.2,0.3)"));      // additional space
  ASSERT_THROW(LGD("beta(a,0.3)"));         // invalid number
  ASSERT_THROW(LGD("beta(+0.2,-0.3)"));     // negative argument
  ASSERT_THROW(LGD("beta(-0.2,+0.3)"));     // negative argument
  ASSERT_THROW(LGD("beta(0.2,)"));          // lacks argument 2
  ASSERT_THROW(LGD("beta( +0.2 , +0.3 )")); // space after argument 1
  ASSERT_THROW(LGD("beto(0.5,0.25)"));      // beto != beta
  ASSERT_THROW(LGD("beta[0.5,0.25]"));      // invalid delimiters
}

//===========================================================================
// test2. ranges
//===========================================================================
void ccruncher_test::LGDTest::test2()
{
  ASSERT_THROW(LGD(LGD::Fixed,-0.5)); // fixed & distinct than [0,1]
  ASSERT_THROW(LGD(LGD::Fixed,+1.5)); // fixed & distinct than [0,1]
  ASSERT_THROW(LGD(LGD::Beta,-0.5,+0.5));  // beta & value1 <= 0.0
  ASSERT_THROW(LGD(LGD::Beta,+0.5,-0.5));  // beta & value1 <= 0.0
  ASSERT_THROW(LGD(LGD::Uniform,-0.5,+0.5));  // uniform & value1 < 0.0
  ASSERT_THROW(LGD(LGD::Uniform,+0.5,+1.5));  // uniform & value2 > 1.0
  ASSERT_THROW(LGD(LGD::Uniform,+0.5,+0.25)); // uniform & value2 <= value1

  ASSERT_NO_THROW(LGD(LGD::Fixed,NAN));  // fixed & distinct than [0,1]
}

