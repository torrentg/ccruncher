
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
#include <gsl/gsl_min.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_sf_gamma.h>
#include "math/CopulaCalibration.hpp"
#include "math/BlockMatrixChol.hpp"
#include "math/BlockMatrixCholInv.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

#define MIN_NU 2.0
#define MAX_NU 5000.0

//===========================================================================
// Description:
// ------------
// implements the t-Student parameter estimation using the MLE 
// described in paper 'Simulation of High-Dimensional t-Student Copula
// Copulas with a Given Block Matrix Correlation Matrix' by
// Gerard Torrent-Gironella and Josep Fortiana
//
//===========================================================================

//=============================================================
// getObservation
//=============================================================
void ccruncher::CopulaCalibration::getObservation(int row, const fparams *p, double *ret)
{
  for(int i=0,s=0; i<p->k; i++)
  {
    double pct = (double)p->h[row][i].ndefaulted/(double)p->h[row][i].nobligors;
    int num = (int)(pct*p->n[i]);

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

    // equidistributed
    for(int j=1; j<=num; j++)
    {
      ret[s] = (p->p[i]) * double(j)/double(num+1);
      s++;
    }
    for(int j=1; j<=(p->n[i]-num); j++)
    {
      ret[s] = p->p[i] + (1.0 - p->p[i]) * double(j)/double(p->n[i]-num+1);
      s++;
    }

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
/*
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
*/
  }
}

