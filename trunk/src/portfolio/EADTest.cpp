
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#define EPSILON 1E-14

//===========================================================================
// test1. constructors
//===========================================================================
void ccruncher_test::EADTest::test1()
{
  // fixed values
  {
    EAD r0 = EAD(EAD::Fixed,NAN);
    ASSERT(r0.getType() == EAD::Fixed);
    ASSERT(isnan(r0.getValue1()));
    ASSERT(isnan(r0.getValue2()));
  
    EAD r1;
    ASSERT(r1.getType() == EAD::Fixed);
    ASSERT(isnan(r1.getValue1()));
    ASSERT(isnan(r1.getValue2()));
  
    EAD r2(EAD::Fixed,1.0);
    ASSERT(r2.getType() == EAD::Fixed);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 1.0, EPSILON);
    ASSERT(isnan(r2.getValue2()));

    EAD r3("+1000.0");
    ASSERT(r3.getType() == EAD::Fixed);
    ASSERT_EQUALS_EPSILON(r3.getValue1(), 1000.0, EPSILON);

    EAD r4("+1000.0 -100*2");
    ASSERT(r4.getType() == EAD::Fixed);
    ASSERT_EQUALS_EPSILON(r4.getValue1(), 800.0, EPSILON);
  }

  // lognormal distribution
  {
    EAD r0 = EAD(EAD::Lognormal, 800.0, 60.0);
    ASSERT(r0.getType() == EAD::Lognormal);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 800.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 60.0, EPSILON);

    EAD r1("lognormal(800.0,60.0)");
    ASSERT(r1.getType() == EAD::Lognormal);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 800.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 60.0, EPSILON);

    EAD r2("lognormal(max(-10,800.0),min(60.0,100))");
    ASSERT(r2.getType() == EAD::Lognormal);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 800.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 60.0, EPSILON);
  }

  // exponential distribution
  {
    EAD r0 = EAD(EAD::Exponential, 800.0);
    ASSERT(r0.getType() == EAD::Exponential);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 800.0, EPSILON);
    ASSERT(isnan(r0.getValue2()));

    EAD r1("exponential(800.0)");
    ASSERT(r1.getType() == EAD::Exponential);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 800.0, EPSILON);
    ASSERT(isnan(r1.getValue2()));

    EAD r2("exponential(1000-200.0)");
    ASSERT(r2.getType() == EAD::Exponential);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 800.0, EPSILON);
    ASSERT(isnan(r2.getValue2()));
  }

  // uniform distribution
  {
    EAD r0(EAD::Uniform, 0.0, 1.0);
    ASSERT(r0.getType() == EAD::Uniform);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 0.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 1.0, EPSILON);

    EAD r1(EAD::Uniform, 0.25, 0.5);
    ASSERT(r1.getType() == EAD::Uniform);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 0.25, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 0.5, EPSILON);

    EAD r2("uniform(0.2,0.3)");
    ASSERT(r2.getType() == EAD::Uniform);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 0.3, EPSILON);

    EAD r3("uniform( +0.2, +0.3 )");
    ASSERT(r3.getType() == EAD::Uniform);
    ASSERT_EQUALS_EPSILON(r3.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r3.getValue2(), 0.3, EPSILON);
  }

  // gamma distribution
  {
    EAD r0(EAD::Gamma, 0.5, 0.25);
    ASSERT(r0.getType() == EAD::Gamma);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 0.5, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 0.25, EPSILON);
  
    EAD r1("gamma(0.2,0.3)");
    ASSERT(r1.getType() == EAD::Gamma);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 0.3, EPSILON);

    EAD r2("gamma( +0.2,0.3 )");
    ASSERT(r2.getType() == EAD::Gamma);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 0.3, EPSILON);
  }

  // normal distribution
  {
    EAD r0(EAD::Normal, 500.0, 25.0);
    ASSERT(r0.getType() == EAD::Normal);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 500.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 25.0, EPSILON);

    EAD r1("normal(500.0,25)");
    ASSERT(r1.getType() == EAD::Normal);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 500.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 25.0, EPSILON);

    EAD r2("normal( +500*1 , 25+5*(4-2*2) )");
    ASSERT(r2.getType() == EAD::Normal);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 500.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 25.0, EPSILON);
  }
}

