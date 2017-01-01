
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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
#include "params/Interest.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] date Date on which the curve is defined.
 * @param[in] interestType Type of interest to apply.
 * @param[in] splineType Type of spline to use in interpolation.
 */
ccruncher::Interest::Interest(const Date &date, InterestType interestType, SplineType splineType) :
    mSpline(nullptr), mAccel(nullptr)
{
  mDate = date;
  mInterestType = interestType;
  mSplineType = splineType;
  isDirty = false;
}

/**************************************************************************//**
 * @see Interest::operator=
 * @param[in] o Object to copy.
 */
ccruncher::Interest::Interest(const Interest &o) : mSpline(nullptr), mAccel(nullptr)
{
  *this = o;
}

/**************************************************************************/
ccruncher::Interest::~Interest()
{
  gsl_spline_free(mSpline);
  gsl_interp_accel_free(mAccel);
}

/**************************************************************************//**
 * @details Assign the given object reallocating its own objects.
 * @param[in] o Object to assign.
 */
Interest & ccruncher::Interest::operator=(const Interest &o)
{
  isDirty = o.isDirty;
  mInterestType = o.mInterestType;
  mSplineType = o.mSplineType;
  mDate = o.mDate;
  mRates = o.mRates;

  gsl_spline_free(mSpline);
  mSpline = nullptr;

  gsl_interp_accel_free(mAccel);
  mAccel = nullptr;

  if (o.mSpline != nullptr && !isDirty) {
    mSpline = gsl_spline_alloc(o.mSpline->interp->type, o.mSpline->size);
    gsl_spline_init(mSpline, o.mSpline->x, o.mSpline->y, o.mSpline->size);
  }

  if (o.mAccel != nullptr && !isDirty) {
    mAccel = gsl_interp_accel_alloc();
    gsl_interp_accel_reset(mAccel);
  }

  return *this;
}

/**************************************************************************//**
 * @details Checks that it is a valid rate, non-repeated time, etc.
 * @param[in] rates List of rates.
 * @throw Exception Error inserting rates.
 */
void ccruncher::Interest::insertRates(const vector<Rate> &rates)
{
  for(Rate rate : rates) {
    insertRate(rate);
  }
}

/**************************************************************************//**
 * @details Checks that it is a valid rate, non-repeated time, etc.
 * @param[in] rate Rate to insert.
 * @throw Exception Error inserting rate.
 */
void ccruncher::Interest::insertRate(const Rate &rate)
{
  if (mDate == NAD || rate.date == NAD || rate.date < mDate) {
    throw Exception("invalid rate date");
  }

  // checking interest rate value
  if (rate.value < -0.5 || 1.0 < rate.value) {
    throw Exception("rate value '" + to_string(rate.value) + "' out of range [-0.5, +1.0]");
  }

  // finding insert position (mRates is a date-sorted vector)
  auto pos = upper_bound(mRates.begin(), mRates.end(), rate);

  // checking if previously defined
  if (!mRates.empty() && pos != mRates.begin() && rate.date == (pos-1)->date) {
    throw Exception("repeated rate date");
  }

  // checking rate date
  if (mDate != NAD && rate.date < mDate) {
    throw Exception("rate date before interest date");
  }

  // inserting value (preserving order)
  mRates.insert(pos, rate);
  isDirty = true;
}

/**************************************************************************//**
 * @details Checks that it is a valid rate, non-repeated time, etc.
 * @param[in] date Date (date > curve date).
 * @param[in] r Rate (-0.5 < r < 1.0).
 * @throw Exception Error inserting rate.
 */
void ccruncher::Interest::insertRate(Date date, double r)
{
  insertRate(Rate(date, r));
}

/**************************************************************************//**
 * @details Compute rate at day d-date0, where date0 is the curve's date.
 * @param[in] date Date.
 * @return Rate to apply. 0 if d <= date0.
 */
