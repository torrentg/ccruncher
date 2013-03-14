
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

#include "params/Params.hpp"
#include "params/ParamsTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::ParamsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <parameters>\n\
      <parameter name='time.0' value='18/02/2003'/>\n\
      <parameter name='time.T' value='18/02/2008'/>\n\
      <parameter name='stopcriteria.maxiterations' value='3000'/>\n\
      <parameter name='stopcriteria.maxseconds' value='30000000'/>\n\
      <parameter name='copula.type' value='gaussian'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='antithetic' value='true'/>\n\
      <parameter name='lhs' value='false'/>\n\
      <parameter name='portfolio.onlyActiveObligors' value='false'/>\n\
      <parameter name='blocksize' value='256'/>\n\
    </parameters>";

  ExpatParser xmlparser;
  Params params;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &params));

  ASSERT(Date("18/02/2003") == params.time0);
  ASSERT(Date("18/02/2008") == params.timeT);
  ASSERT(3000 == params.maxiterations);
  ASSERT(30000000 == params.maxseconds);
  ASSERT("gaussian" == params.copula_type);
  ASSERT("gaussian" == params.getCopulaType());
  ASSERT(38765874L == params.rng_seed);
  ASSERT(true == params.antithetic);
  ASSERT(1 == params.lhs_size);
  ASSERT(false == params.onlyactive);
  ASSERT(256 == params.blocksize);
}

//===========================================================================
// test2
// error: time.T < time.0
//===========================================================================
void ccruncher_test::ParamsTest::test2()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <parameters>\n\
      <parameter name='time.0' value='18/02/2003'/>\n\
      <parameter name='time.T' value='21/03/2001'/>\n\
      <parameter name='stopcriteria.maxiterations' value='3000'/>\n\
      <parameter name='stopcriteria.maxseconds' value='30000000'/>\n\
      <parameter name='copula.type' value='gaussian'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='antithetic' value='true'/>\n\
    </parameters>";

  ExpatParser xmlparser;
  Params params;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &params));
}

//===========================================================================
// test3
// test copula_type param (case t-student)
//===========================================================================
void ccruncher_test::ParamsTest::test3()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <parameters>\n\
      <parameter name='time.0' value='18/02/2003'/>\n\
      <parameter name='time.T' value='12/08/2010'/>\n\
      <parameter name='stopcriteria.maxiterations' value='3000'/>\n\
      <parameter name='stopcriteria.maxseconds' value='30000000'/>\n\
      <parameter name='copula.type' value='t(3)'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='antithetic' value='true'/>\n\
    </parameters>";

  ExpatParser xmlparser;
  Params params;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &params));

  ASSERT(params.getCopulaType() == "t");
  ASSERT(params.getCopulaParam() == 3.0);
}

//===========================================================================
// test4
// antithetic + blocksize even = exception
//===========================================================================
void ccruncher_test::ParamsTest::test4()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <parameters>\n\
      <parameter name='time.0' value='18/02/2003'/>\n\
      <parameter name='time.T' value='18/02/2008'/>\n\
      <parameter name='stopcriteria.maxiterations' value='3000'/>\n\
      <parameter name='stopcriteria.maxseconds' value='30000000'/>\n\
      <parameter name='copula.type' value='gaussian'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='antithetic' value='true'/>\n\
      <parameter name='lhs' value='false'/>\n\
      <parameter name='portfolio.onlyActiveObligors' value='false'/>\n\
      <parameter name='blocksize' value='257'/>\n\
    </parameters>";

  ExpatParser xmlparser;
  Params params;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &params));
}

