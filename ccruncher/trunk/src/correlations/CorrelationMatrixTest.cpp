
//***************************************************************************
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
// CorrelationMatrixTest.cpp - CorrelationMatrixTest code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
//***************************************************************************

#include <iostream>
#include "CorrelationMatrix.hpp"
#include "CorrelationMatrixTest.hpp"
#include "utils/XMLUtils.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//***************************************************************************
// setUp
//***************************************************************************
void CorrelationMatrixTest::setUp()
{
  XMLUtils::initialize();
}

//***************************************************************************
// setUp
//***************************************************************************
void CorrelationMatrixTest::tearDown()
{
  XMLUtils::terminate();
}

//***************************************************************************
// getSectors
//***************************************************************************
Sectors CorrelationMatrixTest::getSectors()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <sectors>\n\
      <sector name='S1' order='1' desc='calzado'/>\n\
      <sector name='S2' order='2' desc='otros sectores'/>\n\
    </sectors>";

  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // sectors list creation 
  Sectors ret = Sectors(*(doc->getDocumentElement()));

  delete wis;
  delete parser;
  return ret;
}

//***************************************************************************
// test1
//***************************************************************************
void CorrelationMatrixTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <mcorrels epsilon='1e-12'>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.05'/>\n\
      <sigma sector1='S2' sector2='S2' value='0.3'/>\n\
    </mcorrels>";
  double vmatrix[] = {
    0.25, 0.05,
    0.05, 0.30
  };

  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // sectors list creation 
  Sectors sectors = getSectors();
  
  // correlation matrix creation
  CorrelationMatrix *crm = NULL;
  ASSERT_NO_THROW(crm = new CorrelationMatrix(&sectors, *(doc->getDocumentElement())));
  double **matrix = crm->getMatrix();

  ASSERT(2 == crm->size());

  for(int i=0;i<2;i++)
  {
    for(int j=0;j<2;j++)
    {
      ASSERT_DOUBLES_EQUAL(vmatrix[j+i*2], matrix[i][j], EPSILON);
    }
  }
  
  if (crm != NULL) delete crm;
  delete wis;
  delete parser;
}

//***************************************************************************
// test2
//***************************************************************************
void CorrelationMatrixTest::test2()
{
  // non valid xml (undefined sector S4)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <mcorrels epsilon='1e-12'>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.05'/>\n\
      <sigma sector1='S2' sector2='S4' value='0.3'/>\n\
    </mcorrels>";

  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // sectors list creation 
  Sectors sectors = getSectors();
  
  // correlation matrix creation
  ASSERT_THROW(CorrelationMatrix(&sectors, *(doc->getDocumentElement())));

  delete wis;
  delete parser;
}

//***************************************************************************
// test3
//***************************************************************************
void CorrelationMatrixTest::test3()
{
  // incomplete matrix
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <mcorrels epsilon='1e-12'>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.05'/>\n\
    </mcorrels>";

  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // sectors list creation 
  Sectors sectors = getSectors();
  
  // correlation matrix creation
  ASSERT_THROW(CorrelationMatrix(&sectors, *(doc->getDocumentElement())));

  delete wis;
  delete parser;
}

//***************************************************************************
// test4
//***************************************************************************
void CorrelationMatrixTest::test4()
{
  // non valid correlation matrix (non definite positive)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <mcorrels epsilon='1e-12'>\n\
      <sigma sector1='S1' sector2='S1' value='0.25'/>\n\
      <sigma sector1='S1' sector2='S2' value='0.95'/>\n\
      <sigma sector1='S2' sector2='S2' value='0.3'/>\n\
    </mcorrels>";

  // creating xml
  DOMBuilder *parser = XMLUtils::getParser();
  Wrapper4InputSource *wis = XMLUtils::getInputSource(xmlcontent);
  DOMDocument *doc = XMLUtils::getDocument(parser, wis);

  // sectors list creation 
  Sectors sectors = getSectors();
  
  // correlation matrix creation
  ASSERT_THROW(CorrelationMatrix(&sectors, *(doc->getDocumentElement())));

  delete wis;
  delete parser;
}