//===========================================================================
// test2. exceptions
//===========================================================================
void ccruncher_test::EADTest::test2()
{
  // fixed values
  ASSERT_THROW(EAD(EAD::Fixed,-0.5));    // fixed & less than 0
  ASSERT_NO_THROW(EAD(EAD::Fixed,NAN));  // fixed & distinct than [0,1]

  // lognormal
  ASSERT_THROW(EAD("lognormal (0.2,0.3)"));      // additional space
  ASSERT_THROW(EAD("lognormal(0.2,0.3"));        // non-closed parentesis
  ASSERT_THROW(EAD("lognormal(a,0.3)"));         // invalid number
  ASSERT_THROW(EAD("lognormal(+0.2,-0.3)"));     // negative argument
  ASSERT_THROW(EAD("lognormal(-0.2,+0.3)"));     // negative argument
  ASSERT_THROW(EAD("lognormal(0.2)"));           // only one argument
  ASSERT_THROW(EAD("lognormal()"));              // lacks arguments
  ASSERT_THROW(EAD("lognormal(0.2,)"));          // lacks argument 2
  ASSERT_THROW(EAD("lognormal(,0.3)"));          // lacks argument 1

  // exponential
  ASSERT_THROW(EAD("exponential (0.2)"));        // additional space
  ASSERT_THROW(EAD("exponential 0.2)"));         // non-opened parentesis
  ASSERT_THROW(EAD("exponential(0.2"));          // non-closed parentesis
  ASSERT_THROW(EAD("exponential(a)"));           // invalid number
  ASSERT_THROW(EAD("exponential(-0.3)"));        // negative argument
  ASSERT_THROW(EAD("exponential(-0.2,+0.3)"));   // 2 arguments
  ASSERT_THROW(EAD("exponential()"));            // 0 arguments

  // gamma
  ASSERT_THROW(EAD("gamma (0.2,0.3)"));      // additional space
  ASSERT_THROW(EAD("gamma(a,0.3)"));         // invalid number
  ASSERT_THROW(EAD("gamma(+0.2,-0.3)"));     // negative argument
  ASSERT_THROW(EAD("gamma(-0.2,+0.3)"));     // negative argument
  ASSERT_THROW(EAD("gamma(0.2,)"));          // lacks argument 2
  ASSERT_THROW(EAD("gama(0.5,0.25)"));       // gamma != gama
  ASSERT_THROW(EAD("gamma[0.5,0.25]"));      // invalid delimiters
  ASSERT_THROW(EAD("gamma(0.2,0.3"));        // lacks ending ')'
  ASSERT_THROW(EAD("gamma(0.2)"));           // requires 2 arguments
  ASSERT_THROW(EAD("gamma(0.2,0.1,0.4)"));   // requires 2 arguments

  // uniform
  ASSERT_THROW(EAD(EAD::Uniform,0.5));           // requires 2 arguments
  ASSERT_THROW(EAD(EAD::Uniform,-0.5,+0.5));     // uniform & value1 < 0.0
  ASSERT_NO_THROW(EAD(EAD::Uniform,+0.5,+1.5));  // uniform & value2 > 1.0
  ASSERT_THROW(EAD(EAD::Uniform,+0.5,+0.25));    // uniform & value2 <= value1
  ASSERT_THROW(EAD("uniform(0.3,0.2)"));         // low > up
  ASSERT_THROW(EAD("uniform(-0.3,0.2)"));        // negative value
  ASSERT_THROW(EAD("uniform(0.2)"));             // requires 2 arguments
  ASSERT_THROW(EAD("uniform(0.2,0.1,0.4)"));     // requires 2 arguments

  // gamma
  ASSERT_THROW(EAD("gamma(10)"));           // requires 2 arguments
  ASSERT_THROW(EAD(EAD::Gamma,0.5));        // requires 2 arguments
  ASSERT_THROW(EAD(EAD::Gamma,-0.5,+0.5));  // gamma & value1 <= 0.0
  ASSERT_THROW(EAD(EAD::Gamma,+0.5,-0.5));  // gamma & value1 <= 0.0

  // normal
  ASSERT_THROW(EAD("normal (0.2,0.3)"));      // additional space
  ASSERT_THROW(EAD("normal(a,0.3)"));         // invalid number
  ASSERT_THROW(EAD("normal(+0.2,-0.3)"));     // negative argument
  ASSERT_THROW(EAD("normal(-0.2,+0.3)"));     // negative argument
  ASSERT_THROW(EAD("normal(0.2,)"));          // lacks argument 2
  ASSERT_THROW(EAD("normal[0.5,0.25]"));      // invalid delimiters
  ASSERT_THROW(EAD("normal(0.2,0.3"));        // lacks ending ')'
  ASSERT_THROW(EAD("normal(0.2)"));           // requires 2 arguments
  ASSERT_THROW(EAD("normal(0.2,0.1,0.4)"));   // requires 2 arguments

  // other
  ASSERT_THROW(EAD("nonfunction(0.2,0.1)"));   // unrecognized function
  ASSERT_THROW(EAD("nonfunction(0.2)"));       // unrecognized function
}
