
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

#include "transitions/TransitionMatrix.hpp"
#include "transitions/TransitionMatrixTest.hpp"
#include "utils/ExpatParser.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-5

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::TransitionMatrixTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::TransitionMatrixTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::TransitionMatrixTest::getRatings()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' desc='muy bueno'/>\n\
      <rating name='B' desc='bueno'/>\n\
      <rating name='C' desc='regular'/>\n\
      <rating name='D' desc='malo'/>\n\
      <rating name='E' desc='fallido'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;
  Ratings ret;
  xmlparser.parse(xmlcontent, &ret);

  return ret;
}

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::TransitionMatrixTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mtransitions period='12'>\n\
      <transition from='A' to='A' value='0.80'/>\n\
      <transition from='A' to='B' value='0.10'/>\n\
      <transition from='A' to='C' value='0.07'/>\n\
      <transition from='A' to='D' value='0.02'/>\n\
      <transition from='A' to='E' value='0.01'/>\n\
      <transition from='B' to='A' value='0.05'/>\n\
      <transition from='B' to='B' value='0.75'/>\n\
      <transition from='B' to='C' value='0.10'/>\n\
      <transition from='B' to='D' value='0.07'/>\n\
      <transition from='B' to='E' value='0.03'/>\n\
      <transition from='C' to='A' value='0.03'/>\n\
      <transition from='C' to='B' value='0.07'/>\n\
      <transition from='C' to='C' value='0.70'/>\n\
      <transition from='C' to='D' value='0.14'/>\n\
      <transition from='C' to='E' value='0.06'/>\n\
      <transition from='D' to='A' value='0.05'/>\n\
      <transition from='D' to='B' value='0.05'/>\n\
      <transition from='D' to='C' value='0.15'/>\n\
      <transition from='D' to='D' value='0.60'/>\n\
      <transition from='D' to='E' value='0.15'/>\n\
      <transition from='E' to='A' value='0.00'/>\n\
      <transition from='E' to='B' value='0.00'/>\n\
      <transition from='E' to='C' value='0.00'/>\n\
      <transition from='E' to='D' value='0.00'/>\n\
      <transition from='E' to='E' value='1.00'/>\n\
    </mtransitions>";
  double vmatrix12[] = {
    0.80, 0.10, 0.07, 0.02, 0.01,
    0.05, 0.75, 0.10, 0.07, 0.03,
    0.03, 0.07, 0.70, 0.14, 0.06,
    0.05, 0.05, 0.15, 0.60, 0.15,
    0.00, 0.00, 0.00, 0.00, 1.00
  };
  /*
    computed with octave:
    A=[ 0.80, 0.10, 0.07, 0.02, 0.01;
        0.05, 0.75, 0.10, 0.07, 0.03;
        0.03, 0.07, 0.70, 0.14, 0.06;
        0.05, 0.05, 0.15, 0.60, 0.15;
        0.00, 0.00, 0.00, 0.00, 1.00]
    A^(1/12)
  */
  double vmatrix1[] = {
   0.98114,  0.01026,  0.00691,  0.00118,  0.00051,
   0.00491,  0.97536,  0.01037,  0.00739,  0.00197,
   0.00257,  0.00728,  0.96840,  0.01729,  0.00445,
   0.00543,  0.00493,  0.01835,  0.95611,  0.01518,
   0.00000,  0.00000,  0.00000,  0.00000,  1.00000
  };

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // transition matrix creation
  TransitionMatrix trm(ratings);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &trm));

  double **matrix = trm.getMatrix();

  ASSERT(5 == trm.size());

  for(int i=0;i<5;i++)
  {
    for(int j=0;j<5;j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix12[j+i*5], matrix[i][j], EPSILON);
    }
  }

  // testing index default (internal indexation, begins with 0)
  ASSERT(4 == trm.getIndexDefault());

  // testing scale method
  TransitionMatrix aux = trm.scale(12);
  matrix = aux.getMatrix();

  for(int i=0;i<5;i++)
  {
    for(int j=0;j<5;j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix12[j+i*5], matrix[i][j], EPSILON);
    }
  }

  // testing scale method
  aux = trm.scale(1);
  matrix = aux.getMatrix();

  for(int i=0;i<5;i++)
  {
    for(int j=0;j<5;j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix1[j+i*5], matrix[i][j], EPSILON);
    }
  }

}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::TransitionMatrixTest::test2()
{
  // non valid transition matrix (row sum=1 not true)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mtransitions period='12'>\n\
      <transition from='A' to='A' value='0.90'/>\n\
      <transition from='A' to='B' value='0.10'/>\n\
      <transition from='A' to='C' value='0.07'/>\n\
      <transition from='A' to='D' value='0.02'/>\n\
      <transition from='A' to='E' value='0.01'/>\n\
      <transition from='B' to='A' value='0.05'/>\n\
      <transition from='B' to='B' value='0.75'/>\n\
      <transition from='B' to='C' value='0.10'/>\n\
      <transition from='B' to='D' value='0.07'/>\n\
      <transition from='B' to='E' value='0.03'/>\n\
      <transition from='C' to='A' value='0.03'/>\n\
      <transition from='C' to='B' value='0.07'/>\n\
      <transition from='C' to='C' value='0.70'/>\n\
      <transition from='C' to='D' value='0.14'/>\n\
      <transition from='C' to='E' value='0.06'/>\n\
      <transition from='D' to='A' value='0.05'/>\n\
      <transition from='D' to='B' value='0.05'/>\n\
      <transition from='D' to='C' value='0.15'/>\n\
      <transition from='D' to='D' value='0.60'/>\n\
      <transition from='D' to='E' value='0.15'/>\n\
      <transition from='E' to='A' value='0.00'/>\n\
      <transition from='E' to='B' value='0.00'/>\n\
      <transition from='E' to='C' value='0.00'/>\n\
      <transition from='E' to='D' value='0.00'/>\n\
      <transition from='E' to='E' value='1.00'/>\n\
    </mtransitions>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // transition matrix creation
  TransitionMatrix trm(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &trm));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::TransitionMatrixTest::test3()
{
  // non valid xml, refers to non-existents rating (K)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mtransitions period='12'>\n\
      <transition from='K' to='A' value='0.80'/>\n\
      <transition from='A' to='B' value='0.10'/>\n\
      <transition from='A' to='C' value='0.07'/>\n\
      <transition from='A' to='D' value='0.02'/>\n\
      <transition from='A' to='E' value='0.01'/>\n\
      <transition from='B' to='A' value='0.05'/>\n\
      <transition from='B' to='B' value='0.75'/>\n\
      <transition from='B' to='C' value='0.10'/>\n\
      <transition from='B' to='D' value='0.07'/>\n\
      <transition from='B' to='E' value='0.03'/>\n\
      <transition from='C' to='A' value='0.03'/>\n\
      <transition from='C' to='B' value='0.07'/>\n\
      <transition from='C' to='C' value='0.70'/>\n\
      <transition from='C' to='D' value='0.14'/>\n\
      <transition from='C' to='E' value='0.06'/>\n\
      <transition from='D' to='A' value='0.05'/>\n\
      <transition from='D' to='B' value='0.05'/>\n\
      <transition from='D' to='C' value='0.15'/>\n\
      <transition from='D' to='D' value='0.60'/>\n\
      <transition from='D' to='E' value='0.15'/>\n\
      <transition from='E' to='A' value='0.00'/>\n\
      <transition from='E' to='B' value='0.00'/>\n\
      <transition from='E' to='C' value='0.00'/>\n\
      <transition from='E' to='D' value='0.00'/>\n\
      <transition from='E' to='E' value='1.00'/>\n\
    </mtransitions>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // transition matrix creation
  TransitionMatrix trm(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &trm));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::TransitionMatrixTest::test4()
{
  // non valid transition matrix (default element not defined)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mtransitions period='12'>\n\
      <transition from='A' to='A' value='0.80'/>\n\
      <transition from='A' to='B' value='0.10'/>\n\
      <transition from='A' to='C' value='0.07'/>\n\
      <transition from='A' to='D' value='0.02'/>\n\
      <transition from='A' to='E' value='0.01'/>\n\
      <transition from='B' to='A' value='0.05'/>\n\
      <transition from='B' to='B' value='0.75'/>\n\
      <transition from='B' to='C' value='0.10'/>\n\
      <transition from='B' to='D' value='0.07'/>\n\
      <transition from='B' to='E' value='0.03'/>\n\
      <transition from='C' to='A' value='0.03'/>\n\
      <transition from='C' to='B' value='0.07'/>\n\
      <transition from='C' to='C' value='0.70'/>\n\
      <transition from='C' to='D' value='0.14'/>\n\
      <transition from='C' to='E' value='0.06'/>\n\
      <transition from='D' to='A' value='0.05'/>\n\
      <transition from='D' to='B' value='0.05'/>\n\
      <transition from='D' to='C' value='0.15'/>\n\
      <transition from='D' to='D' value='0.60'/>\n\
      <transition from='D' to='E' value='0.15'/>\n\
      <transition from='E' to='A' value='0.50'/>\n\
      <transition from='E' to='B' value='0.25'/>\n\
      <transition from='E' to='C' value='0.25'/>\n\
      <transition from='E' to='D' value='0.00'/>\n\
      <transition from='E' to='E' value='0.00'/>\n\
    </mtransitions>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // transition matrix creation
  TransitionMatrix trm(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &trm));
}

