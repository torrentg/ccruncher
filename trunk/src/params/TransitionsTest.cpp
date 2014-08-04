
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

#include <vector>
#include "params/Transitions.hpp"
#include "params/TransitionsTest.hpp"
#include "utils/ExpatParser.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1E-5

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::TransitionsTest::getRatings()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='very good'/>\n\
      <rating name='B' description='good'/>\n\
      <rating name='C' description='bad'/>\n\
      <rating name='D' description='very bad'/>\n\
      <rating name='E' description='defaulted'/>\n\
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
void ccruncher_test::TransitionsTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <transitions period='12'>\n\
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
      <transition from='E' to='E' value='1.00'/>\n\
    </transitions>";
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
  Transitions trm(ratings);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &trm));
  ASSERT(5 == trm.size());
  for(int i=0; i<5; i++)
  {
    for(int j=0; j<5; j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix12[j+i*5], trm[i][j], EPSILON);
    }
  }

  // testing index default (internal indexation, begins with 0)
  ASSERT(4 == trm.getIndexDefault());

  // testing scale method
  Transitions aux = trm.scale(12);
  for(int i=0; i<5; i++)
  {
    for(int j=0; j<5; j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix12[j+i*5], aux[i][j], EPSILON);
    }
  }

  // testing scale method
  aux = trm.scale(1);
  for(int i=0; i<5; i++)
  {
    for(int j=0; j<5; j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix1[j+i*5], aux[i][j], EPSILON);
    }
  }

}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::TransitionsTest::test2()
{
  // non valid transition matrix (row sum=1 not true)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <transitions period='12'>\n\
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
    </transitions>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // transition matrix creation
  Transitions trm(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &trm));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::TransitionsTest::test3()
{
  // non valid xml, refers to non-existents rating (K)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <transitions period='12'>\n\
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
    </transitions>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // transition matrix creation
  Transitions trm(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &trm));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::TransitionsTest::test4()
{
  // non valid transition matrix (default element not defined)
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <transitions period='12'>\n\
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
    </transitions>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // transition matrix creation
  Transitions trm(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &trm));
}

//===========================================================================
// test5
// checks matrix regularization
// case extracted from paper: Regularization Algorithms for Transition Matrices
// from Alexander Kreinin and Marina Sidelnikova
//
// matlab code:
//
// A=[
// 0.8973, 0.0976, 0.0048, 0.0000, 0.0003, 0.0000, 0.0000, 0.0000;
// 0.0092, 0.8887, 0.0964, 0.0036, 0.0015, 0.0002, 0.0000, 0.0004;
// 0.0008, 0.0224, 0.9059, 0.0609, 0.0077, 0.0021, 0.0000, 0.0002;
// 0.0008, 0.0037, 0.0602, 0.8545, 0.0648, 0.0130, 0.0011, 0.0019;
// 0.0003, 0.0008, 0.0046, 0.0402, 0.8566, 0.0788, 0.0047, 0.0140;
// 0.0001, 0.0004, 0.0016, 0.0053, 0.0586, 0.8406, 0.0274, 0.0660;
// 0.0000, 0.0000, 0.0000, 0.0100, 0.0279, 0.0538, 0.6548, 0.2535;
// 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000;
// ]
// B=A^0.5;
// %7-th row regularization
// for i=1:8
//   lambda=(sum(B(7,:))-1)/8;
//   for j=1:8
//     if (B(7,j) ~= 0)
//       B(7,j) = B(7,j)-lambda;
//     end
//     if (B(7,j) < 0)
//       B(7,j) = 0;
//     end
//   end
// end
//
//===========================================================================
void ccruncher_test::TransitionsTest::test5()
{
  // Moody's ratings
  string xmlcontent = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='Aaa' description='AAA'/>\n\
      <rating name='Aa'  description='AA'/>\n\
      <rating name='A'   description='A'/>\n\
      <rating name='Baa' description='BBB'/>\n\
      <rating name='Ba'  description='BB'/>\n\
      <rating name='B'   description='B'/>\n\
      <rating name='C'   description='C'/>\n\
      <rating name='D'   description='defaulted'/>\n\
    </ratings>";

  // creating xml
  ExpatParser xmlparser;
  Ratings ratings;
  xmlparser.parse(xmlcontent, &ratings);

  // Moody's transition matrix (1-year)
  double vmatrix12[] = {
    0.8973, 0.0976, 0.0048, 0.0000, 0.0003, 0.0000, 0.0000, 0.0000,
    0.0092, 0.8887, 0.0964, 0.0036, 0.0015, 0.0002, 0.0000, 0.0004,
    0.0008, 0.0224, 0.9059, 0.0609, 0.0077, 0.0021, 0.0000, 0.0002,
    0.0008, 0.0037, 0.0602, 0.8545, 0.0648, 0.0130, 0.0011, 0.0019,
    0.0003, 0.0008, 0.0046, 0.0402, 0.8566, 0.0788, 0.0047, 0.0140,
    0.0001, 0.0004, 0.0016, 0.0053, 0.0586, 0.8406, 0.0274, 0.0660,
    0.0000, 0.0000, 0.0000, 0.0100, 0.0279, 0.0538, 0.6548, 0.2535,
    0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 1.0000
  };
  // Moody's transition matrix (6-months, regularized)
  double QOM[] = {
   0.94711,  0.05164,  0.00113,  0.00000,  0.00012,  0.00000,  0.00000,  0.00000,
   0.00486,  0.94226,  0.05090,  0.00104,  0.00069,  0.00006,  0.00000,  0.00020,
   0.00038,  0.01179,  0.95092,  0.03244,  0.00348,  0.00093,  0.00000,  0.00006,
   0.00041,  0.00176,  0.03203,  0.92341,  0.03490,  0.00623,  0.00053,  0.00073,
   0.00015,  0.00039,  0.00206,  0.02166,  0.92436,  0.04271,  0.00231,  0.00636,
   0.00005,  0.00020,  0.00078,  0.00245,  0.03166,  0.91583,  0.01584,  0.03319,
   0.00000,  0.00000,  0.00000,  0.00551,  0.01539,  0.03076,  0.80884,  0.13949,
   0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  0.00000,  1.00000
  };

  vector<vector<double>> values(8, vector<double>(8));
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++) 
    {
      values[i][j] = vmatrix12[j+i*8];
    }
  }

  // transition matrix creation (1-year)
  Transitions t12(ratings, values, 12);
  ASSERT(t12.size() == 8);
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      ASSERT_EQUALS_EPSILON(vmatrix12[j+i*8], t12[i][j], EPSILON);
    }
  }

  // regularized transition matrix (6-months)
  Transitions t6 = t12.scale(6);
  ASSERT(t6.size() == 8);
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      ASSERT_EQUALS_EPSILON(QOM[j+i*8], t6[i][j], EPSILON);
    }
  }
}