double ccruncher::Interest::getRate(const Date &date) const
{
  if (mDate == NAD || mRates.size() == 0 || date <= mDate) return 0.0;

  if (isDirty) {
    setSpline();
  }

  assert(mSpline != nullptr && mSpline->size > 0);
  int n = mSpline->size - 1;
  double d = date-mDate; // days from curve date
  double r;

  if (mSpline->x[n] <= d) {
    double df = gsl_spline_eval_deriv(mSpline, mSpline->x[n], mAccel);
    r = mSpline->y[n] + df*(d-mSpline->x[n]);
  }
  else if (d <= mSpline->x[0]) {
    double df = gsl_spline_eval_deriv(mSpline, mSpline->x[0], mAccel);
    r = mSpline->y[0] + df*(d-mSpline->x[0]);
  }
  else {
    r = gsl_spline_eval(mSpline, d, mAccel);
  }

  return r;
}

/**************************************************************************//**
 * @details This factor transport a money value from date1 to date0
 *          where date0 is the interest curve date. Factor is computed
 *          according to interest type (simple/compound/continuous)
 * @param[in] date Date.
 * @return Factor to apply.
 */
double ccruncher::Interest::getFactor(const Date &date) const
{
  if (mDate == NAD || mRates.size() == 0 || date <= mDate) return 1.0;

  double r = getRate(date); // anual rate
  double t = (date-mDate)/365.25; // years from curve date

  if (mInterestType == InterestType::Simple) {
    return 1.0/(1.0+r*t);
  }
  else if (mInterestType == InterestType::Compound) {
    return 1.0/pow(1.0+r, t);
  }
  else if (mInterestType == InterestType::Continuous) {
    return 1.0/exp(r*t);
  }
  else {
    assert(false);
    return NAN;
  }
}

/**************************************************************************//**
 * @details Internal method. Assumes that rates are sorted by date.
 *          Create the linear/cubic splines interpolation function.
 * @throw Exception Lack of data to create interpolation function.
 */
void ccruncher::Interest::setSpline() const
{
  gsl_spline_free(mSpline);
  mSpline = nullptr;

  gsl_interp_accel_free(mAccel);
  mAccel = nullptr;

  size_t n = mRates.size();

  if (mSplineType == SplineType::Linear && n < gsl_interp_type_min_size(gsl_interp_linear)) {
    throw Exception("insuficient number of rates to define a linear spline");
  }
  if (mSplineType == SplineType::Cubic && n < gsl_interp_type_min_size(gsl_interp_cspline)) {
    throw Exception("insuficient number of rates to define a cubic spline");
  }

  vector<double> x(n);
  vector<double> y(n);

  for(size_t i=0; i<n; i++)
  {
    // asserting that mRates is sorted
    assert(i>0?(mRates[i-1].date<mRates[i].date):true);
    // checking that rate dates are valid
    if (mRates[i].date < mDate) {
      throw Exception("found a rate date before interest curve date");
    }
    // setting interpolation points
    x[i] = mRates[i].date - mDate; // days from curve date
    y[i] = mRates[i].value;
  }

  if (mSplineType == SplineType::Cubic) {
    mSpline = gsl_spline_alloc(gsl_interp_cspline, n);
  }
  else {
    mSpline = gsl_spline_alloc(gsl_interp_linear, n);
  }

  gsl_spline_init(mSpline, &(x[0]), &(y[0]), n);
  assert(mSpline->size == n);
  mAccel = gsl_interp_accel_alloc();
  isDirty = false;
}

/**************************************************************************//**
 * @details Allowed type: simple, compound, continuous.
 * @throw Exception Type not recognized.
 */
Interest::InterestType ccruncher::Interest::getInterestType(const string &str)
{
  if (str == "simple") return InterestType::Simple;
  else if (str == "compound") return InterestType::Compound;
  else if (str == "continuous") return InterestType::Continuous;
  else throw Exception("interest type '" + str + "' not recognized");
}

/**************************************************************************//**
 * @details Allowed type: linear, cubic.
 * @throw Exception Type not recognized.
 */
Interest::SplineType ccruncher::Interest::getSplineType(const string &str)
{
  if (str == "linear") return SplineType::Linear;
  else if (str == "cubic") return SplineType::Cubic;
  else throw Exception("spline type '" + str + "' not recognized");
}

