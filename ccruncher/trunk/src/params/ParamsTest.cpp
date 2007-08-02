
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// ParamsTest.cpp - ParamsTest code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added param montecarlo.method
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/08/12 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed copula identifier: normal -> gaussian
//
// 2005/09/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added param montecarlo.simule
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/05 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed simulate=values option
//   . removed method=rating-path
//
//===========================================================================

#include "params/Params.hpp"
#include "params/ParamsTest.hpp"
#include "utils/ExpatParser.hpp"

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::ParamsTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::ParamsTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::ParamsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <params>\n\
      <property name='time.begindate' value='18/02/2003'/>\n\
      <property name='time.steps' value='12'/>\n\
      <property name='time.steplength' value='2'/>\n\
      <property name='stopcriteria.maxiterations' value='3000'/>\n\
      <property name='stopcriteria.maxseconds' value='30000000'/>\n\
      <property name='copula.type' value='gaussian'/>\n\
      <property name='copula.seed' value='38765874'/>\n\
      <property name='montecarlo.antithetic' value='true'/>\n\
    </params>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Params params;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &params));

  ASSERT(Date("18/02/2003") == params.begindate);
  ASSERT(12 == params.steps);
  ASSERT(2 == params.steplength);
  ASSERT(3000 == params.maxiterations);
  ASSERT(30000000 == params.maxseconds);
  ASSERT("gaussian" == params.copula_type);
  ASSERT(38765874L == params.copula_seed);
  ASSERT(true == params.antithetic);

  Date *dates = NULL;
  ASSERT_NO_THROW(dates = params.getDates());

  if (dates != NULL)
  {
    ASSERT(Date("18/02/2003") == dates[0]);
    ASSERT(Date("18/04/2003") == dates[1]);
    ASSERT(Date("18/06/2003") == dates[2]);
    ASSERT(Date("18/08/2003") == dates[3]);
    ASSERT(Date("18/10/2003") == dates[4]);
    ASSERT(Date("18/12/2003") == dates[5]);
    ASSERT(Date("18/02/2004") == dates[6]);
    ASSERT(Date("18/04/2004") == dates[7]);
    ASSERT(Date("18/06/2004") == dates[8]);
    ASSERT(Date("18/08/2004") == dates[9]);
    ASSERT(Date("18/10/2004") == dates[10]);
    ASSERT(Date("18/12/2004") == dates[11]);
    ASSERT(Date("18/02/2005") == dates[12]);
    delete [] dates;
  }
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::ParamsTest::test2()
{
  // error: steplength not given
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <params>\n\
      <property name='time.begindate' value='18/02/2003'/>\n\
      <property name='time.steps' value='12'/>\n\
      <property name='stopcriteria.maxiterations' value='3000'/>\n\
      <property name='stopcriteria.maxseconds' value='30000000'/>\n\
      <property name='copula.type' value='gaussian'/>\n\
      <property name='copula.seed' value='38765874'/>\n\
      <property name='montecarlo.antithetic' value='true'/>\n\
    </params>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Params params;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &params));
}
