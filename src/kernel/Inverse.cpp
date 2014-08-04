
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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
#include <algorithm>
#include <cassert>
#include <gsl/gsl_cdf.h>
#include "kernel/Inverse.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-12
// maximum error = 1 hour
#define MAX_ERROR 1.0/24.0

/**************************************************************************/
ccruncher::Inverse::Inverse()
{
  mNdf = NAN;
  mMaxT = NAN;
  mSpline = nullptr;
}

/**************************************************************************//**
 * @param[in] ndf Degrees of freedom of the t-student distribution. If
 *            ndf <= 0 then gaussian, t-student otherwise.
 * @param[in] maxt Maximum time (in days from starting date).
 * @param[in] cdf Default probability cdf.
 * @throw Exception Error creating inverse function.
 */
ccruncher::Inverse::Inverse(double ndf, double maxt, const CDF &cdf)
{
  mNdf = NAN;
  mMaxT = NAN;
  mSpline = nullptr;
  init(ndf, maxt, cdf);
}

/**************************************************************************//**
 * @param[in] o Object to copy.
 */
ccruncher::Inverse::Inverse(const Inverse &o)
{
  *this = o;
}

/**************************************************************************/
ccruncher::Inverse::~Inverse()
{
  if (mSpline != nullptr) {
    gsl_spline_free(mSpline);
  }
}

/**************************************************************************//**
 * @param[in] o Object to assign.
 * @return Reference to the current instance.
 */
Inverse & ccruncher::Inverse::operator=(const Inverse &o)
{
  mMaxT = o.mMaxT;
  mNdf = o.mNdf;
  mSpline = nullptr;

  if (o.mSpline != nullptr) {
    mSpline = gsl_spline_alloc(o.mSpline->interp->type, o.mSpline->size);
    gsl_spline_init(mSpline, o.mSpline->x, o.mSpline->y, o.mSpline->size);
  }

  return *this;
}

/**************************************************************************//**
 * @param[in] ndf Degrees of freedom of the t-student distribution. If
 *            ndf <= 0 then gaussian, t-student otherwise.
 * @param[in] maxt Maximum time (in days from starting date).
 * @param[in] cdf Default probability cdf.
 * @throw Exception Error creating inverse function.
 */
void ccruncher::Inverse::init(double ndf, double maxt, const CDF &cdf)
{
  if (maxt < 1.0) {
    throw Exception("maxt out of range");
  }

  mNdf = ndf;
  mMaxT = maxt;

  setSpline(cdf);
}

/**************************************************************************//**
 * @details Return the first day bigger than 0 where dprobs(day) > 0.
 * @param[in] cdf Default probability cdf.
 * @return First day having DP > 0.
 */
int ccruncher::Inverse::getMinDay(const CDF &cdf) const
{
  for(int day=1; day<mMaxT; day++)
  {
    if (1e-8 < cdf.evalue((double)day))
    {
      return day;
    }
  }
  return mMaxT;
}

/**************************************************************************//**
 * @param[in] u Probability value in [0,1].
 * @return Quantile of the gaussian/t-Student distribution.
 */
double ccruncher::Inverse::icdf(double u) const
{
  assert(0.0 <= u && u <= 1.0);
  if (mNdf <= 0.0) return gsl_cdf_ugaussian_Pinv(u);
  else return gsl_cdf_tdist_Pinv(u, mNdf);
}

/**************************************************************************//**
 * @details Creates a spline function of t=icdf(tcdf(x)), where:
 *            - t is the default time (in days from starting time)
 *            - x is a value in range [-inf,+inf],
 *            - tcdf() is the t-Student cdf
 *            - icdf() is the inverse if the default probability cdf.
 *
 *          With an error less than 1 hour, then use
 *          these values to create spline functions using (x, t) -observe that
 *          are reversed-.
 *          Determines the spline with the minimum number of nodes. Nodes
 *          coming from dprobs have precedence over interpolated values.
 * @param[in] cdf Default probability cdf.
 * @throw Exception Error creating spline functions.
 */
