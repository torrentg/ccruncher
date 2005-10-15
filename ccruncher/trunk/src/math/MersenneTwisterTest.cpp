
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
// MersenneTwisterTest.cpp - MersenneTwisterTest code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2004/12/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from cppUnit to MiniCppUnit
//
// 2005/07/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . created ccruncher_test namespace
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <iostream>
#include <string>
#include "math/Normal.hpp"
#include <MersenneTwister.h>
#include "math/MersenneTwisterTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON 0.008
#define NUMBINS 40
#define NUMITERS 10000

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::MersenneTwisterTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::MersenneTwisterTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test1
// it's a statiscal test. probably crunch some times
//===========================================================================
void ccruncher_test::MersenneTwisterTest::test1(void)
{
  MTRand mtw;
  long ybins[NUMBINS];

  for (int i=0;i<NUMBINS;i++)
  {
    ybins[i] = 0L;
  }

  ASSERT_NO_THROW(mtw.seed(4357));

  for(int i=0; i<NUMITERS; i++)
  {
    double val = mtw.rand();

    if (val >= (double)(NUMBINS-1)/(double)NUMBINS)
    {
      ybins[NUMBINS-1]++;
    }
    else
    {
      for (int j=1;j<NUMBINS;j++)
      {
        if (val < j*1.0/NUMBINS)
        {
          ybins[j-1]++;
          break;
        }
      }
    }
  }

  for (int i=0;i<NUMBINS;i++)
  {
    double prob1 = 1.0/(double)NUMBINS;
    double prob2 = (double)ybins[i]/(double)NUMITERS;
    ASSERT_DOUBLES_EQUAL(prob1, prob2, EPSILON);
  }
}

//===========================================================================
// test2
// it's a statiscal test. probably crunch some times
//===========================================================================
void ccruncher_test::MersenneTwisterTest::test2(void)
{
  MTRand mtw;
  double val;
  double xbin[NUMBINS];
  long ybin[NUMBINS];
  double zbin[NUMBINS];
  double prob1, prob2;

  ASSERT_NO_THROW(mtw.seed(4357));

  for (int i=0;i<NUMBINS;i++)
  {
    xbin[i] = -4.0 + (8.0/NUMBINS)*i;
    ybin[i] = 0L;
    zbin[i] = 0.0;
  }

  for (int i=0;i<NUMITERS;i++)
  {
    val = mtw.randNorm();
    if (val < xbin[0])
    {
      ybin[0]++;
    }
    else if (xbin[NUMBINS-1] <= val)
    {
      ybin[NUMBINS-1]++;
    }
    else
    {
      for (int j=1;j<NUMBINS-1;j++)
      {
        if (val < xbin[j])
        {
          ybin[j]++;
          break;
        }
      }
    }
  }

  prob1 = Normal::cdf(xbin[0]);
  prob2 = (double) ybin[0] / (double) NUMITERS;
  ASSERT_DOUBLES_EQUAL(prob1, prob2, EPSILON);

  for (int i=1;i<NUMBINS-1;i++)
  {
    prob1 = Normal::cdf(xbin[i]) - Normal::cdf(xbin[i-1]);
    prob2 = (double) ybin[i] / (double) NUMITERS;
    ASSERT_DOUBLES_EQUAL(prob1, prob2, EPSILON);
  }

  prob1 = 1.0 - Normal::cdf(xbin[NUMBINS-1]);
  prob2 = (double) ybin[NUMBINS-1] / (double) NUMITERS;
  ASSERT_DOUBLES_EQUAL(prob1, prob2, EPSILON);
}
