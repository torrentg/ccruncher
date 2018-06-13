
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#include <cassert>
#include <algorithm>
#include "params/CDF.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-12

/**************************************************************************//**
 * @param[in] xmin Minimum x value.
 * @param[in] xmax Maximum x value.
 */
ccruncher::CDF::CDF(double xmin, double xmax)
{
  setRange(xmin, xmax);
}

/**************************************************************************//**
 * @param[in] o Object to copy.
 * @see CDF::operator=
 */
ccruncher::CDF::CDF(const CDF &o)
{
  *this = o;
}

/**************************************************************************//**
 * @param[in] data Function points provided by user. x-values are in range
 *          (-inf,+inf) and y-values are in range [0,1]. Note that points
 *          can be unordered (respect x-component).
 * @param[in] xmin Minimum x value.
 * @param[in] xmax Maximum x value.
 * @exception Points out of range, or non increasing  function.
 */
ccruncher::CDF::CDF(const std::vector<std::pair<double,double>> &data,
    double xmin, double xmax)
{
  setRange(xmin, xmax);
  for(auto &p : data) {
    add(p.first, p.second);
  }
}

/**************************************************************************//**
 * @param[in] x x-values are in range (-inf,+inf).
 * @param[in] y x-values are in range (-inf,+inf).
 * @param[in] xmin Minimum x value.
 * @param[in] xmax Maximum x value.
 * @exception x and y lengths are distinct, or values are out of range,
 *         or non increasing  function.
 */
ccruncher::CDF::CDF(const std::vector<double> &x, const std::vector<double> &y,
    double xmin, double xmax)
{
  setRange(xmin, xmax);
  if (x.size() != y.size()) {
    throw Exception("vector lengths are differents");
  }

  for(size_t i=0; i<x.size(); i++) {
    add(x[i], y[i]);
  }
}

/**************************************************************************//**
 * @param[in] xmin Minimum x value.
 * @param[in] xmax Maximum x value.
 * @exception Exists values out of range
 */
void ccruncher::CDF::setRange(double xmin, double xmax)
{
  if (xmax <= xmin) {
    throw Exception("invalid range");
  }
  if (!mData.empty()) {
    if (mData.front().first < xmin || xmax < mData.back().first) {
      throw Exception("there are values out of range");
    }
  }
  mXmin = xmin;
  mXmax = xmax;
  freeSpline();
}

/**************************************************************************/
ccruncher::CDF::~CDF()
{
  freeSpline();
}

/**************************************************************************//**
 * @details Assign the given object reallocating its own objects.
 * @param[in] o Object to assign.
 */
CDF& ccruncher::CDF::operator=(const CDF &o)
{
  freeSpline();
  mXmin = o.mXmin;
  mXmax = o.mXmax;
  mData = o.mData;
  return *this;
}

/**************************************************************************/
void ccruncher::CDF::freeSpline()
{
  gsl_spline_free(mSpline);
  mSpline = nullptr;

  gsl_interp_accel_free(mAccel);
  mAccel = nullptr;
}

/**************************************************************************//**
 * @details Check ranges and grants that data is x-ordered.
 * @param[in] x x-component, in the (-inf,+inf) range.
 * @param[in] prob y-component is a probability in [0,1] range.
 * @exception values out of range, repeated point, or decreasing function.
 */
void ccruncher::CDF::add(double x, double prob)
{
  if (std::isnan(x)) {
    throw Exception("invalid x value");
  }
  else if (std::isinf(+x) || mXmax <= x) {
    if(prob != 1.0) throw Exception("invalid x value");
    else return;
  }
  else if (std::isinf(-x) || x < mXmin) {
    if(prob != 0.0) throw Exception("invalid x value");
    else return;
  }

  if (std::isnan(prob) || prob < 0.0 || 1.0 < prob) {
    throw Exception("invalid prob value");
  }

  pair<double,double> point(x,prob);
  auto it = lower_bound(mData.begin(), mData.end(), point);

  if (it != mData.end()) {
    if (it->first == x || it->second + EPSILON < prob) {
      throw Exception("repeated value or decreasing function");
    }
  }
  else if (!mData.empty() && mData.back().second > prob + EPSILON) {
    throw Exception("decreasing function");
  }

  mData.insert(it, point);
  freeSpline();
}

