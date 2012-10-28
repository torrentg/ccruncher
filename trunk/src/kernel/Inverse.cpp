
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
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_cdf.h>
#include "kernel/Inverse.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define EPSILON 1e-10

//===========================================================================
// constructor
//===========================================================================
ccruncher::Inverse::Inverse(double ndf_, Date t0_, Date t1_, const Survivals &survivals) throw(Exception)
{
  assert(t0 < t1);
  assert(2.0 <= ndf);

  ndf = ndf_;
  t0 = t0_;
  t1 = t1_;

  setRanges(survivals);
  setCoefs(survivals);
}

//===========================================================================
// set ranges
// for each rating computes
//   * invF(dnorm(1 day))
//   * invF(dnorm(t1-t0))
// where:
//   * invF is the inverse of the survival function
//   * dnorm is the CDF of the normal o t-Student dist
//   * t1-t0 are the days from t0 (starting date) to t1 (ending date)
//===========================================================================
void ccruncher::Inverse::setRanges(const Survivals &survivals)
{
  int nratings = survivals.getRatings().size();
  range.resize(nratings, pair<double,double>(NAN,NAN));

  for(int irating=0; irating<nratings; irating++)
  {
    if (irating == survivals.getIndexDefault()) {
      // 'default' rating -> range = [NAN,NAN]
      continue;
    }

    //TODO: modify Survivals to CDF
    //TODO: modify survivals to operate with days (not months)
    //TODO: remove from survival fillholes, etc.
    double prob1 = survivals.evalue(irating, 1);
    if (ndf <= 0.0) range[irating].first = gsl_cdf_ugaussian_Pinv(prob1);
    else range[irating].first = gsl_cdf_tdist_Pinv(prob1, ndf);

    double prob2 = survivals.evalue(irating, t1-t0);
    if (ndf <= 0.0) range[irating].second = gsl_cdf_ugaussian_Pinv(prob2);
    else range[irating].second = gsl_cdf_tdist_Pinv(prob2, ndf);

    assert(range[irating].first <= range[irating].second);
  }
}

//===========================================================================
// set interpolation coefficients
//===========================================================================
void ccruncher::Inverse::setCoefs(const Survivals &survivals) throw(Exception)
{
  int nratings = survivals.getRatings().size();
  data.resize(nratings);

  for(int irating=0; irating<nratings; irating++)
  {
    if (irating == survivals.getIndexDefault()) {
      // 'default' rating -> data[irating].size() = 0
      continue;
    }

    data[irating] = getCoefs(irating, survivals, 100);
  }
}

//===========================================================================
// set interpolation coefficients
// n: number of breaks (eg. n=3 -> 4 points -included extremals-)
// we don't use gsl splines because we try to minize memory footprint
//===========================================================================
vector<ccruncher::Inverse::csc> ccruncher::Inverse::getCoefs(int irating, const Survivals &survivals, int n) throw(Exception)
{
  assert(n >= 3);
  // creating equiespaced points
  vector<double> y(n+1, NAN);
  y[0] = 1;
  for(int i=1; i<n-1; i++)
  {
    double x = range[irating].first + i*(range[irating].second-range[irating].first)/(double)n;
    y[i] = survivals.inverse(irating, gsl_cdf_tdist_P(x, ndf));
  }
  y.back() = (double)(t1-t0);

  // http://mathworld.wolfram.com/CubicSpline.html
  gsl_vector *diag = gsl_vector_alloc(n+1);
  gsl_vector *e = gsl_vector_alloc(n);
  gsl_vector *b = gsl_vector_alloc(n+1);
  gsl_vector *x = gsl_vector_alloc(n+1);
  gsl_vector_set_all(diag, 4.0);
  gsl_vector_set_all(e, 1.0);
  gsl_vector_set(diag, 0, 2.0);
  gsl_vector_set(diag, n, 2.0);
  for(int i=1; i<n; i++)
  {
    double val = 3.0*(y[i+1]-y[i-1]);
    gsl_vector_set(b, i, val);
  }
  gsl_vector_set(b, 0, 3.0*(y[1]-y[0]));
  gsl_vector_set(b, n, 3.0*(y[n]-y[n-1]));

  int rc = gsl_linalg_solve_symm_tridiag(diag, e, b, x);
  gsl_vector_free(diag);
  gsl_vector_free(e);
  if (rc != GSL_SUCCESS) {
    gsl_vector_free(b);
    gsl_vector_free(x);
    throw Exception("error computing cubic splines coefficients");
  }

  vector<csc> ret(n);
  const double *d = gsl_vector_const_ptr(x, 0);
  for(int i=0; i<n; i++)
  {
    ret[i].coef[0] = y[i];
    ret[i].coef[1] = d[i];
    ret[i].coef[2] = 3.0*(y[i+1]-y[i]) - 2.0*d[i] - d[i+1];
    ret[i].coef[3] = 2.0*(y[i]-y[i+1]) + d[i] + d[i+1];
  }

  gsl_vector_free(b);
  gsl_vector_free(x);

  return ret;
}

