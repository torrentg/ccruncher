
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

#include <iomanip>
#include <cmath>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_multimin.h>
#include "math/CopulaCalibration.hpp"
#include "math/BlockMatrixChol.hpp"
#include "math/BlockMatrixCholInv.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

#define MIN_NU 2.0
#define MAX_NU 5000.0
#define DELTA 0.01
#define DELTA0 0.05

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

  //TODO: revisar i pulir
  int aux=0;
  for(int i=0; i<k; i++) if (n[i]>0) aux++;

  params.k = aux; //k
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
/*
void ccruncher::CopulaCalibration::run() throw(Exception)
{
  //TODO: comprovar tamanys de parametres

  gsl_multimin_fminimizer *minimizer=NULL;
  gsl_multimin_function minex_func;
  gsl_vector *ss=NULL, *x0=NULL;
  size_t iter = 0;
  int status;
  double size;
  int num_params = 1+ (params.k*(params.k+1))/2;

  // starting point (correls=0, ndf=30)
  // we start from a known valid point
  // if p0 was given by user, then p0 can be non-valid
  x0 = gsl_vector_alloc(num_params);
  gsl_vector_set_all(x0, 0.15);
  gsl_vector_set(x0, 2, 0.05);
  gsl_vector_set(x0, 0, 50.0);

  // set initial step sizes
  ss = gsl_vector_alloc(num_params);
  gsl_vector_set_all(ss, 0.01);
  gsl_vector_set(ss, 0, 0.1);

  // initialize method and iterate
  minex_func.n = num_params;
  minex_func.f = CopulaCalibration::f;
  minex_func.params = (void *)(&params);

  //TODO: set gsl_multimin_fminimizer_nmsimplex2 instead of gsl_multimin_fminimizer_nmsimplex
  minimizer = gsl_multimin_fminimizer_alloc(gsl_multimin_fminimizer_nmsimplex, num_params);
  gsl_multimin_fminimizer_set(minimizer, &minex_func, x0, ss);

  do
  {
     iter++;
     status = gsl_multimin_fminimizer_iterate(minimizer);
   
     if (status) break;

     size = gsl_multimin_fminimizer_size(minimizer);
     status = gsl_multimin_test_size(size, 1e-5);

     if (status == GSL_SUCCESS)
     {
       printf ("converged to minimum at\n");
     }

     printf("%5d f()= %7.3f size= %.3f ", (int) iter, minimizer->fval, size);
     printf("ndf= %7.3f ", gsl_vector_get(minimizer->x, 0));
     for(int i=0; i<(params.k*(params.k+1))/2; i++) printf("%.3f ", gsl_vector_get(minimizer->x, 1+i));
     printf("\n");
  }
  while (status == GSL_CONTINUE && iter < 1000);

  //TODO: check convergence

  deserialize(params.k, minimizer->x, M, &ndf);
  cout << "SOLUTION:" << endl;
  cout << "  ndf = " << ndf << endl;
  cout << "  correlations = " << endl;
  for(int i=0; i<params.k; i++) {
    for(int j=0; j<params.k; j++) {
        if (params.n[i]==0 || params.n[j]==0) M[i][j] = 0.0;
        cout << setprecision(4) << M[i][j] << "\t";
    }
    cout << endl;
  }

  gsl_vector_free(x0);
  gsl_vector_free(ss);
  gsl_multimin_fminimizer_free(minimizer);
}
*/
/*
void ccruncher::CopulaCalibration::run() throw(Exception)
{
  gsl_multimin_fdfminimizer *minimizer=NULL;
  gsl_multimin_function_fdf my_func;
  gsl_vector *x0=NULL;
  size_t iter = 0;
  int status;
  int num_params = 1+ (params.k*(params.k+1))/2;

  my_func.n = num_params;
  my_func.f = CopulaCalibration::f;
  my_func.df = CopulaCalibration::df;
  my_func.fdf = CopulaCalibration::fdf;
  my_func.params = (void *)(&params);

  // starting point (correls=0, ndf=30)
  // we start from a known valid point
  // if p0 was given by user, then p0 can be non-valid
  x0 = gsl_vector_alloc(num_params);
  gsl_vector_set_all(x0, 0.1);
  gsl_vector_set(x0, 0, 30.0);

//for(int i=0; i<=200; i++) {
//double x = -1.0+0.01*i;
//gsl_vector_set(x0, 2, x);
//double y = f(x0, &params);
//cout << x << "\t" << y << endl;
//}

  minimizer = gsl_multimin_fdfminimizer_alloc(gsl_multimin_fdfminimizer_conjugate_fr, num_params);

  gsl_multimin_fdfminimizer_set(minimizer, &my_func, x0, 0.05, 0.05);

  do
    {
      iter++;
      status = gsl_multimin_fdfminimizer_iterate(minimizer);

      if (status)
      {
          printf ("error: %s\n", gsl_strerror(status));
          break;
      }

      status = gsl_multimin_test_gradient(minimizer->gradient, 1e-3);

      if (status == GSL_SUCCESS) printf ("Minimum found at:\n");

      printf("%5d f()= %7.3f ", (int) iter, minimizer->f);
      printf("ndf= %7.3f ", gsl_vector_get(minimizer->x, 0));
      for(int i=0; i<(params.k*(params.k+1))/2; i++) printf("%.3f ", gsl_vector_get(minimizer->x, 1+i));
      printf("\n");
    }
  while (status == GSL_CONTINUE && iter < 100);

  gsl_multimin_fdfminimizer_free(minimizer);
  gsl_vector_free(x0);
}
*/

