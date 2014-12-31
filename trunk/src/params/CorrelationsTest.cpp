
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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

#include <iostream>
#include "params/Correlations.hpp"
#include "params/CorrelationsTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1E-14

//===========================================================================
// getFactors
//===========================================================================
Factors ccruncher_test::CorrelationsTest::getFactors()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <factors>\n\
      <factor name='S1' loading='0.25' description='retail'/>\n\
      <factor name='S2' loading='0.3' description='others'/>\n\
    </factors>";

  // creating xml
  ExpatParser xmlparser;

  // factors list creation
  Factors ret;
  xmlparser.parse(xmlcontent, &ret);

  return ret;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::CorrelationsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <correlations>\n\
      <correlation factor1='S1' factor2='S2' value='0.05'/>\n\
    </correlations>";
  double vmatrix[] = {
    1.00, 0.05,
    0.05, 1.00
  };

  // factors list creation
  Factors factors = getFactors();

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().factors = &factors;

  // correlation matrix creation
  Correlations crm;
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &crm));

  ASSERT(2 == crm.size());

  for(size_t i=0; i<2; i++)
  {
    for(size_t j=0; j<2; j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix[j+i*2], crm[i][j], EPSILON);
    }
  }
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::CorrelationsTest::test2()
{
  // non valid xml (undefined factor S4)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <correlations>\n\
      <correlation factor1='S1' factor2='S4' value='0.05'/>\n\
    </correlations>";

  // factors list creation
  Factors factors = getFactors();

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().factors = &factors;

  // correlation matrix creation
  Correlations crm(factors.size());
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::CorrelationsTest::test3()
{
  // incomplete matrix
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <correlations/>";

  // factors list creation
  Factors factors = getFactors();

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().factors = &factors;

  // correlation matrix creation
  Correlations crm(factors.size());
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::CorrelationsTest::test4()
{
  // non valid correlation matrix (elements out of range (-1,1))
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <correlations>\n\
      <correlation factor1='S1' factor2='S2' value='1.1'/>\n\
    </correlations>";

  // factors list creation
  Factors factors = getFactors();

  // creating xml
  ExpatParser xmlparser;
  xmlparser.getUserData().factors = &factors;

  // correlation matrix creation
  Correlations crm(factors.size());
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::CorrelationsTest::test5()
{
  // created from scratch
  Correlations correlations(2);
  correlations[0][0] = 1.0;
  correlations[0][1] = 0.05;
  correlations[1][0] = 0.05;
  correlations[1][1] = 1.0;
  ASSERT(Correlations::isValid(correlations));
  gsl_matrix *chol = nullptr;
  ASSERT_NO_THROW(chol = Correlations::getCholesky(correlations));
  gsl_matrix_free(chol);
}

//===========================================================================
// test6
// non definite-positive
//===========================================================================
void ccruncher_test::CorrelationsTest::test6()
{
  Correlations correlations(2);
  correlations[0][0] = 1.0;
  correlations[0][1] = 1.1;
  correlations[1][0] = 1.1;
  correlations[1][1] = 1.0;
  ASSERT(!Correlations::isValid(correlations));
  gsl_matrix *chol = nullptr;
  ASSERT_THROW(chol = Correlations::getCholesky(correlations));
  ASSERT(chol == nullptr)
}

