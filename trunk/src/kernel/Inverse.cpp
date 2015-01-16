
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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

#define EPSILON 1e-10
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
 * @param[in] nodes Days from starting date where asset events ocurres.
 * @throw Exception Error creating inverse function.
 */
ccruncher::Inverse::Inverse(double ndf, double maxt, const CDF &cdf, const vector<int> &nodes)
{
  mNdf = NAN;
  mMaxT = NAN;
  mSpline = nullptr;
  init(ndf, maxt, cdf, nodes);
}

/**************************************************************************//**
 * @param[in] o Object to copy.
 */
ccruncher::Inverse::Inverse(const Inverse &o) : mSpline(nullptr)
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

  if (mSpline != nullptr) {
    gsl_spline_free(mSpline);
    mSpline = nullptr;
  }

  if (o.mSpline != nullptr) {
    mSpline = gsl_spline_alloc(o.mSpline->interp->type, o.mSpline->size);
    gsl_spline_init(mSpline, o.mSpline->x, o.mSpline->y, o.mSpline->size);
  }

  return *this;
}

/**************************************************************************//**
 * @details We added this method for debug purposes.
 * @return Number of nodes used by spline.
 */
size_t ccruncher::Inverse::size() const
{
  return (mSpline==nullptr?0:mSpline->size);
}

/**************************************************************************//**
 * @param[in] ndf Degrees of freedom of the t-student distribution. If
 *            ndf <= 0 then gaussian, t-student otherwise.
 * @param[in] maxt Maximum time (in days from starting date).
 * @param[in] cdf Default probability cdf.
 * @param[in] nodes Days from starting date where asset events ocurres.
 * @throw Exception Error creating inverse function.
 */
void ccruncher::Inverse::init(double ndf, double maxt, const CDF &cdf, const vector<int> &nodes)
{
  if (maxt < 1.0) {
    throw Exception("maxt out of range");
  }

  mNdf = ndf;
  mMaxT = maxt;

  setSpline(cdf, nodes);
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
    if (EPSILON < cdf.evalue((double)day))
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
double ccruncher::Inverse::tinv(double u) const
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
 *          Determines the spline with the minimum number of nodes.
 * @param[in] cdf Default probability cdf.
 * @param[in] nodes Days where assets events are defined.
 * @throw Exception Error creating spline functions.
 */
void ccruncher::Inverse::setSpline(const CDF &cdf, const vector<int> &nodes_)
{
  if (mSpline != nullptr) {
    gsl_spline_free(mSpline);
    mSpline = nullptr;
  }

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
    mSpline->x[0] = tinv(cdf.evalue(maxday));
    mSpline->x[1] = tinv(cdf.evalue(maxday));
    mSpline->y[0] = maxday;
    mSpline->y[1] = maxday;
    return;
  }

  vector<int> nodes(nodes_.begin(), nodes_.end());
  sort(nodes.begin(), nodes.end());
  while(!nodes.empty() && nodes[0] < minday) nodes.erase(nodes.begin());
  if (nodes.empty() || nodes[0] != minday) nodes.insert(nodes.begin(), minday);
  while(!nodes.empty() && nodes.back() > maxday) nodes.erase(nodes.end()-1);
  if (nodes.empty() || nodes.back() != maxday) nodes.insert(nodes.end(), maxday);
  assert(nodes.size() >= 2);

  // we create a temporary cache because icdf is very expensive
  map<int,double> cache;
  for(size_t i=0; i<nodes.size(); i++) {
    int day = nodes[i];
    cache[day] = tinv(cdf.evalue(day));
  }

  vector<int> days(1, minday);
  int dayko = maxday;

  while(dayko > 0)
  {
    auto pos = lower_bound(days.begin(), days.end(), dayko);
    assert(days.begin() < pos);
    assert(find(days.begin(), days.end(), dayko) == days.end());
    days.insert(pos, dayko);
    setSpline(days, cache);
    dayko = getWorstDay(nodes, cache);
  }

  //for(size_t i=0; i<days.size(); i++) {
  //  cout << "day[" << i+1 << "] = " << days[i] << std::endl;
  //}

}

/**************************************************************************//**
 * @param[in] days List of days.
 * @param[in] cache tinv(cdf(t)) values evaluated at days.
 */
void ccruncher::Inverse::setSpline(const vector<int> &days, const map<int,double> &cache)
{
  assert(days.size() >= 2);

  if (mSpline != nullptr) {
    gsl_spline_free(mSpline);
    mSpline = nullptr;
  }

  vector<double> x(days.size(), 0.0);
  vector<double> y(days.size(), 0.0);

  for(size_t i=0; i<days.size(); i++)
  {
    y[i] = days[i];
    x[i] = cache.at(days[i]); //tinv(dprobs.evalue(irating, y[i]));
    //TODO: manage +inf, -inf cases
  }

  if (gsl_interp_type_min_size(gsl_interp_cspline) <= x.size()) {
    mSpline = gsl_spline_alloc(gsl_interp_cspline, x.size());
    gsl_spline_init(mSpline, &(x[0]), &(y[0]), x.size());
    if (!isIncreasing(mSpline)) {
      gsl_spline_free(mSpline);
      mSpline = nullptr;
    }
  }

  if (mSpline == nullptr) {
    mSpline = gsl_spline_alloc(gsl_interp_linear, x.size());
    gsl_spline_init(mSpline, &(x[0]), &(y[0]), x.size());
  }

}

/**************************************************************************//**
 * @details Evaluate current spline at returns worst accurate day.
 * @param[in] nodes Days where assets events are defined.
 * @param[in] cache tinv(cdf(t)) values evaluated at nodes.
 * @return Worst day. If all days are accurate, returns 0.
 */
int ccruncher::Inverse::getWorstDay(const vector<int> &nodes, const map<int,double> &cache) const
{
  int worstDay = 0;
  double maxErr = 0.0;

  for(size_t i=0; i<nodes.size(); i++)
  {
    double x = cache.at(nodes[i]);
    double t = evalue(x);
    double err = fabs(t-nodes[i]);
    if (err > MAX_ERROR && err > maxErr) {
      maxErr = err;
      worstDay = nodes[i];
    }
  }

  return worstDay;
}

/**************************************************************************//**
 * @details Returned values are:
 *          - "none": not interpolated.
 *          - "linear": linear interpolation.
 *          - "cspline": cubic spline interpolation.
 * @return Type of interpolation used.
 */
string ccruncher::Inverse::getInterpolationType() const
{
  if (mSpline == nullptr) {
    return "none";
  }
  return gsl_interp_name(mSpline->interp);
}

/**************************************************************************//**
 * @details Check if the spline function is monotonically increasing
 *          checking its derivatives at nodes.
 * @return true = is increasing, false = otherwise.
 */
bool ccruncher::Inverse::isIncreasing(const gsl_spline *spline)
{
  assert(spline != nullptr);
  if (spline == nullptr) return false;

  for(size_t i=0; i<spline->size; i++)
  {
    double deriv = gsl_spline_eval_deriv(spline, spline->x[i], nullptr);
    if (deriv < 0.0) {
      return false;
    }
  }
  return true;
}