//===========================================================================
// calibrate copula using MLE
//===========================================================================
void ccruncher::CopulaCalibration::run() throw(Exception)
{
  int num_params = 1+(params.k*(params.k+1))/2;
  gsl_vector *x0 = gsl_vector_alloc(num_params);
  double minval = 1e10;
  int N0=100, N1=10, N2=10, N3=10;

  for(int i0=2; i0<=N0; i0++)
  {
cout << "NDF = " << double(i0) << endl;
    gsl_vector_set(x0, 0, double(i0));

    for(int i1=0; i1<=N1; i1++)
    {
      gsl_vector_set(x0, 1, 0.0+i1/double(N1));

      for(int i2=0; i2<=N2; i2++)
      {
        gsl_vector_set(x0, 2, 0.0+i2/double(N2));

        for(int i3=0; i3<=N3; i3++)
        {
          gsl_vector_set(x0, 3, 0.0+i3/double(N3));

          try
          {
            double y = f(x0, &params);

//cout << "coerced= " << params.coerced << ", y= " << y << "\t x= ";
//for(int i=0; i<(int)x0->size; i++) cout << gsl_vector_get(x0, i) << "\t";
//cout << endl;

            if (!params.coerced && y < minval)
            {
              minval = y;
              cout << "y= " << y << "\t x= ";
              for(int i=0; i<(int)x0->size; i++) cout << gsl_vector_get(x0, i) << "\t";
              cout << endl;
            }
          }
          catch(Exception &e)
          {
            //TODO: veure perque salta excepcio tot i fer coercio
            continue;
          }
        }
      }
    }
  }

  gsl_vector_free(x0);
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

/*
    // all concentrated in the mean
    for(int j=1; j<=num; j++)
    {
      ret[s] = (p->p[i])/2.0;
      s++;
    }
    for(int j=1; j<=(p->n[i]-num); j++)
    {
      ret[s] = p->p[i] + (1.0 - p->p[i])/2.0;
      s++;
    }
*/
/*
    // equidistributed
    for(int j=1; j<=num; j++)
    {
      ret[s] = (p->p[i]) * double(j)/double(num+1);
      s++;
    }
    for(int j=1; j<=(p->n[i]-num); j++)
    {
      ret[s] = p->p[i] + (1.0 - p->p[i]) * double(j)/double(num+1);
      s++;
    }
*/
/*
    // concentrated + symmetric
    // TODO: check round
    if (p->n[i]-2*num > 0)
    {
      for(int j=1; j<=num; j++)
      {
        ret[s] = (p->p[i])/2.0;
        s++;
      }
      for(int j=1; j<=num; j++)
      {
        ret[s] = 1.0 - (p->p[i])/2.0;
        s++;
      }
      for(int j=1; j<=(p->n[i]-2*num); j++)
      {
        ret[s] = p->p[i] + (1.0 - 2.0*p->p[i])/2.0;
        s++;
      }
    }
    else {
        cout << "USE concentrated" << endl;
    }
*/
    // equidistributed + symmetric
    // TODO: check round
    if (p->n[i]-2*num > 0)
    {
      for(int j=1; j<=num; j++)
      {
        ret[s] = (p->p[i]) * double(j)/double(num+1);
        s++;
      }
      for(int j=1; j<=num; j++)
      {
        ret[s] = 1.0 - (p->p[i]) * double(j)/double(num+1);
        s++;
      }
      for(int j=1; j<=(p->n[i]-2*num); j++)
      {
        ret[s] = p->p[i] + (1.0 - 2.0*p->p[i]) * double(j)/double(p->n[i]-2*num+1);
        s++;
      }
    }
    else {
        cout << "USE equidistributed" << endl;
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
  double ret = 0.0;
  double nu0, nu;
  fparams *p = (fparams*) params_;
/*
cout << "f called (";
for(unsigned int i=0; i<v->size; i++) cout << gsl_vector_get(v,i) << ", ";
cout << ")" << endl;
*/
  deserialize(p->k, v, p->M, &nu0);

  if (nu0 < MIN_NU) nu = MIN_NU;
  else if (nu0 > MAX_NU) nu = MAX_NU;
  else nu = nu0;

  //TODO: tractar sectors amb 1 individu

  BlockMatrixChol *L = new BlockMatrixChol(p->M, p->n, p->k);
  BlockMatrixCholInv *I = L->getInverse();
/*
cout << " COERCED = " << L->isCoerced() << ", NDF = " << nu << ", SIGMA = (";
for(int i=0; i<p->k; i++) for(int j=0; j<p->k; j++) cout << L->getCorrelations()[i][j] << ", ";
cout << ")" << endl;
*/
  // sign inverted because gsl minimize and we try to maximize
  for(int i=0; i<p->t; i++)
  {
    // determinant section (we use eigenvalues to avoid accuracy problems)
    const vector<eig> &eigenvalues = L->getEigenvalues();
    double aux0 = 0.0;
    for(unsigned int j=0; j<eigenvalues.size(); j++)
    {
      if (eigenvalues[j].value <= 0.0 || eigenvalues[j].multiplicity <= 0) cout << "VAP NEGATIU: " << eigenvalues[j].value << " (" << eigenvalues[j].multiplicity << ")" << endl;
      assert(eigenvalues[j].multiplicity > 0 && eigenvalues[i].value > 0.0);
      ret += eigenvalues[j].multiplicity * log(eigenvalues[j].value);
    }
    ret += 0.5*aux0;

    ret -= gsl_sf_lngamma((nu+p->dim)/2.0);
    ret -= (p->dim-1.0)*gsl_sf_lngamma(nu/2.0);
    ret += p->dim*gsl_sf_lngamma((nu+1.0)/2.0);

    getObservation(i, p, p->x);
    I->mult(p->x, p->y);

    double aux1=0.0, aux2=0.0;
    for(int j=0; j<p->dim; j++)
    {
        aux1 += (p->y[j])*(p->y[j]);
        // exist an error in 'Copula’s Conditional Dependence Measures for Portfolio Management and Value at Risk' by Dean Fantazzini.
        // the error is that Fantazzini divides by 2 instead of nu
        // see the SAS page: http://support.sas.com/documentation/cdl/en/etsug/63939/HTML/default/viewer.htm#etsug_copula_sect016.htm
        aux2 += log(1.0+(p->x[j])*(p->x[j])/nu);
    }

    ret += (nu+p->dim)/2.0 * log(1.0+aux1/nu);
    ret -= (nu+1.0)/2.0 * aux2;
  }

  if (L != NULL) delete L;
  if (I != NULL) delete I;

  if (nu > MAX_NU) ret += (nu-MAX_NU)*1000;
  else if (nu < MIN_NU) ret += (MIN_NU-nu)*1000;

  if (L->isCoerced()) {
      p->coerced = true;
      ret += 1500;
  }
  else p->coerced = false;
/*
cout << "  y=" << ret << endl;
*/
  return ret;
}

//=============================================================
// function to minimize
// v = values to be optimized
// params = pointer to a fparams struct
//=============================================================
void ccruncher::CopulaCalibration::df(const gsl_vector * x, void * params_, gsl_vector * g)
{
  int num_params = x->size;
  gsl_vector *v = gsl_vector_alloc(num_params);
  for(int j=0; j<num_params; j++) gsl_vector_set(v, j, gsl_vector_get(x,j));

cout << "df called (";
for(unsigned int i=0; i<x->size; i++) cout << gsl_vector_get(x,i) << ", ";
cout << ")" << endl;

  for(int i=0; i<num_params; i++)
  {
    double delta = (i==0?DELTA0:DELTA);
    double val = gsl_vector_get(x,i);

    gsl_vector_set(v, i, val+delta);
    double y1 = f(v, params_);
    gsl_vector_set(v, i, val-delta);
    double y2 = f(v, params_);
    gsl_vector_set(g, i, (y1-y2)/(2.0*delta));

cout << "  val=" << val << ", delta=" << delta << endl;
cout << "  y1=" << y1 << ", y2=" << y2 << ", df=" << (y1-y2)/(2.0*delta) << endl;
//TODO: si una evaluacio dona 1e6 -> usar diferenciacio numerica no-centrada
    gsl_vector_set(v, i, val);
  }

  gsl_vector_free(v);
}

void ccruncher::CopulaCalibration::fdf(const gsl_vector * x, void * params_, double *y, gsl_vector *g)
{
  *y = f(x, params_);
  df(x, params_, g);

cout << "fdf called (";
for(unsigned int i=0; i<x->size; i++) cout << gsl_vector_get(x,i) << ", ";
cout << ")" << endl;
cout << "  f= " << *y << endl;
cout << "  df= ";
for(unsigned int i=0; i<x->size; i++) cout << gsl_vector_get(g, i) << ", ";
cout << endl;
}
