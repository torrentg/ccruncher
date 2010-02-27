
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

#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <gsl/gsl_cdf.h>
#include "math/BlockTStudentCopula.hpp"
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
void ccruncher::BlockTStudentCopula::reset()
{
  n = 0;
  m = 0;
  ndf = -1.0;
  aux1 = NULL;
  aux2 = NULL;
  lut = LookupTable();
}

//===========================================================================
// finalize
//===========================================================================
void ccruncher::BlockTStudentCopula::finalize()
{
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
  reset();
  n = x.n;
  m = x.m;
  ndf = x.ndf;
  chol = BlockMatrixChol(x.chol);
  aux1 = Arrays<double>::allocVector(n);
  aux2 = Arrays<double>::allocVector(n);
  lut = x.lut;
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
  assert(C_ != NULL);
  assert(n_ != NULL);
  assert(m_ > 0);
  double **correls = NULL;
  reset();
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
    chol = BlockMatrixChol(correls, n_, m_);
    Arrays<double>::deallocMatrix(correls, m_);

    // allocating mem for temp arrays
    n = chol.getDim();
    aux1 = Arrays<double>::allocVector(n);
    aux2 = Arrays<double>::allocVector(n);

    // creating lookuptable to speedup t-student CDF
    initLUT();

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
ccruncher::BlockTStudentCopula::~BlockTStudentCopula()
{
  finalize();
}

//===========================================================================
// initLUT
// t-student and gaussian CDF are symmetric respect to 0.0
// lut contains CDF t-student evaluated between 0.0 and x where cdf(x)>0.9999
//===========================================================================
void ccruncher::BlockTStudentCopula::initLUT() throw(Exception)
{
  double minv = 0.0;
  double maxv = gsl_cdf_tdist_Pinv(0.9999, ndf);
  vector<double> values;
  double steplength = (maxv-minv)/(double)(LUTSIZE-1);

  for(int i=0; i<LUTSIZE; i++)
  {
    double x = (double)(i)*steplength;
    double val = gsl_cdf_tdist_P(x, ndf);
    values.push_back(val);
    //double gsl_ran_tdist_pdf (double x, double nu)
  }
  
  maxv += steplength;
  values.push_back(1.0);

  lut = LookupTable(minv, maxv, values);
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

  chol.mult(aux1, aux2);
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
    //aux1[i] = gsl_cdf_tdist_P(factor*aux2[i], ndf);
    // t-student lut covers only positive values
    // negative values are computed as 1-lut(abs(x))
    double x = factor*aux2[i];
    double y = lut.evalue(abs(x));
    aux1[i] = (x<0.0?1.0-y:y);
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
  return chol.getConditionNumber();
}

