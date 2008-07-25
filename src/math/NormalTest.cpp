
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
// NormalTest.cpp - NormalTest code - $Rev$
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
#include <cmath>
#include "math/Normal.hpp"
#include "math/NormalTest.hpp"

//---------------------------------------------------------------------------

#define EPSILON1 0.0001
#define NUMITERS 10000
#define NUMBINS 41

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::NormalTest::setUp()
{
  // nothing to do
}

//===========================================================================
// setUp
//===========================================================================
void ccruncher_test::NormalTest::tearDown()
{
  // nothing to do
}

//===========================================================================
// test probability distribution function
// validated using octave command: normal_pdf(x, mu, sigma)
//===========================================================================
void ccruncher_test::NormalTest::test_pdf(void)
{
  double x1[13] = {
    -2.00    ,
    -1.00    ,
    -0.50    ,
    -0.25    ,
    -0.10    ,
    -0.05    ,
    +0.00    ,
    +0.05    ,
    +0.10    ,
    +0.25    ,
    +0.50    ,
    +1.00    ,
    +2.00
  };
  double y1[13] = {+0.053991,
    +0.24197 ,
    +0.35207 ,
    +0.38667 ,
    +0.39695 ,
    +0.39844 ,
    +0.39894 ,
    +0.39844 ,
    +0.39695 ,
    +0.38667 ,
    +0.35207 ,
    +0.24197 ,
    +0.053991
  };

  for (int i=0;i<13;i++)
  {
    ASSERT_EQUALS_EPSILON(y1[i], Normal::pdf(x1[i]), EPSILON1);
  }
}

//===========================================================================
// test cumulative distribution function
// validated using octave command: normal_cdf(x, mu, sigma)
//===========================================================================
void ccruncher_test::NormalTest::test_cdf(void)
{
  double x1[13] = {
    -2.00    ,
    -1.00    ,
    -0.50    ,
    -0.25    ,
    -0.10    ,
    -0.05    ,
    +0.00    ,
    +0.05    ,
    +0.10    ,
    +0.25    ,
    +0.50    ,
    +1.00    ,
    +2.00
  };
  double y1[13] = {
    +0.022750,
    +0.15866 ,
    +0.30854 ,
    +0.40129 ,
    +0.46017 ,
    +0.48006 ,
    +0.5     ,
    +0.51994 ,
    +0.53983 ,
    +0.59871 ,
    +0.69146 ,
    +0.84134 ,
    +0.97725
  };

  for (int i=0;i<13;i++)
  {
    ASSERT_EQUALS_EPSILON(y1[i], Normal::cdf(x1[i]), EPSILON1);
  }
}

//===========================================================================
// test cumulative distribution inverse function
// validated using octave command: normal_inv(x, mu, sigma)
//===========================================================================
void ccruncher_test::NormalTest::test_cdfinv(void)
{
  double x1[7] = {
    +0.01   ,
    +0.05   ,
    +0.10   ,
    +0.25   ,
    +0.50   ,
    +0.85   ,
    +0.99
  };
  double y1[7] = {
    -2.3263 ,
    -1.6449 ,
    -1.2816 ,
    -0.67449,
    +0.0    ,
    +1.0364 ,
    +2.3263
  };

  for (int i=0;i<7;i++)
  {
    ASSERT_EQUALS_EPSILON(y1[i], Normal::cdfinv(x1[i]), EPSILON1);
  }

  ASSERT(isnan(Normal::cdfinv(-1.0)));
  ASSERT(isnan(Normal::cdfinv(+2.0)));
}
