
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

#include "params/Factors.hpp"
#include "params/FactorsTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::FactorsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <factors>\n\
      <factor name='S1' loading='10%' description='retail'/>\n\
      <factor name='S2' loading='20%'/>\n\
    </factors>";

  // creating xml
  ExpatParser xmlparser;

  // factors creation
  Factors factors;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &factors));

  ASSERT(2 == factors.size());

  ASSERT_EQUALS(0, factors.indexOf("S1"));
  ASSERT_EQUALS(1, factors.indexOf("S2"));

  ASSERT("S1" == factors[0].getName());
  ASSERT("S2" == factors[1].getName());

  ASSERT_EQUALS_EPSILON(factors[0].getLoading(), 0.1, 1e-14)
  ASSERT_EQUALS_EPSILON(factors[1].getLoading(), 0.2, 1e-14)

  vector<double> v = factors.getLoadings();
  ASSERT(v.size() == 2);
  ASSERT_EQUALS_EPSILON(v[0], 0.1, 1e-14)
  ASSERT_EQUALS_EPSILON(v[1], 0.2, 1e-14)

  ASSERT("retail" == factors[0].getDescription());
  ASSERT("" == factors[1].getDescription());

  ASSERT(factors[0].getName() == "S1");
  ASSERT(factors[1].getName() == "S2");
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::FactorsTest::test2()
{
  // xml with error (repeated factor)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <factors>\n\
      <factor name='S1' loading='10%' description='retail'/>\n\
      <factor name='S1' loading='20%' description='others'/>\n\
    </factors>";

  // creating xml
  ExpatParser xmlparser;

  // factors creation
  Factors factors;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &factors));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::FactorsTest::test3()
{
  // xml with error (loading out of range)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <factors>\n\
      <factor name='S1' loading='10%' description='retail'/>\n\
      <factor name='S2' loading='120%' description='retail'/>\n\
    </factors>";

  // creating xml
  ExpatParser xmlparser;

  // factors creation
  Factors factors;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &factors));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::FactorsTest::test4()
{
  // xml with error (tag factir)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <factors>\n\
      <factor name='S1' loading='10%' description='retail'/>\n\
      <factir name='S2' loading='20%' description='others'/>\n\
    </factors>";

  // creating xml
  ExpatParser xmlparser;

  // factors creation
  Factors factors;
  ASSERT_THROW(xmlparser.parse(xmlcontent, &factors));
}
