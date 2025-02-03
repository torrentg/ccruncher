
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#define EPSILON 1E-6

//===========================================================================
// test1. constructors
//===========================================================================
void ccruncher_test::EADTest::test1()
{
  // fixed values
  {
    EAD r0 = EAD(EAD::Type::Fixed,NAN);
    ASSERT(r0.getType() == EAD::Type::Fixed);
    ASSERT(std::isnan(r0.getValue1()));
    ASSERT(std::isnan(r0.getValue2()));
    ASSERT(std::isnan(r0.getExpected()));

    EAD r1;
    ASSERT(r1.getType() == EAD::Type::Fixed);
    ASSERT_EQUALS_EPSILON(0.0, r1.getValue1(), EPSILON);
    ASSERT(std::isnan(r1.getValue2()));
    ASSERT_EQUALS_EPSILON(0.0, r1.getExpected(), EPSILON);

    EAD r2(EAD::Type::Fixed,1.0);
    ASSERT(r2.getType() == EAD::Type::Fixed);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 1.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getExpected(), 1.0, EPSILON);
    ASSERT(std::isnan(r2.getValue2()));

    EAD r3("+1000.0");
    ASSERT(r3.getType() == EAD::Type::Fixed);
    ASSERT_EQUALS_EPSILON(r3.getValue1(), 1000.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r3.getExpected(), 1000.0, EPSILON);

    EAD r4("+1000.0 -100*2");
    ASSERT(r4.getType() == EAD::Type::Fixed);
    ASSERT_EQUALS_EPSILON(r4.getValue1(), 800.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r4.getExpected(), 800.0, EPSILON);

    EAD r5(3000.0);
    ASSERT(r5.getType() == EAD::Type::Fixed);
    ASSERT_EQUALS_EPSILON(r5.getValue1(), 3000.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r5.getExpected(), 3000.0, EPSILON);
  }

  // lognormal distribution
  {
    EAD r0 = EAD(EAD::Type::Lognormal, log(800.0), log(1.2));
    ASSERT(r0.getType() == EAD::Type::Lognormal);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), log(800.0), EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), log(1.2), EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getExpected(), 813.4076, EPSILON);

    EAD r1("lognormal(log(800.0),log(1.2))");
    ASSERT(r1.getType() == EAD::Type::Lognormal);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), log(800.0), EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), log(1.2), EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getExpected(), 813.4076, EPSILON);

    EAD r2("lognormal(max(-10,log(800.0)),min(log(1.2),100))");
    ASSERT(r2.getType() == EAD::Type::Lognormal);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), log(800.0), EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), log(1.2), EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getExpected(), 813.4076, EPSILON);
  }

  // exponential distribution
  {
    EAD r0 = EAD(EAD::Type::Exponential, 800.0);
    ASSERT(r0.getType() == EAD::Type::Exponential);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 800.0, EPSILON);
    ASSERT(std::isnan(r0.getValue2()));
    ASSERT_EQUALS_EPSILON(r0.getExpected(), 800.0, EPSILON);

    EAD r1("exponential(800.0)");
    ASSERT(r1.getType() == EAD::Type::Exponential);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 800.0, EPSILON);
    ASSERT(std::isnan(r1.getValue2()));
    ASSERT_EQUALS_EPSILON(r1.getExpected(), 800.0, EPSILON);

    EAD r2("exponential(1000-200.0)");
    ASSERT(r2.getType() == EAD::Type::Exponential);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 800.0, EPSILON);
    ASSERT(std::isnan(r2.getValue2()));
    ASSERT_EQUALS_EPSILON(r2.getExpected(), 800.0, EPSILON);
  }

  // uniform distribution
  {
    EAD r0(EAD::Type::Uniform, 0.0, 1.0);
    ASSERT(r0.getType() == EAD::Type::Uniform);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 0.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 1.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getExpected(), 0.5, EPSILON);

    EAD r1(EAD::Type::Uniform, 0.25, 0.5);
    ASSERT(r1.getType() == EAD::Type::Uniform);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 0.25, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 0.5, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getExpected(), 0.375, EPSILON);

    EAD r2("uniform(0.2,0.3)");
    ASSERT(r2.getType() == EAD::Type::Uniform);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 0.3, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getExpected(), 0.25, EPSILON);

    EAD r3("uniform( +0.2, +0.3 )");
    ASSERT(r3.getType() == EAD::Type::Uniform);
    ASSERT_EQUALS_EPSILON(r3.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r3.getValue2(), 0.3, EPSILON);
    ASSERT_EQUALS_EPSILON(r3.getExpected(), 0.25, EPSILON);
  }

  // gamma distribution
  {
    EAD r0(EAD::Type::Gamma, 0.5, 0.25);
    ASSERT(r0.getType() == EAD::Type::Gamma);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 0.5, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 0.25, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getExpected(), 0.125, EPSILON);
  
    EAD r1("gamma(0.2,0.3)");
    ASSERT(r1.getType() == EAD::Type::Gamma);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 0.3, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getExpected(), 0.06, EPSILON);

    EAD r2("gamma( +0.2,0.3 )");
    ASSERT(r2.getType() == EAD::Type::Gamma);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 0.3, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getExpected(), 0.06, EPSILON);
  }

  // normal distribution
  {
    EAD r0(EAD::Type::Normal, 500.0, 25.0);
    ASSERT(r0.getType() == EAD::Type::Normal);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 500.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 25.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getExpected(), 500.0, EPSILON);

    EAD r1("normal(500.0,25)");
    ASSERT(r1.getType() == EAD::Type::Normal);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 500.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 25.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getExpected(), 500.0, EPSILON);

    EAD r2("normal( +500*1 , 25+5*(4-2*2) )");
    ASSERT(r2.getType() == EAD::Type::Normal);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 500.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 25.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getExpected(), 500.0, EPSILON);
  }
}

