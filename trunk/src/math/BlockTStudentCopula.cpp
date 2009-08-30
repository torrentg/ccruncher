
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

#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <gsl/gsl_cdf.h>
#include "math/BlockTStudentCopula.hpp"
#include "utils/Arrays.hpp"

//---------------------------------------------------------------------------

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197196
#endif

//===========================================================================
// init
//===========================================================================
void ccruncher::BlockTStudentCopula::init()
{
  owner = false;
  aux1 = NULL;
  aux2 = NULL;
  chol = NULL;
  correls = NULL;
  ndf = -1.0;
}

//===========================================================================
// finalize
//===========================================================================
void ccruncher::BlockTStudentCopula::finalize()
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
ccruncher::BlockTStudentCopula::BlockTStudentCopula(const BlockTStudentCopula &x) throw(Exception) : Copula()
{
  init();
  n = x.n;
  m = x.m;
  ndf = x.ndf;
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
ccruncher::BlockTStudentCopula::BlockTStudentCopula(double **C_, int *n_, int m_, double ndf_) throw(Exception)
{
  // basic initializations
  init();
  owner = true;
  m = m_;

  if (ndf_ <= 0.0) {
    throw Exception("invalid degrees of freedom (ndf > 0 required)");
  }
  ndf = ndf_;

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
    chol = new BlockMatrixChol(correls, n_, m_);

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
ccruncher::BlockTStudentCopula::~BlockTStudentCopula()
{
  finalize();
}

//===========================================================================
// size. returns number of components
//===========================================================================
int ccruncher::BlockTStudentCopula::size()
{
  return n;
}

//===========================================================================
// transform initial correlation to normal correlation
// observation: 2*sin(1*M_PI/6) = 1 => diagonal values = 1 always
//===========================================================================
double ccruncher::BlockTStudentCopula::transform(double val)
{
  return 2.0*sin(val*M_PI/6.0);
}

//===========================================================================
// randNm
// fill aux1 with rand N(0,1) (independents)
// fill aux2 with rand N(0,1) (correlateds)
//===========================================================================
void ccruncher::BlockTStudentCopula::randNm()
{
  for(int i=0;i<n;i++)
  {
    aux1[i] = random.nextGaussian();
  }

  chol->mult(aux1, aux2);
}

//===========================================================================
// Compute a copula. Put in aux1 a random vector where each marginal follows
// a U[0,1] related by a t-student copula
//===========================================================================
void ccruncher::BlockTStudentCopula::next()
{
  // generate a random vector following N(0,sigmas) into aux2
  randNm();
  // create the factor (involves the generation of a chi-square)
  double chisq = random.nextChisq(ndf);
  if (chisq < 1e-14) chisq = 1e-14; //avoid division by 0
  double factor = sqrt(ndf/chisq);

  // puting in aux1 the copula
  for(int i=0;i<n;i++)
  {
    aux1[i] = gsl_cdf_tdist_P(factor*aux2[i], ndf);
  }
}

//===========================================================================
// Return components i-th from current copula
//===========================================================================
double ccruncher::BlockTStudentCopula::get(int i)
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
//===========================================================================
void ccruncher::BlockTStudentCopula::setSeed(long k)
{
  random.setSeed(k);
}

//===========================================================================
// returns the cholesky matrix condition number
//===========================================================================
double ccruncher::BlockTStudentCopula::getConditionNumber()
{
  return chol->getConditionNumber();
}

