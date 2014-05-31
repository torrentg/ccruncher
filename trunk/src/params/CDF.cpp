
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
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

#include <cassert>
#include <algorithm>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>
#include "params/CDF.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-12
#define MAX_ITER_BISECTION 100
#define MAX_ITER_NEWTON 40
// absolute error less than 1 sec (1/(24*60*60)=1.16e-5)
#define ABS_ERR_ROOT 1e-7

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
  if (mSpline != nullptr) {
    gsl_spline_free(mSpline);
    mSpline = nullptr;
  }
  if (mAccel != nullptr) {
    gsl_interp_accel_free(mAccel);
    mAccel = nullptr;
  }
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
  auto it = lower_bound(mData.begin(), mData.end(), point,
      [](const pair<double,double> &p1, const pair<double,double> &p2) {
          return p1.first < p2.first;
      });

  if (it != mData.end()) {
    if (it->first == x || it->second < prob) {
      throw Exception("repeated value or decreasing function");
    }
  }
  else if (!mData.empty() && mData.back().second > prob) {
    throw Exception("decreasing function");
  }

  mData.insert(it, point);
  freeSpline();
}

/**************************************************************************//**
 * @details Evaluate cdf taking care in the extremes.
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] x Value in range (-inf,+inf).
 * @param[in] params Root-finding function parameters (includes splines to use).
 * @return Probability, value in [0,1].
 */
double ccruncher::CDF::f(double x, void *params)
{
  fparams *p = static_cast<fparams *>(params);
  if (x <= p->spline->x[0]) {
    double x0 = p->spline->x[0];
    double y0 = p->spline->y[0];
    double deriv = gsl_spline_eval_deriv(p->spline, x0, p->accel);
    return y0 + deriv*(x-x0) - p->y;
  }
  else if (p->spline->x[p->spline->size-1] <= x) {
    double x1 = p->spline->x[p->spline->size-1];
    double y1 = p->spline->y[p->spline->size-1];
    double deriv = gsl_spline_eval_deriv(p->spline, x1, p->accel);
    return y1 + deriv*(x-x1) - p->y;
  }
  else {
    return gsl_spline_eval(p->spline, x, p->accel) - p->y;
  }
}

/**************************************************************************//**
 * @details Evaluate cdf'(x) -derivative- taking care in the extremes.
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] x Value in range (-inf,+inf).
 * @param[in] params Root-finding function parameters (includes splines to use).
 * @return Probability, value in [0,1].
 */
double ccruncher::CDF::df(double x, void *params)
{
  fparams *p = static_cast<fparams *>(params);
  if (x <= p->spline->x[0]) {
    x = p->spline->x[0];
  }
  else if (p->spline->x[p->spline->size-1] <= x) {
    x = p->spline->x[p->spline->size-1];
  }
  return gsl_spline_eval_deriv(p->spline, x, p->accel);
}

/**************************************************************************//**
 * @details Evaluate cdf(x) and cdf'(x) simultaneously.
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] x Value in range (-inf,+inf).
 * @param[in] params Root-finding function parameters (includes splines to use).
 * @param[out] y Probability, value in [0,1].
 * @param[out] dy Derivative of cdf at x (>= 0)
 */
void ccruncher::CDF::fdf (double x, void *params, double *y, double *dy)
{
  *y = f(x, params);
  *dy = df(x, params);
}

/**************************************************************************//**
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] y Probability in [0,1].
 * @return Value x satisfying cdf(x)=y.
 */
double ccruncher::CDF::inverseLinear(double y) const
{
  assert(mSpline != nullptr);
  assert(mAccel != nullptr);

  int status;
  int iter = 0;
  double root = 0;
  double x_lo = mSpline->x[0];
  double x_hi = mSpline->x[mSpline->size-1];
  gsl_function F;
  fparams params = {mSpline, mAccel, y};

  F.function = &ccruncher::CDF::f;
  F.params = &params;

  gsl_root_fsolver *solver = gsl_root_fsolver_alloc(gsl_root_fsolver_bisection);
  gsl_root_fsolver_set(solver, &F, x_lo, x_hi);

  do
  {
    iter++;
    gsl_root_fsolver_iterate(solver);
    root = gsl_root_fsolver_root(solver);
    x_lo = gsl_root_fsolver_x_lower(solver);
    x_hi = gsl_root_fsolver_x_upper(solver);
    status = gsl_root_test_interval(x_lo, x_hi, ABS_ERR_ROOT, 0.0);
  }
  while (status == GSL_CONTINUE && iter < MAX_ITER_BISECTION);

  gsl_root_fsolver_free(solver);

  return root;
}

/**************************************************************************//**
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] y Probability in [0,1].
 * @return Value x satisfying cdf(x)=y.
 */
double ccruncher::CDF::inverseCubic(double y) const
{
  assert(mSpline != nullptr);
  assert(mAccel != nullptr);

  int status;
  int iter = 0;
  double root = (mSpline->x[mSpline->size-1]+mSpline->x[0])/2.0;
  double x0 = 0.0;
  gsl_function_fdf FDF;
  fparams params = {mSpline, mAccel, y};

  FDF.f = &ccruncher::CDF::f;
  FDF.df = &ccruncher::CDF::df;
  FDF.fdf = &ccruncher::CDF::fdf;
  FDF.params = &params;

  gsl_root_fdfsolver *solver = gsl_root_fdfsolver_alloc(gsl_root_fdfsolver_newton);
  gsl_root_fdfsolver_set(solver, &FDF, root);

  do
  {
    iter++;
    gsl_root_fdfsolver_iterate(solver);
    x0 = root;
    root = gsl_root_fdfsolver_root(solver);
    status = gsl_root_test_delta(root, x0, ABS_ERR_ROOT, 0.0);
  }
  while (status == GSL_CONTINUE && iter < MAX_ITER_NEWTON);

  gsl_root_fdfsolver_free(solver);

  if (iter < MAX_ITER_NEWTON) {
    return root;
  }
  else {
    return inverseLinear(y);
  }
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

  // removing repeated starting values
  size_t pos1 = 0;
  for(size_t i=1; i<x.size(); i++) {
    if (fabs(x[i]-x[0]) < EPSILON) pos1 = i;
    else break;
  }
  if (pos1 > 0) {
    x.erase(x.begin(), x.begin()+pos1);
  }

  // removing repeated ending values
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
 * @details Evalue inv_cdf(prob), where prob is a probability. Cdf is
 *          invertible because it is an strictly increasing function.
 *          Inverse value is obtained by root-finding methods.
 * @note    Inverse of a spline isn't a spline.
 * @note    inv(spline(x,y)) != spline(y,x)
 * @note    This is not a high-performance method
 * @param[in] prob Probability, value in range [0,1].
 * @return Value x fulfiling cdf(x)=prob.
 * @exception Given prob out of range [0,1]
 */
double ccruncher::CDF::inverse(double prob) const
{
  if (mSpline == nullptr) {
    setSpline();
  }

  if (prob < 0.0 || prob > 1.0) {
    throw Exception("prob out-of-range");
  }

  if (mData.back().second < prob) {
    return mData.back().first + 1.0;
  }
  else {
    if (mSpline->interp->type == gsl_interp_cspline) {
      return inverseCubic(prob);
    }
    else if (mSpline->interp->type == gsl_interp_linear) {
      return inverseLinear(prob);
    }
    else {
      assert(false);
      return NAN;
    }
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