//===========================================================================
// test6
// checks getDefaultProbabilities method
//===========================================================================
void ccruncher_test::TransitionsTest::test6()
{
  string xmlcontent1 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A' description='alive'/>\n\
      <rating name='E' description='dead'/>\n\
    </ratings>";

  string xmlcontent2 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <transitions period='12'>\n\
      <transition from='A' to='A' value='90%'/>\n\
      <transition from='A' to='E' value='10%'/>\n\
      <transition from='E' to='A' value='0%'/>\n\
      <transition from='E' to='E' value='100%'/>\n\
    </transitions>";

  // creating objects
  ExpatParser xmlparser;
  Ratings ratings;
  xmlparser.parse(xmlcontent1, &ratings);
  Transitions transitions(ratings);
  xmlparser.parse(xmlcontent2, &transitions);
  Transitions tone = transitions.scale(1);
  vector<CDF> dprobs = tone.getCDFs(Date("1/1/2010"), transitions.getPeriod()+1);
  ASSERT_EQUALS_EPSILON(0.0, dprobs[0].evalue(0.0), EPSILON);
  ASSERT_EQUALS_EPSILON(0.1, dprobs[0].evalue(365.0), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0, dprobs[1].evalue(0.0), EPSILON);
  ASSERT_EQUALS_EPSILON(1.0, dprobs[1].evalue(365.0), EPSILON);
}

//===========================================================================
// test7
// checks for non-absorbing markov chain
//===========================================================================
void ccruncher_test::TransitionsTest::test7()
{
  string xmlcontent1 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <ratings>\n\
      <rating name='A'/>\n\
      <rating name='B'/>\n\
      <rating name='C'/>\n\
      <rating name='D'/>\n\
    </ratings>";

  string xmlcontent2 = "<?xml version='1.0' encoding='UTF-8'?>\n\
    <transitions period='12'>\n\
      <transition from='A' to='A' value='90%'/>\n\
      <transition from='A' to='B' value='10%'/>\n\
      <transition from='B' to='A' value='10%'/>\n\
      <transition from='B' to='B' value='90%'/>\n\
      <transition from='C' to='C' value='90%'/>\n\
      <transition from='C' to='D' value='10%'/>\n\
      <transition from='D' to='D' value='100%'/>\n\
    </transitions>";

  // creating objects
  ExpatParser xmlparser;
  Ratings ratings;
  xmlparser.parse(xmlcontent1, &ratings);
  Transitions transitions(ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent2, &transitions));
}
