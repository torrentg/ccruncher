
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

#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include "math/BlockGaussianCopula.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

//===========================================================================
// reset
//===========================================================================
void ccruncher::BlockGaussianCopula::reset()
{
  n = 0;
  aux1 = NULL;
  aux2 = NULL;
  chol = NULL;
  rng = NULL;
  owner = true;
}

//===========================================================================
// finalize
//===========================================================================
void ccruncher::BlockGaussianCopula::finalize()
{
  if (rng != NULL) {
    gsl_rng_free(rng);
    rng = NULL;
  }
  
  if (aux1 != NULL) {
    Arrays<double>::deallocVector(aux1);
    aux1 = NULL;
  }

  if (aux2 != NULL) {
    Arrays<double>::deallocVector(aux2);
    aux2 = NULL;
  }
  
  if (chol != NULL && owner == true) {
    delete chol;
    chol = NULL;
  }
}

//===========================================================================
// copy constructor
// the owner flag is set to avoid memory allocation and memory fragmentation 
// when exists multiple copulas with the same static data (chol)
//===========================================================================
ccruncher::BlockGaussianCopula::BlockGaussianCopula(const BlockGaussianCopula &x, bool alloc) throw(Exception) : 
  Copula(), rng(NULL), chol(NULL)
{
  reset();
  n = x.n;
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  owner = alloc;
  if (alloc == true)
  {
    chol = new BlockMatrixChol(*x.chol);
  }
  else
  {
    chol = x.chol;
  }
  aux1 = Arrays<double>::allocVector(n);
  aux2 = Arrays<double>::allocVector(n);
  next();
}

//===========================================================================
// constructor
// C: sectorial correlation/autocorrelation matrix
// n: number of elements at each sector
// m: number of sectors
// type: type of correlation (0=basic, 1=spearman, 2=kendall)
// coerce: if correlation matrix is not definite-positive, replace by
//         the closes definite-positive matrix using the eigenvalue method
//===========================================================================
ccruncher::BlockGaussianCopula::BlockGaussianCopula(double **C_, int *n_, int m, int type, bool coerce) throw(Exception) :
  Copula(), rng(NULL), chol(NULL)
{
  assert(C_ != NULL);
  assert(n_ != NULL);
  assert(m > 0);
  double **correls = NULL;
  
  reset();
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  owner = true;

  try
  {
    // copying correlation coeficients
    correls = Arrays<double>::allocMatrix(m, m, C_);

    // transforming correls coeficients
    for(int i=0;i<m;i++)
    {
      for(int j=0;j<m;j++)
      {
        if (type == 0) { // basic
          correls[i][j] = correls[i][j];
        }
        else if (type == 1) { // spearman
          correls[i][j] = 2.0*sin(correls[i][j]*M_PI/6.0);
        }
        else if (type == 2) { // kendall
          correls[i][j] = sin(correls[i][j]*M_PI/2.0);
        }
        else {
          throw Exception("unrecognized correlation type");
        }
      }
    }

    // computing cholesky factorization
    chol = new BlockMatrixChol(correls, n_, m, coerce);
    Arrays<double>::deallocMatrix(correls, m);

    // allocating mem for temp arrays
    n = chol->getDim();
    aux1 = Arrays<double>::allocVector(n);
    aux2 = Arrays<double>::allocVector(n);

    // preparing to receive the first get()
    next();
  }
  catch(Exception &e)
  {
    Arrays<double>::deallocMatrix(correls, m);
    finalize();
    throw;
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
// clone
//===========================================================================
Copula* ccruncher::BlockGaussianCopula::clone(bool alloc)
{
  return new BlockGaussianCopula(*this, alloc);
}

//===========================================================================
// size. returns number of components
//===========================================================================
int ccruncher::BlockGaussianCopula::size() const
{
  return n;
}

//===========================================================================
// Compute a copula. Put in aux1 a random vector where each marginal follows
// a U[0,1] related by a normal copula
//===========================================================================
void ccruncher::BlockGaussianCopula::next()
{
  // fill aux1 with rand N(0,1) (independents)
  for(int i=0; i<n; i++)
  {
    aux1[i] = gsl_ran_ugaussian(rng);
  }

  // fill aux2 with rand N(0,A) (correlateds)
  chol->mult(aux1, aux2);
  
  // puting in aux1 the copula
  for(int i=0; i<n; i++)
  {
    aux1[i] = gsl_cdf_ugaussian_P(aux2[i]);
  }
}

//===========================================================================
// Return components i-th from current copula
//===========================================================================
double ccruncher::BlockGaussianCopula::get(int i) const
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
// Returns simulated values
//===========================================================================
const double* ccruncher::BlockGaussianCopula::get() const
{
  return aux1;
}

//===========================================================================
// Set new seed in the number generator
//===========================================================================
void ccruncher::BlockGaussianCopula::setSeed(long seed)
{
  gsl_rng_set(rng, (unsigned long) seed);
}

//===========================================================================
// returns the Random Number Generator
//===========================================================================
gsl_rng* ccruncher::BlockGaussianCopula::getRng()
{
  return rng;
}

//===========================================================================
// returns the cholesky matrix
//===========================================================================
const BlockMatrixChol* ccruncher::BlockGaussianCopula::getCholesky() const
{
  return chol;
}
