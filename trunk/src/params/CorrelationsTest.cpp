
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// getSectors
//===========================================================================
Sectors ccruncher_test::CorrelationsTest::getSectors()
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
void ccruncher_test::CorrelationsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <correlations>\n\
      <correlation sector1='S1' sector2='S1' value='0.25'/>\n\
      <correlation sector1='S1' sector2='S2' value='0.05'/>\n\
      <correlation sector1='S2' sector2='S2' value='0.3'/>\n\
    </correlations>";
  double vmatrix[] = {
    0.25, 0.05,
    0.05, 0.30
  };

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors = getSectors();

  // correlation matrix creation
  Correlations crm(sectors);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &crm));

  ASSERT(2 == crm.size());

  for(int i=0; i<2; i++)
  {
    for(int j=0; j<2; j++)
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
  // non valid xml (undefined sector S4)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <correlations>\n\
      <correlation sector1='S1' sector2='S1' value='0.25'/>\n\
      <correlation sector1='S1' sector2='S2' value='0.05'/>\n\
      <correlation sector1='S2' sector2='S4' value='0.3'/>\n\
    </correlations>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors = getSectors();

  // correlation matrix creation
  Correlations crm(sectors);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::CorrelationsTest::test3()
{
  // incomplete matrix
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <correlations>\n\
      <correlation sector1='S1' sector2='S1' value='0.25'/>\n\
      <correlation sector1='S1' sector2='S2' value='0.05'/>\n\
    </correlations>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors = getSectors();

  // correlation matrix creation
  Correlations crm(sectors);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::CorrelationsTest::test4()
{
  // non valid correlation matrix (elements not belonging to (-1,1))
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <correlations>\n\
      <correlation sector1='S1' sector2='S1' value='0.25'/>\n\
      <correlation sector1='S1' sector2='S2' value='1.1'/>\n\
      <correlation sector1='S2' sector2='S2' value='0.3'/>\n\
    </correlations>";

  // creating xml
  ExpatParser xmlparser;

  // sectors list creation
  Sectors sectors = getSectors();

  // correlation matrix creation
  Correlations crm(sectors);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &crm));
}