//=============================================================
// function to minimize
// v = values to be optimized
// params = pointer to a fparams struct
//=============================================================
double ccruncher::CopulaCalibration::f(double nu, void *params_)
{
  double ret = 0.0;
  fparams *p = (fparams*) params_;

  //TODO: tractar sectors amb 1 individu

  for(int i=0; i<p->k; i++)
  {
    for(int j=0; j<p->k; j++)
    {
      double h = M_PI/6.0 + 1.0/(0.44593 + 1.3089*nu);
      p->M[i][j] = sin(h*p->sigma[i][j])/sin(h);
    }
  }

  BlockMatrixChol *L = new BlockMatrixChol(p->M, &(p->n[0]), p->k);
  BlockMatrixCholInv *I = L->getInverse();
/*
cout << " COERCED = " << L->isCoerced() << ", NDF = " << nu << ", SIGMA = (";
for(int i=0; i<p->k; i++) for(int j=0; j<p->k; j++) cout << L->getCorrelations()[i][j] << ", ";
cout << ")" << endl;
*/

  // determinant section (we use eigenvalues to avoid accuracy problems)
  const vector<eig> &eigenvalues = L->getEigenvalues();
  double aux0 = 0.0;
  for(unsigned int j=0; j<eigenvalues.size(); j++)
  {
    assert(eigenvalues[j].multiplicity > 0 && eigenvalues[j].value > 0.0);
    aux0 += eigenvalues[j].multiplicity * log(eigenvalues[j].value);
  }
  ret += 0.5*aux0;

  ret -= gsl_sf_lngamma((nu+p->dim)/2.0);
  ret -= (p->dim-1.0)*gsl_sf_lngamma(nu/2.0);
  ret += p->dim*gsl_sf_lngamma((nu+1.0)/2.0);

  ret *= p->h.size();

  // sign inverted because gsl minimize and we try to maximize
  for(int i=0; i<(int)p->h.size(); i++)
  {
    getObservation(i, p, p->x);

    for (int j=0; j<p->dim; j++)
    {
      assert(0.0 <= p->x[j] && p->x[j] <= 1.0);
      p->x[j] = gsl_cdf_tdist_Pinv(p->x[j], nu);
    }

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

  if (L->isCoerced()) {
      p->coerced = true;
      ret *= 1.5;
  }
  else p->coerced = false;

  return ret;
}

//=============================================================
// estimate correlations using method proposed by Douglas Luca in
// 'Default Correlation: From Definition to Proposed Solutions'
// h: h[i,j] is the i-th observation of the sector j
// ret: allocated metrix of size (kxk) where k=num of sectors
//=============================================================
void ccruncher::CopulaCalibration::correls(const vector<vector<hdata> > &h, double **ret) throw(Exception)
{
  if (h.size() < 2) throw Exception("invalid number of observations");
  int k = h[0].size();
  if (k < 1) throw Exception("invalid number of sectors");
  vector<double> p(k,0.0);
  vector<vector<double> > M(k,vector<double>(k,0.0));

  for(int i=0; i<k; i++)
  {
    // computing 1-period default probability for sector i
    for(int t=0; t<(int)h.size(); t++)
    {
      p[i] += (double)h[t][i].ndefaulted/(double)h[t][i].nobligors;
    }
    p[i] /= (double)h.size();

    // computing probability co-default for sector i
    for(int t=0; t<(int)h.size(); t++)
    {
      M[i][i] += (double)(h[t][i].ndefaulted*(h[t][i].ndefaulted-1))/(double)(h[t][i].nobligors*(h[t][i].nobligors-1));
    }
    M[i][i] /= (double)h.size();

    for(int j=0; j<=i; j++)
    {
      // computing probability co-default for sector i and j
      for(int t=0; t<(int)h.size(); t++)
      {
        M[i][j] += (double)(h[t][i].ndefaulted*h[t][j].ndefaulted)/(double)(h[t][i].nobligors*h[t][j].nobligors);
      }
      M[i][j] /= (double)h.size();
      M[j][i] = M[i][j];
    }
  }

  for(int i=0; i<k; i++)
  {
    for(int j=0; j<k; j++)
    {
      ret[i][j] = (M[i][j]-p[i]*p[j])/sqrt(p[i]*(1.0-p[i])*p[j]*(1.0-p[j]));
    }
  }
}

//=============================================================
// estimate ndf t-student using MLE. see 'Simulation of
// highdimensional t-student copulas with a given block correlation
// matrix' by Gerard Torrent-Gironella and Josep Fortiana
// h: h[i,j] is the i-th observation of the sector j
// ret: allocated metrix of size (kxk) where k=num of sectors
//=============================================================
double ccruncher::CopulaCalibration::ndf(double **sigma, const vector<int> &n, const vector<double> &p, const vector<vector<hdata> > &h) throw(Exception)
{
  assert(n.size() > 0);
  assert(n.size() == p.size());
  assert(h.size() > 1);
  assert(h[0].size() == n.size());

  if (n.size() <= 0) throw Exception("invalid number of sectors");
  if (h.size() < 2) throw Exception("invalid number of observations");

  fparams params;

  params.dim = 0;
  for(int i=0; i<(int)n.size(); i++) params.dim += n[i];
  //TODO: check if exist sectors with 0 elements?
  if (params.dim < 2) return 10.0;

  params.k = n.size();
  params.n = n;
  params.p = p;
  params.h = h;
  params.sigma = sigma;
  params.M = Arrays<double>::allocMatrix(params.k, params.k, NAN);
  params.x = Arrays<double>::allocVector(params.dim);
  params.y = Arrays<double>::allocVector(params.dim);

  int status;
  int iter = 0, max_iter = 250;
  gsl_min_fminimizer *s;
  double a=2.0, m=30.0, b=5000.0;
  gsl_function F;

  //-----------
    for(int i=0; i<(int)n.size(); i++)
    {
        for(int j=0; j<(int)n.size(); j++)
        {
            cout << sigma[i][j] << " ";
        }
        cout << endl;
    }
    double fa = f(a, &params);
    double fb = f(b, &params);
    cout << "F(" << a << ") = " << fa << endl;
    cout << "F(" << b << ") = " << fb << endl;
    double m1 = a+0.5;
    double fm1 = f(m1, &params);
    cout << "F(" << m1 << ") = " << fm1 << endl;
    double m2 = b-500;
    double fm2 = f(m2, &params);
    cout << "F(" << m2 << ") = " << fm2 << endl;
/*
    for(int i=2; i<1000; i++)
    {
        cout << "F(" << i << ") = " << f(i, &params) << endl;
    }
*/
    if (fm1 < fa && fm1 < fb) m = m1;
    else if (fm2 < fa && fm2 < fb) m = m2;
    else return std::min(a,b);
cout << "a=" << a << ", b=" << b << ", m=" << m << endl;
  //-----------

  F.function = CopulaCalibration::f;
  F.params = (void *)(&params);

  s = gsl_min_fminimizer_alloc(gsl_min_fminimizer_brent);
  gsl_min_fminimizer_set(s, &F, m, a, b);

  printf ("%5s [%9s, %9s] %9s %9s\n",
          "iter", "lower", "upper", "min", "err(est)");

  printf ("%5d [%.7f, %.7f] %.7f %.7f\n",
          iter, a, b, m, b - a);

  do
    {
      iter++;
      status = gsl_min_fminimizer_iterate(s);

      m = gsl_min_fminimizer_x_minimum(s);
      a = gsl_min_fminimizer_x_lower(s);
      b = gsl_min_fminimizer_x_upper(s);

      status = gsl_min_test_interval (a, b, 0.001, 0.0);

      if (status == GSL_SUCCESS) printf ("Converged:\n");

      printf ("%5d [%.7f, %.7f] %.7f %.7f\n",
              iter, a, b, m, b - a);
    }
  while (status == GSL_CONTINUE && iter < max_iter);

  gsl_min_fminimizer_free(s);
  Arrays<double>::deallocMatrix(params.M, params.k);
  Arrays<double>::deallocVector(params.x);
  Arrays<double>::deallocVector(params.y);

  return status;
}
