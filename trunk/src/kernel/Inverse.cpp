
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
#include "utils/Format.hpp"
#include <cassert>

#define NBREAKS 100

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Inverse::Inverse()
{
  ndf = NAN;
  t0 = NAD;
  t1 = NAD;
}

//===========================================================================
// constructor
// if ndf <= 0 then gaussian, t-Student otherwise
//===========================================================================
ccruncher::Inverse::Inverse(double ndf_, const Date &maxdate, const DefaultProbabilities &dprobs) throw(Exception)
{
  init(ndf_, maxdate, dprobs);
}

//===========================================================================
// initialize
// if ndf <= 0 then gaussian, t-Student otherwise
//===========================================================================
void ccruncher::Inverse::init(double ndf_, const Date &maxdate, const DefaultProbabilities &dprobs) throw(Exception)
{
  ndf = ndf_;
  t0 = dprobs.getDate();
  t1 = maxdate;

  if (t1 <= t0) {
    throw Exception("maxdate out of range");
  }

  setRanges(dprobs);
  setCoefs(dprobs);
}

//===========================================================================
// set ranges
// for each rating computes
//   * invF(dnorm(1 day))
//   * invF(dnorm(t1-t0))
// where:
//   * invF is the inverse of the probabilities of default function
//   * dnorm is the CDF of the normal o t-Student dist
//   * t1-t0 are the days from t0 (starting date) to t1 (ending date)
//===========================================================================
void ccruncher::Inverse::setRanges(const DefaultProbabilities &dprobs) throw(Exception)
{
  double prob;
  int nratings = dprobs.getRatings().size();
  ranges.resize(nratings, range());

  for(int irating=0; irating<nratings; irating++)
  {
    if (irating == dprobs.getIndexDefault()) {
      continue;
    }

    if (dprobs.getMaxDate(irating) < t1) {
      throw Exception("dprob[" + dprobs.getRatings().getName(irating) + "] undefined at " + Format::toString(t1));
    }

    ranges[irating].maxday = t1-t0;
    prob = dprobs.evalue(irating, t1-t0);
    if (ndf <= 0.0) ranges[irating].maxval = gsl_cdf_ugaussian_Pinv(prob);
    else ranges[irating].maxval = gsl_cdf_tdist_Pinv(prob, ndf);

    if (ranges[irating].maxval == 0.0)
    {
      ranges[irating].minday = 0;
      ranges[irating].minval = 0.0;
    }
    else
    {
      ranges[irating].minday = 0;
      do
      {
        ranges[irating].minday++;
        prob = dprobs.evalue(irating, ranges[irating].minday);
      }
      while(prob == 0.0 && ranges[irating].minday < ranges[irating].maxday);

      if (ndf <= 0.0) ranges[irating].minval = gsl_cdf_ugaussian_Pinv(prob);
      else ranges[irating].minval = gsl_cdf_tdist_Pinv(prob, ndf);
    }

    assert(ranges[irating].minday <= ranges[irating].maxday);
    assert(ranges[irating].minval <= ranges[irating].maxval);
  }
}

//===========================================================================
// set interpolation coefficients
//===========================================================================
void ccruncher::Inverse::setCoefs(const DefaultProbabilities &dprobs) throw(Exception)
{
  int nratings = dprobs.getRatings().size();
  data.resize(nratings);

  for(int irating=0; irating<nratings; irating++)
  {
    if (irating == dprobs.getIndexDefault()) {
      // 'default' rating -> data[irating].size() = 0
      continue;
    }

    data[irating] = getCoefs(irating, dprobs, NBREAKS);
    //TODO: adjust the number of breaks in order to grant precision
  }
}

//===========================================================================
// set interpolation coefficients
// n: number of breaks (eg. n=3 -> 4 points -included extremals-)
// we don't use gsl splines because we try to minize memory footprint
//===========================================================================
vector<ccruncher::Inverse::csc> ccruncher::Inverse::getCoefs(int irating, const DefaultProbabilities &dprobs, int n) throw(Exception)
{
  assert(n >= 3);
  // creating equiespaced points
  vector<double> y(n+1, NAN);
  y[0] = 1.0;
  for(int i=1; i<n; i++)
  {
    double x = ranges[irating].minval + i*(ranges[irating].maxval-ranges[irating].minval)/(double)n;
    if (ndf <= 0.0) x = gsl_cdf_ugaussian_P(x);
    else x = gsl_cdf_tdist_P(x, ndf);
    y[i] = dprobs.inverse(irating, x);
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

