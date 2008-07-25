
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
// BlockGaussianCopula.cpp - BlockGaussianCopula code - $Rev$
// --------------------------------------------------------------------------
//
// 2005/07/24 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <cmath>
#include <cfloat>
#include <cstdlib>
#include "math/Normal.hpp"
#include "math/BlockGaussianCopula.hpp"
#include "utils/Arrays.hpp"

//---------------------------------------------------------------------------

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197196
#endif

//===========================================================================
// init
//===========================================================================
void ccruncher::BlockGaussianCopula::init()
{
  owner = false;
  aux1 = NULL;
  aux2 = NULL;
  chol = NULL;
  correls = NULL;
}

//===========================================================================
// finalize
//===========================================================================
void ccruncher::BlockGaussianCopula::finalize()
{
  if (owner)
  {
    if (chol != NULL) {
      delete chol;
      chol = NULL;
    }
    if (correls != NULL) {
      Arrays<double>::deallocMatrix(correls, m);
      correls = NULL;
    }
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
ccruncher::BlockGaussianCopula::BlockGaussianCopula(const BlockGaussianCopula &x) throw(Exception) : Copula()
{
  init();
  n = x.n;
  m = x.m;
  aux1 = Arrays<double>::allocVector(n);
  aux2 = Arrays<double>::allocVector(n);

  // definim sigmas
  owner = false;
  chol = x.chol;
  correls = x.correls;
  next();
}

//===========================================================================
// constructor
// C: sectorial correlation/autocorrelation matrix
// n: number of elements at each sector
// m: number of sectors
//===========================================================================
ccruncher::BlockGaussianCopula::BlockGaussianCopula(double **C_, int *n_, int m_) throw(Exception)
{
  // basic initializations
  init();
  owner = true;
  m = m_;

  try
  {
    // copying correlation coeficients
    correls = Arrays<double>::allocMatrix(m_, m_, C_);

    // transforming correls coeficients
    for(int i=0;i<m;i++)
    {
      for(int j=0;j<m;j++)
      {
        correls[i][j] = transform(correls[i][j]);
      }
    }

    // computing cholesky factorization
    chol = new BlockMatrixChol(C_, n_, m_);

    // allocating mem for temp arrays
    n = chol->getDim();
    aux1 = Arrays<double>::allocVector(n);
    aux2 = Arrays<double>::allocVector(n);

    // preparing to receive the first get()
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
ccruncher::BlockGaussianCopula::~BlockGaussianCopula()
{
  finalize();
}

//===========================================================================
// size. returns number of components
//===========================================================================
int ccruncher::BlockGaussianCopula::size()
{
  return n;
}

//===========================================================================
// transform initial correlation to normal correlation
// observation: 2*sin(1*M_PI/6) = 1 => diagonal values = 1 always
//===========================================================================
double ccruncher::BlockGaussianCopula::transform(double val)
{
  return 2.0*sin(val*M_PI/6.0);
}

//===========================================================================
// randNm
// fill aux1 with rand N(0,1) (independents)
// fill aux2 with rand N(0,1) (correlateds per sigmas)
//===========================================================================
void ccruncher::BlockGaussianCopula::randNm()
{
  for(int i=0;i<n;i++)
  {
    aux1[i] = mtrand.randNorm();
  }

  chol->mult(aux1, aux2);
}

//===========================================================================
// Compute a copula. Put in aux1 a random vector where each marginal follows
// a U[0,1] related by a normal copula
//===========================================================================
void ccruncher::BlockGaussianCopula::next()
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
double ccruncher::BlockGaussianCopula::get(int i)
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
void ccruncher::BlockGaussianCopula::setSeed(long k)
{
  mtrand.seed((const unsigned long) k);
}
