
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

#include <cmath>
#include <algorithm>
#include "params/Interest.hpp"
#include "utils/Utils.hpp"
#include "utils/Format.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

//===========================================================================
// constructor
//===========================================================================
ccruncher::Interest::Interest(const Date &date_, InterestType type_) :
    spline(NULL), accel(NULL)
{
  type = type_;
  date = date_;
  is_cubic_spline = false;
}

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::Interest::Interest(const Interest &o)
{
  *this = o;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Interest::~Interest()
{
  if (spline != NULL) {
    gsl_spline_free(spline);
  }

  if (accel != NULL) {
    gsl_interp_accel_free(accel);
  }
}

//===========================================================================
// assignment operator
//===========================================================================
Interest & ccruncher::Interest::operator=(const Interest &o)
{
  type = o.type;
  date = o.date;
  rates = o.rates;
  is_cubic_spline = o.is_cubic_spline;

  if (o.spline != NULL) {
    spline = gsl_spline_alloc(o.spline->interp->type, o.spline->size);
    gsl_spline_init(spline, o.spline->x, o.spline->y, o.spline->size);
  }
  else {
    spline = NULL;
  }

  if (o.accel != NULL) {
    accel = gsl_interp_accel_alloc();
    gsl_interp_accel_reset(accel);
  }
  else {
    accel = NULL;
  }

  return *this;
}

//===========================================================================
// set date
//===========================================================================
void ccruncher::Interest::setDate(const Date &d)
{
  if (d == NAD) throw Exception("invalid date");
  date = d;
}

//===========================================================================
// return interest type
//===========================================================================
ccruncher::Interest::InterestType ccruncher::Interest::getType() const
{
  return type;
}

//===========================================================================
// returns the numbers of rates
//===========================================================================
int ccruncher::Interest::size() const
{
  return (int) rates.size();
}

//===========================================================================
// returns interpolated interest rate at day d from date
//===========================================================================
void ccruncher::Interest::getValues(int d, double *t, double *r) const
{
  assert(0 <= d);

  int n = spline->size - 1;

  if (spline->x[n] <= d) {
    double df = gsl_spline_eval_deriv(spline, spline->x[n], accel);
    *r = spline->y[n] + df*(d-spline->x[n]);
    *t = rates[n].y + (double)(d-rates[n].d)/365.0;
  }
  else if (d <= spline->x[0]) {
    double df = gsl_spline_eval_deriv(spline, spline->x[0], accel);
    *r = spline->y[0] + df*(d-spline->x[0]);
    *t = rates[0].y * (double)(d)/(double)(rates[0].d);
  }
  else {
    *r = gsl_spline_eval(spline, d, accel);
    size_t pos = accel->cache;
    *t = rates[pos].y + (double)(d-rates[pos].d)/(double)(rates[pos+1].d-rates[pos].d) * (rates[pos+1].y-rates[pos].y);
  }
}

//===========================================================================
// returns rate at date
//===========================================================================
double ccruncher::Interest::getValue(const Date &d) const
{
  if (date == NAD || rates.size() == 0 || d <= date) return 0.0;

  double r, t;
  getValues(d-date, &t, &r);
  return r;
}

//===========================================================================
// returns factor to aply to transport a money value from date1 to date0
// where date0 is the interest curve date
//===========================================================================
double ccruncher::Interest::getFactor(const Date &d) const
{
  if (date == NAD || rates.size() == 0 || d <= date) return 1.0;

  double r, t;
  getValues(d-date, &t, &r);

  if (type == Simple)
  {
    return 1.0/(1.0+r*t);
  }
  else if (type == Compound)
  {
    return 1.0/pow(1.0+r, t);
  }
  else if (type == Continuous)
  {
    return 1.0/exp(r*t);
  }
  else
  {
    assert(false);
    return NAN;
  }
}

//===========================================================================
// insertRate
//===========================================================================
void ccruncher::Interest::insertRate(const Rate &val) throw(Exception)
{
  assert(date != NAD);

  if (date == NAD) throw Exception("interest curve without date");

  if (val.d < 0)
  {
    throw Exception("rate with invalid time");
  }

  // checking if previously defined
  for (size_t i=0; i<rates.size(); i++)
  {
    Rate aux = rates[i];

    if (abs(aux.d-val.d) == 0)
    {
      throw Exception("rate time '" + val.t_str + "' repeated");
    }
  }

  // inserting value
  try
  {
    rates.push_back(val);
  }
  catch(std::exception &e)
  {
    throw Exception(e);
  }
}

//===========================================================================
// setSpline
// assumed than rates are sorted by date
//===========================================================================
void ccruncher::Interest::setSpline() throw(Exception)
{
  assert(spline == NULL && accel == NULL);
  size_t n = rates.size();

  if (!is_cubic_spline && n < gsl_interp_type_min_size(gsl_interp_linear)) {
    throw Exception("insuficient number of rates to define a linear spline");
  }
  if (is_cubic_spline && n < gsl_interp_type_min_size(gsl_interp_cspline)) {
    throw Exception("insuficient number of rates to define a cubic spline");
  }

  vector<double> x(n);
  vector<double> y(n);

  for(size_t i=0; i<n; i++)
  {
    x[i] = rates[i].d;
    y[i] = rates[i].r;
  }

  if (is_cubic_spline) {
    spline = gsl_spline_alloc(gsl_interp_cspline, n);
  }
  else {
    spline = gsl_spline_alloc(gsl_interp_linear, n);
  }

  gsl_spline_init(spline, &(x[0]), &(y[0]), n);
  assert(spline->size == n);
  accel = gsl_interp_accel_alloc();
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interest::epstart(ExpatUserData &, const char *name_, const char **attributes)
{
  if (isEqual(name_,"interest"))
  {
    if (getNumAttributes(attributes) == 0) {
      type = Compound;
    }
    else if (getNumAttributes(attributes) <= 2)
    {
      string str = Utils::trim(getStringAttribute(attributes, "type"));
      str = Utils::lowercase(str);
      if (str == "simple") {
        type = Simple;
      }
      else if (str == "compound") {
        type = Compound;
      }
      else if (str == "continuous") {
        type = Continuous;
      }
      else  {
        throw Exception("unrecognized interest type");
      }

      str = getStringAttribute(attributes, "spline", "linear");
      if (str == "cubic") is_cubic_spline = true;
      else if (str == "linear") is_cubic_spline = false;
      else {
        throw Exception("unrecognized spline type");
      }
    }
    else {
      throw Exception("incorrect number of attributes");
    }
  }
  else if (isEqual(name_,"rate"))
  {
    if (getNumAttributes(attributes) != 2) {
      throw Exception("incorrect number of attributes");
    }

    Date t(date);
    const char *str = getAttributeValue(attributes, "t");
    if (isInterval(str)) {
      t.add(str);
    }
    else {
      t = Date(str);
    }
    int d = t - date;

    double r = getDoubleAttribute(attributes, "r");
    if (r < -0.5 || 1.0 < r)
    {
      throw Exception("rate value " + Format::toString(r) + " out of range [-0.5, +1.0]");
    }

    insertRate(Rate(d, diff(date, t, 'Y'), r, str));
  }
  else
  {
    throw Exception("unexpected tag '" + string(name_) + "'");
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Interest::epend(ExpatUserData &, const char *name_)
{
  if (isEqual(name_,"interest")) {
    if (rates.empty()) {
      throw Exception("interest has no rates");
    }
    sort(rates.begin(), rates.end());
    setSpline();
  }
}

