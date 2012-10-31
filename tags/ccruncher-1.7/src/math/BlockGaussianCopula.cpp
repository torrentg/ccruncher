
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

//---------------------------------------------------------------------------

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197196
#endif

#define LUTSIZE 10001

//===========================================================================
// reset
//===========================================================================
void ccruncher::BlockGaussianCopula::reset()
{
  n = 0;
  m = 0;
  aux1 = NULL;
  aux2 = NULL;
  chol = NULL;
  lut = NULL;
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
  
  if (lut != NULL && owner == true) {
    delete lut;
    lut = NULL;
  }
  
  if (chol != NULL && owner == true) {
    delete chol;
    chol = NULL;
  }
}

//===========================================================================
// copy constructor
// the owner flag is set to avoid memory allocation and memory fragmentation 
// when exists multiple copulas with the same static data (chol, lut)
//===========================================================================
ccruncher::BlockGaussianCopula::BlockGaussianCopula(const BlockGaussianCopula &x, bool alloc) throw(Exception) : 
  Copula(), rng(NULL), chol(NULL), lut(NULL)
{
  reset();
  n = x.n;
  m = x.m;
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  owner = alloc;
  if (alloc == true)
  {
    chol = new BlockMatrixChol(*x.chol);
    lut = (x.lut==NULL?NULL:new LookupTable(*x.lut));
  }
  else
  {
    chol = x.chol;
    lut = x.lut;
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
//===========================================================================
ccruncher::BlockGaussianCopula::BlockGaussianCopula(double **C_, int *n_, int m_) throw(Exception) : 
  Copula(), rng(NULL), chol(NULL), lut(NULL)
{
  assert(C_ != NULL);
  assert(n_ != NULL);
  assert(m_ > 0);
  double **correls = NULL;
  
  reset();
  m = m_;
  rng = gsl_rng_alloc(gsl_rng_mt19937);
  owner = true;

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
    Arrays<double>::deallocMatrix(correls, m_);

    // allocating mem for temp arrays
    n = chol->getDim();
    aux1 = Arrays<double>::allocVector(n);
    aux2 = Arrays<double>::allocVector(n);

    // creating lookuptable to speedup gaussian CDF
    //initLUT();

    // preparing to receive the first get()
    next();
  }
  catch(Exception &e)
  {
    Arrays<double>::deallocMatrix(correls, m_);
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
// clone
//===========================================================================
Copula* ccruncher::BlockGaussianCopula::clone(bool alloc)
{
  return new BlockGaussianCopula(*this, alloc);
}

//===========================================================================
// initLUT
// t-student and gaussian CDF are symmetric respect to 0.0
// lut contains CDF gaussian evaluated between 0.0 and x where cdf(x)>0.9999
//===========================================================================
void ccruncher::BlockGaussianCopula::initLUT() throw(Exception)
{
  double minv = 0.0;
  double maxv = gsl_cdf_ugaussian_Pinv(0.9999);
  vector<double> values;
  double steplength = (maxv-minv)/(double)(LUTSIZE-1);

  for(int i=0; i<LUTSIZE; i++)
  {
    double x = (double)(i)*steplength;
    double val = gsl_cdf_ugaussian_P(x);
    values.push_back(val);
  }
  
  maxv += steplength;
  values.push_back(1.0);

  lut = new LookupTable(minv, maxv, values);
}

//===========================================================================
// size. returns number of components
//===========================================================================
int ccruncher::BlockGaussianCopula::size() const
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
// fill aux2 with rand N(0,1) (correlateds)
//===========================================================================
void ccruncher::BlockGaussianCopula::randNm()
{
  for(int i=0;i<n;i++)
  {
    aux1[i] = gsl_ran_ugaussian(rng);
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
    aux1[i] = gsl_cdf_ugaussian_P(aux2[i]);
    // gaussian lut covers only positive values
    // negative values are computed as 1-lut(abs(x))
    //double x = factor*aux2[i];
    //double y = lut->evalue(abs(x));
    //aux1[i] = (x<0.0?1.0-y:y);
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
// returns the cholesky matrix condition number
//===========================================================================
double ccruncher::BlockGaussianCopula::getConditionNumber()
{
  return chol->getConditionNumber();
}

//===========================================================================
// returns the Random Number Generator
//===========================================================================
gsl_rng* ccruncher::BlockGaussianCopula::getRng()
{
  return rng;
}