void ccruncher::Inverse::setSpline(const CDF &cdf)
{
  if (cdf.evalue(0.0) > 1.0-EPSILON) {
    return; // default rating
  }
  else if (cdf.getPoints().back().first < mMaxT) {
    throw Exception("cdf not defined in time range");
  }

  int minday = getMinDay(cdf);
  int maxday = mMaxT;

  if (maxday <= minday) {
    // constant function
    mSpline = gsl_spline_alloc(gsl_interp_linear, 2);
    mSpline->x[0] = icdf(cdf.evalue(maxday));
    mSpline->x[1] = icdf(cdf.evalue(maxday));
    mSpline->y[0] = maxday;
    mSpline->y[1] = maxday;
    return;
  }

  vector<int> nodes(cdf.getPoints().size());
  for(size_t i=0; i<cdf.getPoints().size(); i++) nodes[i] = cdf.getPoints()[i].first;

  while(!nodes.empty() && nodes[0] < minday) nodes.erase(nodes.begin());
  if (nodes.empty() || nodes[0] != minday) nodes.insert(nodes.begin(), minday);
  while(!nodes.empty() && nodes.back() > maxday) nodes.erase(nodes.end()-1);
  if (nodes.empty() || nodes.back() != maxday) nodes.insert(nodes.end(), maxday);
  assert(nodes.size() >= 2);

  vector<int> days(1, minday);
  int dayko = maxday;

  // we create a temporary cache because icdf is very expensive
  vector<double> cache(maxday+1, NAN);
  for(int i=minday; i<=maxday; i++) {
    cache[i] = icdf(cdf.evalue((double)i));
  }

  do
  {
    auto pos1 = lower_bound(days.begin(), days.end(), dayko);
    assert(days.begin() < pos1);
    auto pos2 = lower_bound(nodes.begin(), nodes.end(), dayko);
    assert(nodes.begin() < pos2 && pos2 < nodes.end());
    if (dayko == *pos2) {
      // dayko is a remaining node
      days.insert(pos1, dayko);
    }
    else { // dayko is not a node
      vector<int> alternatives;
      if (*(pos1-1) < *(pos2-1)) alternatives.push_back(*(pos2-1)); // left-node
      if (*pos2 < *pos1) alternatives.push_back(*pos2); // right-node
      if (alternatives.empty()) {
        // left and right nodes already set
        days.insert(pos1, dayko);
      }
      else if (alternatives.size() == 1) {
        // inserting remaining node
        days.insert(pos1, alternatives[0]);
      }
      else if (dayko-alternatives[0] <= alternatives[1]-dayko) {
        // inserting nearest node (left case)
        days.insert(pos1, alternatives[0]);
      }
      else {
        // inserting nearest node (right case)
        days.insert(pos1, alternatives[1]);
      }
    }
    setSpline(days, cache);
    dayko = getWorstDay(cache);
  }
  while(dayko > 0 && (int)days.size() < mMaxT);
}

/**************************************************************************//**
 * @param[in] days List of days ().
 * @param[in] cache icdf(dp(t)) values evaluated at days.
 */
void ccruncher::Inverse::setSpline(vector<int> &days, vector<double> &cache)
{
  assert(days.size() >= 2);

  if (mSpline != nullptr) {
    gsl_spline_free(mSpline);
  }

  vector<double> x(days.size(), 0.0);
  vector<double> y(days.size(), 0.0);

  for(size_t i=0; i<days.size(); i++)
  {
    y[i] = days[i];
    x[i] = cache[days[i]]; //icdf(dprobs.evalue(irating, y[i]));
    //TODO: manage +inf, -inf cases
  }

  if (gsl_interp_type_min_size(gsl_interp_cspline) <= x.size()) {
    mSpline = gsl_spline_alloc(gsl_interp_cspline, x.size());
  }
  else {
    mSpline = gsl_spline_alloc(gsl_interp_linear, x.size());
  }
  gsl_spline_init(mSpline, &(x[0]), &(y[0]), x.size());
}

/**************************************************************************//**
 * @details Evaluate current spline at returns worst accurate day.
 * @param[in] cache icdf(dp(t)) values evaluated at days.
 * @return Worst day, if all days are accurate, returns 0.
 */
int ccruncher::Inverse::getWorstDay(vector<double> &cache)
{
  int ret=0;
  double val=0.0;

  int n = mSpline->size - 1;
  assert(mSpline->y[0] >= 0);
  // rounded to nearest integer (positive values)
  int d1 = (int)(mSpline->y[0] + 0.5);
  assert(mSpline->y[n] >= 0);
  // rounded to nearest integer (positive values)
  int d2 = (int)(mSpline->y[n] + 0.5);

  for(int i=d1+1; i<d2; i++)
  {
    double x = cache[i];
    double days = evalue(x);
    double err = fabs(days-i);
    if (err > MAX_ERROR && err > val) {
      val = err;
      ret = i;
    }
  }

  return ret;
}

