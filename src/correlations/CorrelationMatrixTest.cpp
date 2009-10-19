
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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
#include "correlations/CorrelationMatrix.hpp"
#include "correlations/CorrelationMatrixTest.hpp"
#include "utils/ExpatParser.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::CorrelationMatrixTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::CorrelationMatrixTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// getSectors
//===========================================================================
Sectors ccruncher_test::CorrelationMatrixTest::getSectors()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <sectors>\n\
      <sector name='S1' description='retail'/>\n\
      <sector name='S2' description='others'/>\n\
    </sectors>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors ret;
  xmlparser.parse(xmlcontent, &ret);

  return ret;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::CorrelationMatrixTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mcorrels>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.05'/>\n\
      <sigma sector1='S2' sector2='S2' value='0.3'/>\n\
    </mcorrels>";
  double vmatrix[] = {
    0.25, 0.05,
    0.05, 0.30
  };

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors = getSectors();

  // correlation matrix creation
  CorrelationMatrix crm(sectors);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &crm));

  double **matrix = crm.getMatrix();

  ASSERT(2 == crm.size());

  for(int i=0;i<2;i++)
  {
    for(int j=0;j<2;j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix[j+i*2], matrix[i][j], EPSILON);
    }
  }
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::CorrelationMatrixTest::test2()
{
  // non valid xml (undefined sector S4)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mcorrels>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.05'/>\n\
      <sigma sector1='S2' sector2='S4' value='0.3'/>\n\
    </mcorrels>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors = getSectors();

  // correlation matrix creation
  CorrelationMatrix crm(sectors);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::CorrelationMatrixTest::test3()
{
  // incomplete matrix
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mcorrels>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.05'/>\n\
    </mcorrels>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors = getSectors();

  // correlation matrix creation
  CorrelationMatrix crm(sectors);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::CorrelationMatrixTest::test4()
{
  // non valid correlation matrix (elements not belonging to (-1,1))
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mcorrels>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='1.1'/>\n\
      <sigma sector1='S2' sector2='S2' value='0.3'/>\n\
    </mcorrels>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors = getSectors();

  // correlation matrix creation
  CorrelationMatrix crm(sectors);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}