/**************************************************************************//**
 * @details Internal method that creates the spline function.
 *          Create linear/cubic spline according to these rules:
 *          - less than 3 values -> linear
 *          - There are negative derivatives in the cubic spline -> linear
 *          - otherwise -> cubic
 *          Although data values are increasing, the cubic spline can be
 *          a non-increasing. In this case we use linear splines.
 * @exception No points defined
 */
void ccruncher::CDF::setSpline() const
{
  assert(mSpline == nullptr);
  assert(mAccel == nullptr);

  vector<double> x;
  vector<double> y;

  for(size_t i=0; i<mData.size(); i++)
  {
    x.push_back(mData[i].first);
    y.push_back(mData[i].second);
  }

  if (!std::isinf(-mXmin)) {
    // because cdf(mXmin) can be > 0
    if (x.empty() || mXmin < x[0]) {
      x.insert(x.begin(), mXmin);
      y.insert(y.begin(), 0.0);
    }
  }

  if (!std::isinf(mXmax)) {
    x.push_back(mXmax);
    y.push_back(1.0);
  }

  // removing consecutive equal y values
  for(size_t i=1; i<y.size()-1; i++) {
    if (fabs(y[i-1]-y[i]) < EPSILON && fabs(y[i+1]-y[i]) < EPSILON) {
      x.erase(x.begin()+i);
      y.erase(y.begin()+i);
      i--;
    }
  }

  // removing repeated starting x values
  size_t pos1 = 0;
  for(size_t i=1; i<x.size(); i++) {
    if (fabs(x[i]-x[0]) < EPSILON) pos1 = i;
    else break;
  }
  if (pos1 > 0) {
    x.erase(x.begin(), x.begin()+pos1);
  }

  // removing repeated ending x values
  size_t pos2 = x.size()-1;
  for(int i=pos2-1; i>=0; i--) {
    if (fabs(x[i]-x.back()) < EPSILON) pos2 = i;
    else break;
  }
  if (pos2 < x.size()-1) {
    x.erase(x.begin()+pos2+1, x.end());
  }

  size_t n = x.size();

  if (n == 0 || mData.size() == 0) {
    throw Exception("non points defined");
  }
  else if (n == 1) {
    x.push_back(x[0]+1.0); // TODO: revise this
    y.push_back(y[0]);
    n++;
  }

  bool iscspline = false;

  if (gsl_interp_type_min_size(gsl_interp_cspline) <= n)
  {
    mSpline = gsl_spline_alloc(gsl_interp_cspline, n);
    gsl_spline_init(mSpline, &(x[0]), &(y[0]), n);
    iscspline = true;

    for(size_t i=0; i<n; i++)
    {
      double deriv = gsl_spline_eval_deriv(mSpline, x[i], nullptr);
      if (deriv < 0.0) {
        gsl_spline_free(mSpline);
        mSpline = nullptr;
        iscspline = false;
        break;
      }
    }
  }

  if (!iscspline)
  {
    mSpline = gsl_spline_alloc(gsl_interp_linear, n);
    gsl_spline_init(mSpline, &(x[0]), &(y[0]), n);
  }

  assert(mSpline->size == n);
  mAccel = gsl_interp_accel_alloc();
}

/**************************************************************************//**
 * @param[in] x Value in range (-inf,+inf).
 * @return Probability value in range [0,1].
 */
double ccruncher::CDF::evalue(double x) const
{
  if (mSpline == nullptr) {
    setSpline();
  }

  int n = mSpline->size - 1;

  if (x < mXmin) {
    return 0.0;
  }
  else if (x >= mXmax) {
    return 1.0;
  }
  else if (mSpline->x[n] <= x) {
    return mSpline->y[n];
  }
  else if (x <= mSpline->x[0]) {
    return mSpline->y[0];
  }
  else {
    double prob = gsl_spline_eval(mSpline, x, mAccel);
    assert(0.0 <= prob && prob <= 1.0);
    return prob;
  }
}

/**************************************************************************//**
 * @details Returned values are:
 *          - "none": not interpolated.
 *          - "linear": linear interpolation.
 *          - "cspline": cubic spline interpolation.
 * @return Type of interpolation used.
 */
string ccruncher::CDF::getInterpolationType() const
{
  if (mSpline == nullptr) {
    try {
      setSpline();
    }
    catch(...) {
      return "none";
    }
  }

  return gsl_interp_name(mSpline->interp);
}

