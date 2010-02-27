
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#include "math/Random.hpp"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Random::Random()
{
  init();
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Random::Random(long seed)
{
  init();
  setSeed(seed);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Random::~Random()
{
  gsl_rng_free(rng);
}

//===========================================================================
// init
//===========================================================================
void ccruncher::Random::init()
{
  rng = gsl_rng_alloc(gsl_rng_mt19937);
}

//===========================================================================
// setSeed
//===========================================================================
void ccruncher::Random::setSeed(long seed)
{
  gsl_rng_set(rng, (unsigned long) seed);
}

//===========================================================================
// nextGaussian
//===========================================================================
double ccruncher::Random::nextGaussian()
{
  return nextGaussian(0.0, 1.0);
}

//===========================================================================
// nextGaussian
//===========================================================================
double ccruncher::Random::nextGaussian(double mu, double sigma)
{
  assert(sigma > 0.0);
  return mu + gsl_ran_gaussian(rng, sigma);
}

//===========================================================================
// nextChisq
//===========================================================================
double ccruncher::Random::nextChisq(double nu)
{
  assert(nu > 0.0);
  return gsl_ran_chisq(rng, nu);
}

//===========================================================================
// nextUnif
//===========================================================================
double ccruncher::Random::nextUnif(double a, double b)
{
  assert(a < b);
  return gsl_ran_flat(rng, a, b);
}

