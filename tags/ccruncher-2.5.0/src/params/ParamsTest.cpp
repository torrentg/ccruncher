
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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
#include "params/Params.hpp"
#include "params/ParamsTest.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-14

//===========================================================================
// test1 (default values)
//===========================================================================
void ccruncher_test::ParamsTest::test1()
{
  Params params;

  params.setTime0(Date("01/01/2016"));
  params.setTimeT(Date("01/01/2017"));

  ASSERT(params.isValid());
  ASSERT_NO_THROW(params.isValid(true));

  ASSERT(Date("01/01/2016") == params.getTime0());
  ASSERT(Date("01/01/2017") == params.getTimeT());
  ASSERT(params.getAntithetic());
  ASSERT_EQUALS((unsigned short)128, params.getBlockSize());
  ASSERT_EQUALS("gaussian", params.getCopula());
  ASSERT(isinf(params.getNdf()));
  ASSERT_EQUALS((size_t)1000000, params.getMaxIterations());
  ASSERT_EQUALS((size_t)0, params.getMaxSeconds());
  ASSERT_EQUALS(0UL, params.getRngSeed());
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::ParamsTest::test2()
{
  Params params;

  params.setTime0(Date("01/01/2016"));
  params.setTimeT(Date("01/01/2017"));
  params.setAntithetic(false);
  params.setBlockSize(15);
  params.setCopula("t(13)");
  params.setMaxIterations(20000);
  params.setMaxSeconds(3600);
  params.setRngSeed(1234567);

  ASSERT(params.isValid());
  ASSERT_NO_THROW(params.isValid(true));

  ASSERT(Date("01/01/2016") == params.getTime0());
  ASSERT(Date("01/01/2017") == params.getTimeT());
  ASSERT(!params.getAntithetic());
  ASSERT_EQUALS((unsigned short)15, params.getBlockSize());
  ASSERT_EQUALS("t(13)", params.getCopula());
  ASSERT_EQUALS_EPSILON(13.0, params.getNdf(), EPSILON);
  ASSERT_EQUALS((size_t)20000, params.getMaxIterations());
  ASSERT_EQUALS((size_t)3600, params.getMaxSeconds());
  ASSERT_EQUALS(1234567UL, params.getRngSeed());
}

//===========================================================================
// test3 (non valid cases)
//===========================================================================
void ccruncher_test::ParamsTest::test3()
{
  Params params1;
  ASSERT(!params1.isValid());
  ASSERT_THROW(params1.isValid(true));

  Params params2;
  params2.setTime0(Date("01/01/2017"));
  params2.setTimeT(Date("01/01/2016"));
  ASSERT(!params2.isValid());

  Params params3;
  params3.setTime0(Date("01/01/1800"));
  params3.setTimeT(Date("01/01/2016"));
  ASSERT(!params3.isValid());

  Params params4;
  params4.setTime0(Date("01/01/2015"));
  params4.setTimeT(Date("01/01/2016"));
  ASSERT_THROW(params4.setCopula("XXX"));

  Params params5;
  params5.setTime0(Date("01/01/2015"));
  params5.setTimeT(Date("01/01/2016"));
  params5.setAntithetic(true);
  params5.setBlockSize(127);
  ASSERT(!params5.isValid());
}

