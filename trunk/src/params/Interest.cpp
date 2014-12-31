
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
#include "params/Interest.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] date Date on which the curve is defined.
 * @param[in] type Type of interest to apply.
 */
ccruncher::Interest::Interest(const Date &date, Type type) :
    mSpline(nullptr), mAccel(nullptr)
{
  mType = type;
  mDate = date;
  mCubicSpline = false;
}

/**************************************************************************//**
 * @see Interest::operator=
 * @param[in] o Object to copy.
 */
ccruncher::Interest::Interest(const Interest &o)
{
  *this = o;
}

/**************************************************************************/
ccruncher::Interest::~Interest()
{
  if (mSpline != nullptr) {
    gsl_spline_free(mSpline);
  }

  if (mAccel != nullptr) {
    gsl_interp_accel_free(mAccel);
  }
}

/**************************************************************************//**
 * @details Assign the given object reallocating its own objects.
 * @param[in] o Object to assign.
 */
Interest & ccruncher::Interest::operator=(const Interest &o)
{
  mType = o.mType;
  mDate = o.mDate;
  mRates = o.mRates;
  mCubicSpline = o.mCubicSpline;

  if (mSpline != nullptr) {
    gsl_spline_free(mSpline);
  }

  if (mAccel != nullptr) {
    gsl_interp_accel_free(mAccel);
  }

  if (o.mSpline != nullptr) {
    mSpline = gsl_spline_alloc(o.mSpline->interp->type, o.mSpline->size);
    gsl_spline_init(mSpline, o.mSpline->x, o.mSpline->y, o.mSpline->size);
  }
  else {
    mSpline = nullptr;
  }

  if (o.mAccel != nullptr) {
    mAccel = gsl_interp_accel_alloc();
    gsl_interp_accel_reset(mAccel);
  }
  else {
    mAccel = nullptr;
  }

  return *this;
}

/**************************************************************************//**
 * @param[in] date Initial date.
 * @throw Exception Invalid date.
 */
void ccruncher::Interest::setDate(const Date &date)
{
  if (date == NAD) throw Exception("invalid date");
  mDate = date;
}

/**************************************************************************//**
 * @return The interest type.
 */
ccruncher::Interest::Type ccruncher::Interest::getType() const
{
  return mType;
}

/**************************************************************************//**
 * @return Number of user-defined rates.
 */
int ccruncher::Interest::size() const
{
  return (int) mRates.size();
}

/**************************************************************************//**
 * @details Returns interpolated interest rate at d-th day from initial date.
 * @param[in] d Day from initial date.
 * @param[out] t Time in years.
 * @param[out] r Rate to apply.
 */
void ccruncher::Interest::getValues(int d, double *t, double *r) const
{
  assert(0 <= d);

  int n = mSpline->size - 1;

  if (mSpline->x[n] <= d) {
    double df = gsl_spline_eval_deriv(mSpline, mSpline->x[n], mAccel);
    *r = mSpline->y[n] + df*(d-mSpline->x[n]);
    *t = mRates[n].y + (double)(d-mRates[n].d)/365.0;
  }
  else if (d <= mSpline->x[0]) {
    double df = gsl_spline_eval_deriv(mSpline, mSpline->x[0], mAccel);
    *r = mSpline->y[0] + df*(d-mSpline->x[0]);
    *t = mRates[0].y * (double)(d)/(double)(mRates[0].d);
  }
  else {
    *r = gsl_spline_eval(mSpline, d, mAccel);
    size_t pos = mAccel->cache;
    *t = mRates[pos].y + (double)(d-mRates[pos].d)/(double)(mRates[pos+1].d-mRates[pos].d) * (mRates[pos+1].y-mRates[pos].y);
  }
}

/**************************************************************************//**
 * @details Compute rate at day d-date0, where date0 is the curve's date.
 * @param[in] date Date.
 * @return Rate to apply. 0 if d <= date0.
 */
double ccruncher::Interest::getValue(const Date &date) const
{
  if (mDate == NAD || mRates.size() == 0 || date <= mDate) return 0.0;

  double r, t;
  getValues(date-mDate, &t, &r);
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

  double r, t;
  getValues(date-mDate, &t, &r);

  if (mType == Type::Simple)
  {
    return 1.0/(1.0+r*t);
  }
  else if (mType == Type::Compound)
  {
    return 1.0/pow(1.0+r, t);
  }
  else if (mType == Type::Continuous)
  {
    return 1.0/exp(r*t);
  }
  else
  {
    assert(false);
    return NAN;
  }
}

