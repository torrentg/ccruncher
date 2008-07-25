
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
// SurvivalTest.cpp - SurvivalTest code - $Rev$
// --------------------------------------------------------------------------
//
// 2005/05/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/06/28 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added test6()
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/07/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . removed cout's
//
// 2005/07/24 - Gerard Torrent [gerard@fobos.generacio.com]
//   . class CopulaNormal renamed to GaussianCopula
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <iostream>
#include "survival/Survival.hpp"
#include "survival/SurvivalTest.hpp"
#include "math/GaussianCopula.hpp"
#include "utils/Arrays.hpp"
#include "utils/ExpatParser.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::SurvivalTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::SurvivalTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// getRatings
//===========================================================================
Ratings ccruncher_test::SurvivalTest::getRatings()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <ratings>\n\
      <rating name='A' order='1' desc='rating1'/>\n\
      <rating name='E' order='2' desc='default'/>\n\
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
void ccruncher_test::SurvivalTest::test1()
{
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <survival maxmonths='7' epsilon='1e-12'>\n\
      <svalue rating='A' t='0' value='1.00'/>\n\
      <svalue rating='A' t='2' value='0.50'/>\n\
      <svalue rating='A' t='3' value='0.25'/>\n\
      <svalue rating='A' t='5' value='0.10'/>\n\
      <!-- optionally you can add default rating info (value=0 always) -->\n\
    </survival>";
  double svalues[] = { 1.00, 0.75, 0.50, 0.25, 0.175, 0.10, 0.05, 0.00, 0.00};
  int ivalues[] = { 7, 5, 4, 3, 3, 2, 2, 2, 1, 1, 0};

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survival sf(&ratings);
  ASSERT_NO_THROW(xmlparser.parse(xmlcontent, &sf));

  // checking values
  for(int i=0;i<9;i++)
  {
    ASSERT_DOUBLES_EQUAL(svalues[i], sf.evalue(0,i), EPSILON);
    ASSERT_DOUBLES_EQUAL(sf.evalue(1, i), 0.0, EPSILON)
  }

  // checking inverse values
  for(int i=0;i<=10;i++)
  {
    ASSERT_EQUALS(ivalues[i], sf.inverse(0,i/10.0));
    ASSERT_EQUALS(0, sf.inverse(1, i/10.0));
  }
}

//===========================================================================
// test2
//===========================================================================
void ccruncher_test::SurvivalTest::test2()
{
  // non valid survival function (value at t=0 distinct that 1)
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <survival maxmonths='7' epsilon='1e-12'>\n\
      <svalue rating='A' t='0' value='0.98'/>\n\
      <svalue rating='A' t='2' value='0.50'/>\n\
      <svalue rating='A' t='3' value='0.25'/>\n\
      <svalue rating='A' t='5' value='0.10'/>\n\
    </survival>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survival sf(&ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sf));
}

//===========================================================================
// test3
//===========================================================================
void ccruncher_test::SurvivalTest::test3()
{
  // non valid survival function, non monotone
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <survival maxmonths='7' epsilon='1e-12'>\n\
      <svalue rating='A' t='0' value='1.00'/>\n\
      <svalue rating='A' t='2' value='0.50'/>\n\
      <svalue rating='A' t='3' value='0.75'/>\n\
      <svalue rating='A' t='5' value='0.10'/>\n\
    </survival>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survival sf(&ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sf));
}

//===========================================================================
// test4
//===========================================================================
void ccruncher_test::SurvivalTest::test4()
{
  // non valid transition matrix (values out of range [0,1])
  string xmlcontent = "<?xml version='1.0' encoding='ISO-8859-1'?>\n\
    <survival maxmonths='7' epsilon='1e-12'>\n\
      <svalue rating='A' t='0' value='1.00'/>\n\
      <svalue rating='A' t='2' value='-0.50'/>\n\
      <svalue rating='A' t='3' value='-0.75'/>\n\
      <svalue rating='A' t='5' value='-0.85'/>\n\
    </survival>";

  // creating xml
  ExpatParser xmlparser;

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survival sf(&ratings);
  ASSERT_THROW(xmlparser.parse(xmlcontent, &sf));
}

//===========================================================================
// test5 (equals than test1, but with diferent constructor)
//===========================================================================
void ccruncher_test::SurvivalTest::test5()
{
  double mvalues1[] = {1.00, 0.75, 0.50, 0.25, 0.175, 0.10};
  double mvalues2[] = {0.00, 0.00, 0.00, 0.00, 0.000, 0.00};
  double *mvalues[] = {mvalues1, mvalues2};
  int imonths[] = {0, 1, 2, 3, 4, 5};
  double svalues[] = { 1.00, 0.75, 0.50, 0.25, 0.175, 0.10, 0.05, 0.00, 0.00};
  int ivalues[] = { 7, 5, 4, 3, 3, 2, 2, 2, 1, 1, 0};

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survival sf(&ratings, 6, (int *) imonths, (double**) mvalues, 7);

  // checking values
  for(int i=0;i<9;i++)
  {
    ASSERT_DOUBLES_EQUAL(svalues[i], sf.evalue(0,i), EPSILON);
    ASSERT_DOUBLES_EQUAL(sf.evalue(1, i), 0.0, EPSILON)
  }

  // checking inverse values
  for(int i=0;i<=10;i++)
  {
    ASSERT_EQUALS(ivalues[i], sf.inverse(0,i/10.0));
    ASSERT_EQUALS(0, sf.inverse(1, i/10.0));
  }
}

//===========================================================================
// test6 (checks distribution assumptions)
//===========================================================================
void ccruncher_test::SurvivalTest::test6()
{
  double mvalues1[] = {1.00, 0.90};
  double mvalues2[] = {0.00, 0.00};
  double *mvalues[] = {mvalues1, mvalues2};
  int imonths[] = {0, 12};
  int ivalues[] = {0, 0};
  double **id = Arrays<double>::allocMatrix(2,2,0.0);

  // ratings list creation
  Ratings ratings = getRatings();

  // survival function creation
  Survival sf(&ratings, 2, (int *) imonths, (double**) mvalues, 48);

  // creating Id matrix 2x2
  id[0][0] = 1.0;
  id[0][1] = 0.0;
  id[1][0] = 0.0;
  id[1][1] = 1.0;

  // creating randomizer
  GaussianCopula randomizer(2, (double**) id);

  // checking values
  for(int i=0;i<20000;i++)
  {
    if (sf.inverse(0,randomizer.get(0)) > 12) {
      ivalues[0]++;
    }
    else {
      ivalues[1]++;
    }

    randomizer.next();
  }

  // checking that P(X in [0,0.9]) = 0.9
  ASSERT(ivalues[0] > 17500);  // exact value is 18000 (margin=500)
  // checking that P(X in [0.9,1.0]) = 0.1
  ASSERT(ivalues[1] < 2500);  // exact value is 2000 (margin=500)
}