//===========================================================================
// test5
//===========================================================================
void ccruncher_test::TransitionMatrixTest::test5()
{
  // non valid transition matrix (property 4 not acomplished)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <mtransitions period='12'>\n\
      <transition from='A' to='A' value='0.80'/>\n\
      <transition from='A' to='B' value='0.10'/>\n\
      <transition from='A' to='C' value='0.10'/>\n\
      <transition from='A' to='D' value='0.00'/>\n\
      <transition from='A' to='E' value='0.00'/>\n\
      <transition from='B' to='A' value='0.10'/>\n\
      <transition from='B' to='B' value='0.80'/>\n\
      <transition from='B' to='C' value='0.10'/>\n\
      <transition from='B' to='D' value='0.00'/>\n\
      <transition from='B' to='E' value='0.00'/>\n\
      <transition from='C' to='A' value='0.10'/>\n\
      <transition from='C' to='B' value='0.10'/>\n\
      <transition from='C' to='C' value='0.80'/>\n\
      <transition from='C' to='D' value='0.00'/>\n\
      <transition from='C' to='E' value='0.00'/>\n\
      <transition from='D' to='A' value='0.00'/>\n\
      <transition from='D' to='B' value='0.00'/>\n\
      <transition from='D' to='C' value='0.00'/>\n\
      <transition from='D' to='D' value='0.80'/>\n\
      <transition from='D' to='E' value='0.20'/>\n\
      <transition from='E' to='A' value='0.00'/>\n\
      <transition from='E' to='B' value='0.00'/>\n\
      <transition from='E' to='C' value='0.00'/>\n\
      <transition from='E' to='D' value='0.00'/>\n\
      <transition from='E' to='E' value='1.00'/>\n\
    </mtransitions>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // transition matrix creation
  TransitionMatrix trm(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &trm));
}
