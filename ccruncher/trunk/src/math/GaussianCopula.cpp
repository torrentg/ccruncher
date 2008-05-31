
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
// GaussianCopula.cpp - GaussianCopula code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/05/20 - Gerard Torrent [gerard@mail.generacio.com]
//   . implemented Arrays class
//
// 2005/07/24 - Gerard Torrent [gerard@mail.generacio.com]
//   . class CopulaNormal renamed to GaussianCopula
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <cmath>
#include <ctime>
#include <cfloat>
#include <cstdlib>
#include "math/Normal.hpp"
#include "math/GaussianCopula.hpp"
#include "math/CholeskyDecomposition.hpp"
#include "utils/Arrays.hpp"

//---------------------------------------------------------------------------

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197196
#endif

//===========================================================================
// init
//===========================================================================
void ccruncher::GaussianCopula::init()
{
  owner = false;
  aux1 = NULL;
  aux2 = NULL;
  sigmas = NULL;
}

//===========================================================================
// finalize
//===========================================================================
void ccruncher::GaussianCopula::finalize()
{
  if (owner)
  {
    Arrays<double>::deallocMatrix(sigmas, n);
    sigmas = NULL;
  }

  if (aux1 != NULL) {
    Arrays<double>::deallocVector(aux1);
    aux1 = NULL;
  }

  if (aux2 != NULL) {
    Arrays<double>::deallocVector(aux2);
    aux2 = NULL;
  }
}

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::GaussianCopula::GaussianCopula(const GaussianCopula &x) throw(Exception) : Copula()
{
  init();
  n = x.n;
  aux1 = Arrays<double>::allocVector(n);
  aux2 = Arrays<double>::allocVector(n);

  // definim sigmas
  owner = false;
  sigmas = x.sigmas;
  next();
}

//===========================================================================
// constructor
// caution: modify mcorrels content
// caution: releases mcorrels memory in destructor
//===========================================================================
ccruncher::GaussianCopula::GaussianCopula(int n_, double **mcorrels) throw(Exception)
{
  try
  {
    init();
    n = n_;
    aux1 = Arrays<double>::allocVector(n);
    aux2 = Arrays<double>::allocVector(n);

    // omplim sigmas
    owner = true;
    sigmas = mcorrels;
    correls2sigmas(mcorrels);
    next();
  }
  catch(Exception &e)
  {
    finalize();
    throw e;
  }
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::GaussianCopula::~GaussianCopula()
{
  finalize();
}

//===========================================================================
// size. returns number of components
//===========================================================================
int ccruncher::GaussianCopula::size()
{
  return n;
}

//===========================================================================
// transform initial correlation to normal correlation
// observation: 2*sin(1*M_PI/6) = 1 => diagonal values = 1 always
//===========================================================================
double ccruncher::GaussianCopula::transform(double val)
{
  return 2.0*sin(val*M_PI/6.0);
}

//===========================================================================
// given a initial correlation matrix return taus-k matrix needed by copula normal
// caution: replace correls matrix content by sigma matrix content
//===========================================================================
void ccruncher::GaussianCopula::correls2sigmas(double **correlations) throw(Exception)
{
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      sigmas[i][j] = transform(correlations[i][j]);
    }
  }

  bool def_pos = CholeskyDecomposition::choldc(sigmas, aux1, n);

  if (!def_pos)
  {
    throw Exception("correlation matrix not definite positive");
  }
  else
  {
    for(int i=0;i<n;i++)
    {
      sigmas[i][i] = aux1[i];
    }

    for(int i=n-1;i>=0;i--)
    {
      for(int j=0;j<i;j++)
      {
        sigmas[j][i] = 0.0;
      }
    }
  }
}

//===========================================================================
// randNm
// fill aux1 with rand N(0,1) (independents)
// fill aux2 with rand N(0,1) (correlateds per sigmas)
//===========================================================================
void ccruncher::GaussianCopula::randNm()
{
  for(int i=0;i<n;i++)
  {
    aux1[i] = mtrand.randNorm();
  }

  Arrays<double>::prodMatrixVector(sigmas, aux1, n, n, aux2);
}

//===========================================================================
// Compute a copula. Put in aux1 a random vector where each marginal follows
// a U[0,1] related by a normal copula
//===========================================================================
void ccruncher::GaussianCopula::next()
{
  // generate a random vector following N(0,sigmas) into aux2
  randNm();

  // puting in aux1 the copula
  for(int i=0;i<n;i++)
  {
    aux1[i] = Normal::cdf(aux2[i]);
    //aux1[i] = Normal::cdf(aux2[i], 0.0, sigmas[i][i]); //sigmas[i][i]=1.0
  }
}

//===========================================================================
// Return components i-th from current copula
//===========================================================================
double ccruncher::GaussianCopula::get(int i)
{
  if (i < 0 || i >= n)
  {
    return NAN;
  }
  else
  {
    return aux1[i];
  }
}

//===========================================================================
// Set new seed in the number generator
// @param x seed to be set
//===========================================================================
void ccruncher::GaussianCopula::setSeed(long k)
{
  mtrand.seed((const unsigned long) k);
}
