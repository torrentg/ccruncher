
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
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_multimin.h>
#include "math/CopulaCalibration.hpp"
#include "math/BlockMatrixChol.hpp"
#include "math/BlockMatrixCholInv.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

using namespace std;

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
ccruncher::CopulaCalibration::CopulaCalibration() : M(NULL)
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
    Arrays<double>::deallocMatrix(params.h, params.t);
    params.h = NULL;
  }

  if (params.p != NULL) {
    Arrays<double>::deallocVector(params.p);
    params.p = NULL;
  }

  if (params.n != NULL) {
    Arrays<int>::deallocVector(params.n);
    params.n = NULL;
  }

  if (M != NULL && params.k > 0) {
    Arrays<double>::deallocMatrix(M, params.k);
    M = NULL;
  }

  if (params.M != NULL && params.k > 0) {
    Arrays<double>::deallocMatrix(params.M, params.k);
    params.M = NULL;
  }

  if (params.x != NULL) {
    Arrays<double>::deallocVector(params.x);
    params.x = NULL;
  }

  if (params.y != NULL) {
    Arrays<double>::deallocVector(params.y);
    params.y = NULL;
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

  //TODO: check that 0 <= h[i][j] <= 1
  //TODO: check that 0 <= p[i] <= 1

  params.dim = 0;
  for(int i=0; i<k; i++) params.dim += n[i];
  //TODO: check if exist sectors with 0 elements?
  if (params.dim < 2) throw Exception("copula of dimension less than 2");

  params.k = k;
  params.t = t;
  params.n = Arrays<int>::allocVector(k, n);
  params.p = Arrays<double>::allocVector(k, p);
  params.h = Arrays<double>::allocMatrix(t, k, h);
  params.M = Arrays<double>::allocMatrix(k, k, NAN);
  params.x = Arrays<double>::allocVector(params.dim);
  params.y = Arrays<double>::allocVector(params.dim);
  M = Arrays<double>::allocMatrix(k, k, NAN);
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
double** ccruncher::CopulaCalibration::getCorrelations() const
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
  int num_params = (params.k*(params.k+1))/2 + 1;

  // starting point
  x = gsl_vector_alloc(num_params);
  serialize(params.k, sigma0, ndf0, x);

  // set initial step sizes to 1
  ss = gsl_vector_alloc(num_params);
  gsl_vector_set_all(ss, 0.05);

  // initialize method and iterate
  minex_func.n = num_params;
  minex_func.f = CopulaCalibration::f;
  minex_func.params = (void *)(&params);

  //TODO: set gsl_multimin_fminimizer_nmsimplex2 instead of gsl_multimin_fminimizer_nmsimplex
  minimizer = gsl_multimin_fminimizer_alloc(gsl_multimin_fminimizer_nmsimplex, num_params);
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

     printf ("%5d f() = %7.3f size = %.3f\n", (int) iter, minimizer->fval, size);
  }
  while (status == GSL_CONTINUE && iter < 1000);

  gsl_vector_free(x);
  gsl_vector_free(ss);
  gsl_multimin_fminimizer_free(minimizer);
}

//===========================================================================
// serialize params
//===========================================================================
void ccruncher::CopulaCalibration::serialize(int k, const double **matrix, double value, gsl_vector *ret)
{
  int pos = 0;

  gsl_vector_set(ret, pos, value);
  pos++;

  for(int i=0; i<k; i++)
  {
    for(int j=i; j<k; j++)
    {
      gsl_vector_set(ret, pos, matrix[i][j]);
      pos++;
    }
  }
}

//===========================================================================
// deserialize params
//===========================================================================
void ccruncher::CopulaCalibration::deserialize(int k, const gsl_vector *ret, double **matrix, double *value)
{
  int pos = 0;

  *value = gsl_vector_get(ret, pos);
  pos++;

  for(int i=0; i<k; i++)
  {
    for(int j=i; j<k; j++)
    {
      matrix[i][j] = gsl_vector_get(ret, pos);
      matrix[j][i] = gsl_vector_get(ret, pos);
      pos++;
    }
  }
}

//=============================================================
// getObservation
//=============================================================
void ccruncher::CopulaCalibration::getObservation(int row, const fparams *p, double *ret)
{
  for(int i=0,s=0; i<p->k; i++)
  {
    int num = (int)(p->h[row][i]*p->n[i]);

    for(int j=0; j<num; j++)
    {
      ret[s] = (p->p[i])/2.0;
      s++;
    }
    for(int j=num; j<p->n[i]; j++)
    {
      ret[s] = p->p[i] + (1.0 - p->p[i])/2.0;
      s++;
    }
  }
}

//=============================================================
// function to minimize
// v = values to be optimized
// params = pointer to a fparams struct
//=============================================================
double ccruncher::CopulaCalibration::f(const gsl_vector *v, void *params_)
{
  int dim = 0;
  double ret = 0.0;
  double nu;
  fparams *p = (fparams*) params_;

  for(int i=0; i<p->k; i++) dim += p->n[i];
  deserialize(p->k, v, p->M, &nu);

  BlockMatrixChol *L = new BlockMatrixChol(p->M, p->n, p->k);
  BlockMatrixCholInv *I = L->getInverse();

  // sign inverted because gsl minimize and we try to maximize
  for(int i=0; i<p->t; i++)
  {
    ret -= 1.0/fabs(L->getDeterminant());
    ret -= gsl_sf_lngamma((nu+dim)/2.0);
    ret -= (dim-1.0)*gsl_sf_lngamma(nu/2.0);
    ret += dim*gsl_sf_lngamma((nu+1.0)/2.0);

    getObservation(i, p, p->x);
    I->mult(p->x, p->y);

    double aux1=0.0, aux2=0.0;
    for(int j=0; j<dim; j++)
    {
        aux1 += p->y[j]*p->y[j];
        aux2 += log(1.0+p->x[j]*p->x[j]/2.0);
    }

    ret += (nu+dim)/2.0 * log(1.0+aux1);
    ret -= (nu+1.0)/2.0 * aux2;
  }

  delete L;
  delete I;

  return ret;
}
