
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
#include "portfolio/EAD.hpp"
#include "portfolio/EADTest.hpp"

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// test1. constructors
//===========================================================================
void ccruncher_test::EADTest::test1()
{
  EAD r0 = EAD(EAD::Fixed,NAN);
  ASSERT(r0.getType() == EAD::Fixed && isnan(r0.getValue1()) && isnan(r0.getValue2()));
  
  EAD r1;
  ASSERT(r1.getType() == EAD::Fixed && isnan(r1.getValue1()) && isnan(r1.getValue2()));
  
  EAD r2(EAD::Fixed,1.0);
  ASSERT(r2.getType() == EAD::Fixed && fabs(r2.getValue1()-1.0) < EPSILON && isnan(r2.getValue2()));

  EAD r3(EAD::Gamma, 0.5, 0.25);
  ASSERT(r3.getType() == EAD::Gamma && fabs(r3.getValue1()-0.5) < EPSILON && fabs(r3.getValue2()-0.25) < EPSILON);
  
  EAD r4(EAD::Gamma, 0.5, 0.25);
  ASSERT(r4.getType() == EAD::Gamma && fabs(r4.getValue1()-0.5) < EPSILON && fabs(r4.getValue2()-0.25) < EPSILON);
  
  EAD r5("gamma(0.2,0.3)");
  ASSERT(r5.getType() == EAD::Gamma && fabs(r5.getValue1()-0.2) < EPSILON && fabs(r5.getValue2()-0.3) < EPSILON);

  EAD r6("gamma( +0.2, +0.3 )");
  ASSERT(r6.getType() == EAD::Gamma && fabs(r6.getValue1()-0.2) < EPSILON && fabs(r6.getValue2()-0.3) < EPSILON);

  EAD r7("gamma(0.2,0.3"); // warning: lacks ending ')' but works
  ASSERT(r7.getType() == EAD::Gamma && fabs(r7.getValue1()-0.2) < EPSILON && fabs(r7.getValue2()-0.3) < EPSILON);
  
  EAD r8(EAD::Uniform, 0.0, 1.0);
  ASSERT(r8.getType() == EAD::Uniform && fabs(r8.getValue1()-0.0) < EPSILON && fabs(r8.getValue2()-1.0) < EPSILON);
  
  EAD r9(EAD::Uniform, 0.25, 0.5);
  ASSERT(r9.getType() == EAD::Uniform && fabs(r9.getValue1()-0.25) < EPSILON && fabs(r9.getValue2()-0.5) < EPSILON);
  
  EAD r10("uniform(0.2,0.3)");
  ASSERT(r10.getType() == EAD::Uniform && fabs(r10.getValue1()-0.2) < EPSILON && fabs(r10.getValue2()-0.3) < EPSILON);

  EAD r11("uniform( +0.2, +0.3 )");
  ASSERT(r11.getType() == EAD::Uniform && fabs(r11.getValue1()-0.2) < EPSILON && fabs(r11.getValue2()-0.3) < EPSILON);

  EAD r12("uniform(0.2,0.3"); // warning: lacks ending ')' but works
  ASSERT(r12.getType() == EAD::Uniform && fabs(r12.getValue1()-0.2) < EPSILON && fabs(r12.getValue2()-0.3) < EPSILON);
  
  ASSERT_THROW(EAD("gamma (0.2,0.3)"));      // additional space
  ASSERT_THROW(EAD("gamma(a,0.3)"));         // invalid number
  ASSERT_THROW(EAD("gamma(+0.2,-0.3)"));     // negative argument
  ASSERT_THROW(EAD("gamma(-0.2,+0.3)"));     // negative argument
  ASSERT_THROW(EAD("gamma(0.2,)"));          // lacks argument 2
  ASSERT_THROW(EAD("gamma( +0.2 , +0.3 )")); // space after argument 1
  ASSERT_THROW(EAD("gama(0.5,0.25)"));       // gamma != gama
  ASSERT_THROW(EAD("gamma[0.5,0.25]"));      // invalid delimiters
}

//===========================================================================
// test2. ranges
//===========================================================================
void ccruncher_test::EADTest::test2()
{
  ASSERT_THROW(EAD(EAD::Fixed,-0.5)); // fixed & distinct than [0,1]
  ASSERT_NO_THROW(EAD(EAD::Fixed,+1.5)); // fixed & distinct than [0,1]
  ASSERT_THROW(EAD(EAD::Gamma,-0.5,+0.5));  // gamma & value1 <= 0.0
  ASSERT_THROW(EAD(EAD::Gamma,+0.5,-0.5));  // gamma & value1 <= 0.0
  ASSERT_THROW(EAD(EAD::Uniform,-0.5,+0.5));  // uniform & value1 < 0.0
  ASSERT_NO_THROW(EAD(EAD::Uniform,+0.5,+1.5));  // uniform & value2 > 1.0
  ASSERT_THROW(EAD(EAD::Uniform,+0.5,+0.25)); // uniform & value2 <= value1

  ASSERT_NO_THROW(EAD(EAD::Fixed,NAN));  // fixed & distinct than [0,1]
}

