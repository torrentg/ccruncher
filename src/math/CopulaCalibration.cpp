
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

#include <gsl/gsl_multimin.h>
#include "math/CopulaCalibration.hpp"
#include "math/BlockMatrixChol.hpp"
#include "math/BlockMatrixCholInv.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

//===========================================================================
// Description:
// ------------
// implements the t-Student parameter estimation using the MLE 
// described in paper 'Simulation of High-Dimensional t-Student Copula
// Copulas with a Given Block Matrix Correlation Matrix' by
// Gerard Torrent-Gironella and Josep Fortiana
//
//===========================================================================

//===========================================================================
// constructor
//===========================================================================
ccruncher::CopulaCalibration::CopulaCalibration() : 
    params.n(NULL), params.p(NULL), params.h(NULL), M(NULL)
{
  reset();
}

//===========================================================================
// Destructor
//===========================================================================
ccruncher::CopulaCalibration::~CopulaCalibration()
{
  reset();
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::CopulaCalibration::reset()
{
  if (params.h != NULL && params.t > 0) {
    Arrays::deallocMatrix(params.h, params.t);
    params.h = NULL;
  }

  if (params.p != NULL) {
    Arrays::deallocVector(params.p);
    params.p = NULL;
  }

  if (params.n != NULL) {
    Arrays::deallocVector(params.n);
    params.n = NULL;
  }

  if (M != NULL && params.k > 0) {
    Arrays::deallocMatrix(M, params.k);
    M = NULL;
  }

  params.k = 0;
  params.t = 0;
  ndf = NAN;
}

//===========================================================================
// set function params
//===========================================================================
void ccruncher::CopulaCalibration::setParams(int k, int *n, double **h, int t, double *p) throw(Exception)
{
  assert(n != NULL);
  assert(h != NULL);
  assert(p != NULL);

  reset();

  if (k <= 0) throw Exception("invalid number of sectors");
  if (t < 1) throw Exception("invalid number of observations");

  params.k = k;
  params.t = t;
  params.n = Arrays::allocVector(k, n);
  params.p = Arrays::AllocVector(k, p);
  params.h = Arrays::allocmatrix(t, k, h);
  M = Arrays::allocmatrix(k, k, NAN);
  ndf = NAN;
}

//===========================================================================
// return estimated ndf
//===========================================================================
double ccruncher::CopulaCalibration::getNdf() const
{
  return ndf;
}

//===========================================================================
// return estimated correlations
//===========================================================================
const double** ccruncher::CopulaCalibration::getCorrelations() const
{
  return M;
}

//===========================================================================
// calibrate copula using MLE
//===========================================================================
void ccruncher::CopulaCalibration::run(const double **sigma0, double ndf0) throw(Exception)
{
  //TODO: comprovar tamanys de parametres

  gsl_multimin_fminimizer *minimizer=NULL;
  gsl_vector *ss=NULL, *x=NULL;
  gsl_multimin_function minex_func;
  size_t iter = 0;
  int status;
  double size;
  int dim = (params.k*(params.k+1))/2 + 1;

  // starting point
  x = gsl_vector_alloc(dim);
  serialize(sigma0, ndf0, x);

  // set initial step sizes to 1
  ss = gsl_vector_alloc(dim);
  gsl_vector_set_all(ss, 0.05);

  // initialize method and iterate
  minex_func.n = dim;
  minex_func.f = Stitcher2Calibration::f;
  minex_func.params = (void *)(&params);

  minimizer = gsl_multimin_fminimizer_alloc(gsl_multimin_fminimizer_nmsimplex2, dim);
  gsl_multimin_fminimizer_set(minimizer, &minex_func, x, ss);

  do
  {
     iter++;
     status = gsl_multimin_fminimizer_iterate(minimizer);
   
     if (status) break;

     size = gsl_multimin_fminimizer_size(minimizer);
     status = gsl_multimin_test_size(size, 1e-2);

     if (status == GSL_SUCCESS)
     {
       printf ("converged to minimum at\n");
     }

     printf ("%5d f() = %7.3f size = %.3f\n", 
           iter, minimizer->fval, size);
  }
  while (status == GSL_CONTINUE && iter < 100);

  gsl_vector_free(x);
  gsl_vector_free(ss);
  gsl_multimin_fminimizer_free(minimizer);
}

//===========================================================================
// serialize params
//===========================================================================
void ccruncher::CopulaCalibration::serialize(const double **matrix, double value, gsl_vector *ret)
{
  int pos = 0;

  gsl_vector_set(ret, pos, value);
  pos++;

  for(int i=0; i<params.k; i++)
  {
    for(int j=i; j<params.k; j++)
    {
      gsl_vector_set(ret, pos, matrix[i][j]);
      pos++;
    }
  }
}

//===========================================================================
// deserialize params
//===========================================================================
void ccruncher::CopulaCalibration::deserialize(const gsl_vector *ret, double **matrix, double *value)
{
  int pos = 0;

  *value = gsl_vector_get(ret, pos);
  pos++;

  for(int i=0; i<params.k; i++)
  {
    for(int j=i; j<params.k; j++)
    {
      matrix[i][j] = gsl_vector_get(ret, pos);
      matrix[j][i] = gsl_vector_get(ret, pos);
      pos++;
    }
  }
}

//=============================================================
// function to minimize
// v = values to be optimized
// params = pointer to a fparams struct
//=============================================================
double ccruncher::CopulaCalibration::f(const gsl_vector *v, void *p_)
{
  fparams *p = (fparams*) params_;

  double val;
  double **matrix = Arrays<double>::allocMatrix(p->k, p->k, 0.0);
  deserialize(v, matrix, &val);

  BlockMatrixChol *L = new BlockMatrixChol(matrix, p->n, p->k);
  BlockMatrixCholInv *I = L->getInverse();

  //TODO: continuar

  delete L;
  delete I;
}