//===========================================================================
// test2. exceptions
//===========================================================================
void ccruncher_test::EADTest::test2()
{
  // fixed values
  ASSERT_THROW(EAD(EAD::Type::Fixed,-0.5));      // fixed & less than 0
  ASSERT_NO_THROW(EAD(EAD::Type::Fixed,NAN));    // fixed & distinct than [0,1]

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
  ASSERT_THROW(EAD(EAD::Type::Uniform,0.5));           // requires 2 arguments
  ASSERT_THROW(EAD(EAD::Type::Uniform,-0.5,+0.5));     // uniform & value1 < 0.0
  ASSERT_NO_THROW(EAD(EAD::Type::Uniform,+0.5,+1.5));  // uniform & value2 > 1.0
  ASSERT_THROW(EAD(EAD::Type::Uniform,+0.5,+0.25));    // uniform & value2 <= value1
  ASSERT_THROW(EAD("uniform(0.3,0.2)"));         // low > up
  ASSERT_THROW(EAD("uniform(-0.3,0.2)"));        // negative value
  ASSERT_THROW(EAD("uniform(0.2)"));             // requires 2 arguments
  ASSERT_THROW(EAD("uniform(0.2,0.1,0.4)"));     // requires 2 arguments

  // gamma
  ASSERT_THROW(EAD("gamma(10)"));           // requires 2 arguments
  ASSERT_THROW(EAD(EAD::Type::Gamma,0.5));        // requires 2 arguments
  ASSERT_THROW(EAD(EAD::Type::Gamma,-0.5,+0.5));  // gamma & value1 <= 0.0
  ASSERT_THROW(EAD(EAD::Type::Gamma,+0.5,-0.5));  // gamma & value1 <= 0.0

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

//===========================================================================
// test3. comparison
//===========================================================================
void ccruncher_test::EADTest::test3()
{
  ASSERT(EAD("1000") == EAD(1000));
  ASSERT(EAD("1001") != EAD(1000));

  ASSERT(EAD("lognormal(1,2)") == EAD(EAD::Type::Lognormal, 1.0, 2.0));
  ASSERT(EAD("lognormal(2,2)") != EAD(EAD::Type::Lognormal, 1.0, 2.0));

  ASSERT(EAD("exponential(0.2)") == EAD(EAD::Type::Exponential, 0.2));
  ASSERT(EAD("exponential(0.3)") != EAD(EAD::Type::Exponential, 0.2));

  ASSERT(EAD("gamma(0.2,0.3)") == EAD(EAD::Type::Gamma, 0.2, 0.3));
  ASSERT(EAD("gamma(0.2,0.4)") != EAD(EAD::Type::Gamma, 0.2, 0.3));

  ASSERT(EAD("uniform(0.2,0.3)") == EAD(EAD::Type::Uniform, 0.2, 0.3));
  ASSERT(EAD("uniform(0.1,0.3)") != EAD(EAD::Type::Uniform, 0.2, 0.3));

  ASSERT(EAD("normal(0.2,0.3)") == EAD(EAD::Type::Normal, 0.2, 0.3));
  ASSERT(EAD("normal(0.1,0.3)") != EAD(EAD::Type::Normal, 0.2, 0.3));
}

