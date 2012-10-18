
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
#include <cstring>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include "math/GMFCopula.hpp"
#include <cassert>

//===========================================================================
// finalize
//===========================================================================
void ccruncher::GMFCopula::finalize()
{
  if (rng != NULL) gsl_rng_free(rng);
  if (aux != NULL) gsl_vector_free(aux);

  if (owner && n != NULL) delete[] n;
  if (owner && w != NULL) delete[] w;
  if (owner && chol != NULL) gsl_matrix_free(chol);
}

//===========================================================================
// copy constructor
// the owner flag is set to avoid memory allocation and memory fragmentation
// when exists multiple copulas with the same static data (chol)
//===========================================================================
ccruncher::GMFCopula::GMFCopula(const GMFCopula &x, bool alloc) throw(Exception) :
    Copula(), n(NULL), w(NULL), chol(NULL), owner(alloc), k(x.k), rng(NULL), aux(NULL), values(x.values.size(),NAN)
{
  if (owner)
  {
    n = new unsigned int[k];
    memcpy(n, x.n, sizeof(unsigned int)*k);
    w = new double[k];
    memcpy(w, x.w, sizeof(double)*k);
    chol = gsl_matrix_alloc(k, k);
    gsl_matrix_memcpy(chol, x.chol);
  }
  else
  {
    n = x.n;
    w = x.w;
    chol = x.chol;
  }

  rng = gsl_rng_alloc(gsl_rng_mt19937);
  aux = gsl_vector_alloc(k);
}

//===========================================================================
// constructor
// M: factors correlation matrix (diag = factor loadings)
// n: number of elements at each sector
// k: number of sectors
//===========================================================================
ccruncher::GMFCopula::GMFCopula(const vector<vector<double> > &M, const vector<unsigned int> &dims) throw(Exception) :
    Copula(), n(NULL), w(NULL), chol(NULL), owner(true), k(0), rng(NULL), aux(NULL), values(0)
{
  assert(dims.size() > 0);
  assert(M.size() == dims.size());

  try
  {
    unsigned int dim = 0;
    for(unsigned int i=0; i<dims.size(); i++) dim += dims[i];
    if (dim == 0) throw Exception("copula dimension equals 0");

    k = dims.size();
    n = new unsigned int[k];
    memcpy(n, &(dims[0]), sizeof(unsigned int)*k);
    rng = gsl_rng_alloc(gsl_rng_mt19937);
    w = new double[k];
    chol = gsl_matrix_alloc(k, k);
    aux = gsl_vector_alloc(k);

    for(unsigned int i=0; i<k; i++)
    {
      if (M[i][i] < 0.0 || 1.0 < M[i][i]) {
        throw Exception("factor loading out of range [0,1]");
      }
      else {
        w[i] = M[i][i];
        gsl_matrix_set(chol, i, i, 1.0);
      }

      for(unsigned int j=i+1; j<k; j++)
      {
        if (M[i][j] < -1.0 || 1.0 < M[i][j]) {
          throw Exception("factor correlation out of range [-1,+1]");
        }
        if (fabs(M[i][j]-M[j][i]) > 1e-10) {
          throw Exception("non-symmetric correlation matrix");
        }
        gsl_matrix_set(chol, i, j, M[i][j]);
        gsl_matrix_set(chol, j, i, M[i][j]);
      }
    }

    gsl_error_handler_t *eh = gsl_set_error_handler_off();
    int rc = gsl_linalg_cholesky_decomp(chol);
    gsl_set_error_handler(eh);
    if (rc != GSL_SUCCESS) {
      throw Exception("non definite-positive correlation matrix");
    }

    // performance tip: chol contains the w·chol (to avoid multiplications)
    // and w contaings sqrt(1-w^2) (to avoid sqrt and multiplications)
    for(unsigned int i=0; i<k; i++)
    {
      for(unsigned int j=0; j<k; j++)
      {
        double val = gsl_matrix_get(chol, i, j) * w[i];
        gsl_matrix_set(chol, i, j, val);
      }
      w[i] = sqrt(1.0-w[i]*w[i]);
    }

    // preparing to receive the first get()
    values.assign(dim, NAN);
  }
  catch(Exception &)
  {
    finalize();
    throw;
  }
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::GMFCopula::~GMFCopula()
{
  finalize();
}

//===========================================================================
// clone
//===========================================================================
Copula* ccruncher::GMFCopula::clone(bool alloc)
{
  return new GMFCopula(*this, alloc);
}

//===========================================================================
// size. returns number of components
//===========================================================================
int ccruncher::GMFCopula::size() const
{
  return values.size();
}

//===========================================================================
// simulate a multivariate normal and let result in values
//===========================================================================
void ccruncher::GMFCopula::rmvnorm()
{
  assert(k > 0);
  assert(n != NULL);
  assert(w != NULL);
  assert(chol != NULL);
  assert(aux != NULL);
  assert(!values.empty());
  assert(rng != NULL);

  // simulate w·N(0,R)
  for(unsigned int i=0; i<k; i++) {
    gsl_vector_set(aux, i, gsl_ran_ugaussian(rng));
  }
  gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, chol, aux);

  // copula simulation
  for(unsigned int pos=0, i=0; i<k; i++)
  {
    for(unsigned int j=n[i]; j>0; j--)
    {
      // simulate pos-th component of a multivariate Normal
      values[pos] = gsl_vector_get(aux, i) + w[i]*gsl_ran_ugaussian(rng);
      pos++;
    }
  }
}

//===========================================================================
// copula simulation. Put in values a random vector where each marginal
// follows a U[0,1] related by a normal copula
//===========================================================================
void ccruncher::GMFCopula::next()
{
  // simulate a multivariate gaussian
  rmvnorm();

  // inverse sampling method
  for(unsigned int pos=0, i=0; i<k; i++)
  {
    for(unsigned int j=n[i]; j>0; j--)
    {
      values[pos] = gsl_cdf_ugaussian_P(values[pos]);
      pos++;
    }
  }
}

//===========================================================================
// Return components i-th from current copula
//===========================================================================
double ccruncher::GMFCopula::get(int i) const
{
  if (i < 0 || (int)values.size() <= i) return NAN;
  else return values[i];
}

//===========================================================================
// Returns simulated values
//===========================================================================
const double* ccruncher::GMFCopula::get() const
{
  return &(values[0]);
}

//===========================================================================
// Set new seed in the number generator
//===========================================================================
void ccruncher::GMFCopula::setSeed(long seed)
{
  gsl_rng_set(rng, (unsigned long) seed);
}

//===========================================================================
// returns the Random Number Generator
//===========================================================================
gsl_rng* ccruncher::GMFCopula::getRng()
{
  return rng;
}
