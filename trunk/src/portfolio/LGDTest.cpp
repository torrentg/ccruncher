
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#define EPSILON 1E-14

//===========================================================================
// test1. constructors
//===========================================================================
void ccruncher_test::LGDTest::test1()
{
  // fixed values
  {
    LGD r0 = LGD(LGD::Type::Fixed,NAN);
    ASSERT(r0.getType() == LGD::Type::Fixed);
    ASSERT(std::isnan(r0.getValue1()));
    ASSERT(std::isnan(r0.getValue2()));
  
    LGD r1;
    ASSERT(r1.getType() == LGD::Type::Fixed);
    ASSERT(std::isnan(r1.getValue1()));
    ASSERT(std::isnan(r1.getValue2()));
  
    LGD r2(LGD::Type::Fixed,1.0);
    ASSERT(r2.getType() == LGD::Type::Fixed);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 1.0, EPSILON);
    ASSERT(std::isnan(r2.getValue2()));

    LGD r3("0.25");
    ASSERT(r3.getType() == LGD::Type::Fixed);
    ASSERT_EQUALS_EPSILON(r3.getValue1(), 0.25, EPSILON);
    ASSERT(std::isnan(r3.getValue2()));

    LGD r4("0.35");
    ASSERT(r4.getType() == LGD::Type::Fixed);
    ASSERT_EQUALS_EPSILON(r4.getValue1(), 0.35, EPSILON);
    ASSERT(std::isnan(r4.getValue2()));
  }

  // uniform distribution
  {
    LGD r0(LGD::Type::Uniform, 0.0, 1.0);
    ASSERT(r0.getType() == LGD::Type::Uniform);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 0.0, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 1.0, EPSILON);

    LGD r1(LGD::Type::Uniform, 0.25, 0.5);
    ASSERT(r1.getType() == LGD::Type::Uniform);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 0.25, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 0.5, EPSILON);

    LGD r2("uniform(0.2,0.3)");
    ASSERT(r2.getType() == LGD::Type::Uniform);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 0.3, EPSILON);

    LGD r3("uniform( +0.2, +0.3 )");
    ASSERT(r3.getType() == LGD::Type::Uniform);
    ASSERT_EQUALS_EPSILON(r3.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r3.getValue2(), 0.3, EPSILON);
  }

  // beta distribution
  {
    LGD r0(LGD::Type::Beta, 0.5, 0.25);
    ASSERT(r0.getType() == LGD::Type::Beta);
    ASSERT_EQUALS_EPSILON(r0.getValue1(), 0.5, EPSILON);
    ASSERT_EQUALS_EPSILON(r0.getValue2(), 0.25, EPSILON);
  
    LGD r1(LGD::Type::Beta, 0.5, 0.25);
    ASSERT(r1.getType() == LGD::Type::Beta);
    ASSERT_EQUALS_EPSILON(r1.getValue1(), 0.5, EPSILON);
    ASSERT_EQUALS_EPSILON(r1.getValue2(), 0.25, EPSILON);
  
    LGD r2("beta(0.2,0.3)");
    ASSERT(r2.getType() == LGD::Type::Beta);
    ASSERT_EQUALS_EPSILON(r2.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r2.getValue2(), 0.3, EPSILON);

    LGD r3("beta( +0.2, +0.3 )");
    ASSERT(r3.getType() == LGD::Type::Beta);
    ASSERT_EQUALS_EPSILON(r3.getValue1(), 0.2, EPSILON);
    ASSERT_EQUALS_EPSILON(r3.getValue2(), 0.3, EPSILON);
  }
}

//===========================================================================
// test2. ranges
//===========================================================================
void ccruncher_test::LGDTest::test2()
{
  // fixed values
  ASSERT_THROW(LGD(LGD::Type::Fixed,-0.5));       // distinct than [0,1]
  ASSERT_THROW(LGD(LGD::Type::Fixed,+1.5));       // distinct than [0,1]
  ASSERT_THROW(LGD("a"));                         // unrecognized variable
  ASSERT_NO_THROW(LGD(LGD::Type::Fixed,NAN));     // fixed & distinct than [0,1]

  // uniform
  ASSERT_THROW(LGD(LGD::Type::Uniform,-0.5,+0.5));  // out-of range [0,1]
  ASSERT_THROW(LGD(LGD::Type::Uniform,+0.5,+1.5));  // out-of range [0,1]
  ASSERT_THROW(LGD(LGD::Type::Uniform,+0.5,+0.25)); // uniform & value2 <= value1
  ASSERT_THROW(LGD("uniform(0.2,0.3"));             // lacks ending ')'
  ASSERT_THROW(LGD("uniform(0.2)"));                // only 1 argument
  ASSERT_THROW(LGD("uniform(0.2,0.3,0.4"));         // more than 2 arguments

  // beta
  ASSERT_THROW(LGD(LGD::Type::Beta,-0.5,+0.5));   // beta & value1 <= 0.0
  ASSERT_THROW(LGD(LGD::Type::Beta,+0.5,-0.5));   // beta & value1 <= 0.0
  ASSERT_THROW(LGD("beta(0.2,0.3"));              // lacks ending ')'
  ASSERT_THROW(LGD("beta (0.2,0.3)"));            // additional space
  ASSERT_THROW(LGD("beta(a,0.3)"));               // invalid number
  ASSERT_THROW(LGD("beta(+0.2,-0.3)"));           // negative argument
  ASSERT_THROW(LGD("beta(-0.2,+0.3)"));           // negative argument
  ASSERT_THROW(LGD("beta(0.2,)"));                // lacks argument 2
  ASSERT_THROW(LGD("beto(0.5,0.25)"));            // beto != beta
  ASSERT_THROW(LGD("beta[0.5,0.25]"));            // invalid delimiters
  ASSERT_THROW(LGD("beta(0.2)"));                 // only 1 arg
  ASSERT_THROW(LGD("beta(0.2,0.3,0.4)"));         // more than 2 args
}

//===========================================================================
// test3. comparison
//===========================================================================
void ccruncher_test::LGDTest::test3()
{
  ASSERT(LGD("0.5") == LGD("0.5"));
  ASSERT(LGD("0.6") != LGD("0.5"));

  ASSERT(LGD("uniform(0.2,0.3)") == LGD("uniform(0.2,0.3)"));
  ASSERT(LGD("uniform(0.1,0.3)") != LGD("uniform(0.2,0.3)"));

  ASSERT(LGD("beta(0.5,0.25)") == LGD("beta(0.5,0.25)"));
  ASSERT(LGD("beta(0.6,0.25)") != LGD("beta(0.5,0.25)"));
}