/**************************************************************************//**
 * @details Previous to insertion check that it is a valid rate,
 *          non-repeated time, etc.
 * @param[in] val User-defined rate to insert.
 * @throw Exception Error validating rate.
 */
void ccruncher::Interest::insertRate(const Rate &val)
{
  assert(mDate != NAD);

  if (mDate == NAD) throw Exception("interest curve without date");

  if (val.d < 0)
  {
    throw Exception("rate with invalid time");
  }

  // checking if previously defined
  for(Rate &rate : mRates)
  {
    if (abs(rate.d-val.d) == 0)
    {
      throw Exception("rate time repeated");
    }
  }

  // checking interest rate value
  if (val.r < -0.5 || 1.0 < val.r)
  {
    throw Exception("rate value '" + to_string(val.r) + "' out of range [-0.5, +1.0]");
  }

  // inserting value
  try
  {
    mRates.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

/**************************************************************************//**
 * @details Internal method. Assume than rates are sorted by date.
 *          Create the interpolation function using the user preference
 *          (linear/cubic splines).
 * @throw Exception Lack of data to create interpolation function.
 */
void ccruncher::Interest::setSpline()
{
  assert(mSpline == nullptr && mAccel == nullptr);
  size_t n = mRates.size();

  if (!mCubicSpline && n < gsl_interp_type_min_size(gsl_interp_linear)) {
    throw Exception("insuficient number of rates to define a linear spline");
  }
  if (mCubicSpline && n < gsl_interp_type_min_size(gsl_interp_cspline)) {
    throw Exception("insuficient number of rates to define a cubic spline");
  }

  vector<double> x(n);
  vector<double> y(n);

  for(size_t i=0; i<n; i++)
  {
    assert(i>0?(mRates[i-1].d<mRates[i].d):true);
    x[i] = mRates[i].d;
    y[i] = mRates[i].r;
  }

  if (mCubicSpline) {
    mSpline = gsl_spline_alloc(gsl_interp_cspline, n);
  }
  else {
    mSpline = gsl_spline_alloc(gsl_interp_linear, n);
  }

  gsl_spline_init(mSpline, &(x[0]), &(y[0]), n);
  assert(mSpline->size == n);
  mAccel = gsl_interp_accel_alloc();
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Interest::epstart(ExpatUserData &, const char *tag, const char **attributes)
{
  if (isEqual(tag,"interest"))
  {
    if (getNumAttributes(attributes) == 0) {
      mType = Type::Compound;
    }
    else if (getNumAttributes(attributes) <= 2)
    {
      string str = getStringAttribute(attributes, "type");
      if (str == "simple") {
        mType = Type::Simple;
      }
      else if (str == "compound") {
        mType = Type::Compound;
      }
      else if (str == "continuous") {
        mType = Type::Continuous;
      }
      else  {
        throw Exception("unrecognized interest type");
      }

      str = getStringAttribute(attributes, "spline", "linear");
      if (str == "cubic") mCubicSpline = true;
      else if (str == "linear") mCubicSpline = false;
      else {
        throw Exception("unrecognized spline type");
      }
    }
    else {
      throw Exception("incorrect number of attributes");
    }
  }
  else if (isEqual(tag,"rate"))
  {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("incorrect number of attributes");
    }

    Date t(mDate);
    const char *str = getAttributeValue(attributes, "t");
    if (isInterval(str)) {
      t.add(str);
    }
    else {
      t = Date(str);
    }
    int d = t - mDate;

    double r = getDoubleAttribute(attributes, "r");

    insertRate(Rate(d, diff(mDate, t, 'Y'), r));
  }
  else
  {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 */
void ccruncher::Interest::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"interest")) {
    if (mRates.empty()) {
      throw Exception("interest has no rates");
    }
    sort(mRates.begin(), mRates.end(),
         [](const Rate &a, const Rate &b) -> bool {
           return a.d < b.d;
         });
    setSpline();
  }
}

