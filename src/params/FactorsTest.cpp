
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

  ASSERT_EQUALS((unsigned char)0, factors.indexOf("S1"));
  ASSERT_EQUALS((unsigned char)1, factors.indexOf("S2"));

  ASSERT("S1" == factors[0].name);
  ASSERT("S2" == factors[1].name);

  ASSERT_EQUALS_EPSILON(factors[0].loading, 0.1, 1e-14)
  ASSERT_EQUALS_EPSILON(factors[1].loading, 0.2, 1e-14)

  vector<double> v = Factors::getLoadings(factors);
  ASSERT(v.size() == 2);
  ASSERT_EQUALS_EPSILON(v[0], 0.1, 1e-14)
  ASSERT_EQUALS_EPSILON(v[1], 0.2, 1e-14)

  ASSERT("retail" == factors[0].description);
  ASSERT("" == factors[1].description);

  ASSERT(factors[0].name == "S1");
  ASSERT(factors[1].name == "S2");
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

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::FactorsTest::test5()
{
// creation from scratch
  Factors factors;
  factors.push_back(Factor("S1", 0.35, "industry"));
  factors.push_back(Factor("S2", 0.2, "energy"));
  ASSERT(Factors::isValid(factors));
  vector<double> loadings = Factors::getLoadings(factors);
  ASSERT(Factors::isValid(loadings));
  ASSERT_EQUALS(2ul, factors.size());
  ASSERT_EQUALS((unsigned char)0, factors.indexOf("S1"));
  ASSERT_EQUALS((unsigned char)1, factors.indexOf("S2"));
  ASSERT_THROW(factors.indexOf("Sx"));
}

//===========================================================================
// test6
//===========================================================================
void ccruncher_test::FactorsTest::test6()
{
// creation from scratch
  Factors factors;
  factors.push_back(Factor("S1", 0.35, "industry"));
  factors.push_back(Factor("S2", 0.2, "energy"));
  factors.push_back(Factor("S1", 0.2, "coco")); // repeated identifier
  ASSERT(!Factors::isValid(factors));
  vector<double> loadings = Factors::getLoadings(factors);
  loadings[0] += 1.0;
  ASSERT(!Factors::isValid(loadings));
}

