
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
      <parameter name='maxiterations' value='3000'/>\n\
      <parameter name='maxseconds' value='30000000'/>\n\
      <parameter name='copula' value='gaussian'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='antithetic' value='true'/>\n\
      <parameter name='blocksize' value='256'/>\n\
    </parameters>";

  ExpatParser xmlparser;
  Params params;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &params));

  ASSERT(Date("18/02/2003") == params.getTime0());
  ASSERT(Date("18/02/2008") == params.getTimeT());
  ASSERT(3000 == params.getMaxIterations());
  ASSERT(30000000 == params.getMaxSeconds());
  ASSERT("gaussian" == params.getCopula());
  ASSERT(38765874L == params.getRngSeed());
  ASSERT(true == params.getAntithetic());
  ASSERT(256 == params.getBlockSize());
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
      <parameter name='maxiterations' value='3000'/>\n\
      <parameter name='maxseconds' value='30000000'/>\n\
      <parameter name='copula' value='gaussian'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='antithetic' value='true'/>\n\
    </parameters>";

  ExpatParser xmlparser;
  Params params;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &params));
}

//===========================================================================
// test3
// test copula param (case t-student)
//===========================================================================
void ccruncher_test::ParamsTest::test3()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <parameters>\n\
      <parameter name='time.0' value='18/02/2003'/>\n\
      <parameter name='time.T' value='12/08/2010'/>\n\
      <parameter name='maxiterations' value='3000'/>\n\
      <parameter name='maxseconds' value='30000000'/>\n\
      <parameter name='copula' value='t(3)'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='antithetic' value='true'/>\n\
    </parameters>";

  ExpatParser xmlparser;
  Params params;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &params));

  ASSERT(params.getNdf() == 3.0);
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
      <parameter name='maxiterations' value='3000'/>\n\
      <parameter name='maxseconds' value='30000000'/>\n\
      <parameter name='copula' value='gaussian'/>\n\
      <parameter name='rng.seed' value='38765874'/>\n\
      <parameter name='antithetic' value='true'/>\n\
      <parameter name='blocksize' value='257'/>\n\
    </parameters>";

  ExpatParser xmlparser;
  Params params;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &params));
}

