
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
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//===========================================================================

#include <cmath>
#include <climits>
#include <algorithm>
#include <cassert>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>
#include "params/DefaultProbabilities.hpp"
#include "utils/Format.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-12
#define MAX_ITER_BISECTION 100
#define MAX_ITER_NEWTON 40
// absolute error less than 1 sec (1/(24*60*60)=1.16e-5)
#define ABS_ERR_ROOT 1e-7

/**************************************************************************/
ccruncher::DefaultProbabilities::DefaultProbabilities()
{
  indexdefault = -1;
  date = NAD;
}

/**************************************************************************//**
 * @param[in] o Object to copy.
 * @see DefaultProbabilities::operator=
 */
ccruncher::DefaultProbabilities::DefaultProbabilities(const DefaultProbabilities &o)
{
  *this = o;
}

/**************************************************************************//**
 * @param[in] ratings_ List of ratings.
 * @param[in] d Initial date.
 * @throw Exception Invalid rating list.
 */
ccruncher::DefaultProbabilities::DefaultProbabilities(const Ratings &ratings_,
           const Date &d)
{
  indexdefault = -1;
  date = d;
  setRatings(ratings_);
}

/**************************************************************************//**
 * @details This constructor is used to create the default probabilities
 *          from a transition matrix.
 * @see Class Transitions
 * @param[in] ratings_ List of ratings.
 * @param[in] d Initial date.
 * @param[in] dates List of dates where PD values are defined.
 * @param[in] values List of PD values per rating.
 * @throw Exception Invalid data.
 */
ccruncher::DefaultProbabilities::DefaultProbabilities(const Ratings &ratings_,
           const Date &d, const std::vector<Date> &dates,
           const std::vector<std::vector<double>> &values)
{
  indexdefault = -1;
  date = d;
  setRatings(ratings_);

  // adding values
  for(int i=0; i<ratings.size(); i++)
  {
    for(unsigned int j=0; j<dates.size(); j++)
    {
      insertValue(ratings.getName(i), dates[j], values[i][j]);
    }
  }

  validate();
  setSplines();
}

/**************************************************************************/
ccruncher::DefaultProbabilities::~DefaultProbabilities()
{
  reset();
}

/**************************************************************************//**
 * @details Free allocated memory and set internal variables to its
 *          default value.
 */
void ccruncher::DefaultProbabilities::reset()
{
  ratings = Ratings();
  indexdefault = -1;
  date = NAD;

  for(gsl_spline *spline : splines) {
    if (spline != nullptr) {
      gsl_spline_free(spline);
    }
  }
  splines.clear();

  for(gsl_interp_accel *accel : accels) {
    if (accel != nullptr) {
      gsl_interp_accel_free(accel);
    }
  }
  accels.clear();

  ddata.clear();
}

/**************************************************************************//**
 * @details Assign the given object reallocating its own objects.
 * @param[in] o Object to assign.
 */
DefaultProbabilities & ccruncher::DefaultProbabilities::operator=(const DefaultProbabilities &o)
{
  reset();

  date = o.date;
  ddata = o.ddata;
  ratings = o.ratings;
  indexdefault = o.indexdefault;

  splines = o.splines;
  for(size_t i=0; i<splines.size(); i++)
  {
    if (o.splines[i] != nullptr) {
      splines[i] = gsl_spline_alloc(o.splines[i]->interp->type, o.splines[i]->size);
      gsl_spline_init(splines[i], o.splines[i]->x, o.splines[i]->y, o.splines[i]->size);
    }
  }

  accels = o.accels;
  for(size_t i=0; i<accels.size(); i++)
  {
    if (o.accels[i] != nullptr) {
      accels[i] = gsl_interp_accel_alloc();
      gsl_interp_accel_reset(accels[i]);
    }
  }

  return *this;
}

/**************************************************************************//**
 * @param[in] ratings_ List of ratings.
 * @throw Exception Invalid ratings.
 */
void ccruncher::DefaultProbabilities::setRatings(const Ratings &ratings_)
{
  if (ratings_.size() <= 0)
  {
    throw Exception("ratings not defined");
  }
  else
  {
    ratings = ratings_;
    ddata = vector<vector<pd>>(ratings.size());
  }
}

/**************************************************************************//**
 * @return The list of ratings.
 */
const Ratings & ccruncher::DefaultProbabilities::getRatings() const
{
  return ratings;
}

/**************************************************************************//**
 * @param[in] d Initial date.
 */
void ccruncher::DefaultProbabilities::setDate(const Date &d)
{
  date = d;
}

/**************************************************************************//**
 * @return Initial date.
 */
Date ccruncher::DefaultProbabilities::getDate() const
{
  return date;
}

/**************************************************************************//**
 * @return Index (0-based) of the 'default' rating.
 */
int ccruncher::DefaultProbabilities::getIndexDefault() const
{
  return indexdefault;
}

/**************************************************************************//**
 * @details The number of defined default probabilities function matches
 *          the number of ratings.
 * @return Number of DP functions.
 */
int ccruncher::DefaultProbabilities::size() const
{
  return ratings.size();
}

/**************************************************************************//**
 * @details Returned values are:
 *          - "none": default rating.
 *          - "linear": linear interpolation.
 *          - "cspline": cubic spline interpolation.
 * @return Type of interpolation used in the i-th default probability function.
 */
string ccruncher::DefaultProbabilities::getInterpolationType(int i) const
{
  assert(!splines.empty());
  assert(i < (int)splines.size());
  if (i == indexdefault) {
    return "none";
  }
  else {
    assert(splines[i] != nullptr);
    return gsl_interp_name(splines[i]->interp);
  }
}

/**************************************************************************//**
 * @details Inserts a PD(t;r) user defined value.
 * @param[in] srating Rating identifier.
 * @param[in] t Date.
 * @param[in] value Default probability at time t for rating r.
 * @throw Exception Error inserting element.
 */
void ccruncher::DefaultProbabilities::insertValue(const std::string &srating,
                const Date &t, double value)
{
  assert(ratings.size() > 0);
  assert(date != NAD);

  int irating = ratings.getIndex(srating);

  // checking rating index
  if (irating < 0)
  {
    throw Exception("unknow rating at dprob[" + srating + "," + Format::toString(t) + "]");
  }

  // validating time
  if (t < date)
  {
    string msg = "dprob[" + srating + "," + Format::toString(t) +
                 "] has date previous to initial time";
    throw Exception(msg);
  }

  // validating value
  if (value < 0.0 || 1.0 < value)
  {
    string msg = "dprob[" + srating + "," + Format::toString(t) +
                 "] out of range: " + Format::toString(value);
    throw Exception(msg);
  }

  int day = t-date;

  // checking that is not previously defined
  for(size_t i=0; i<ddata[irating].size(); i++) {
    if (ddata[irating][i].day == day) {
      string msg = "dprob[" + srating + "," + Format::toString(t) + "] redefined";
      throw Exception(msg);
    }
  }

  // inserting value
  ddata[irating].push_back(pd(day,value));
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] name Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::DefaultProbabilities::epstart(ExpatUserData &, const char *name, const char **attributes)
{
  if (isEqual(name,"dprobs")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("attributes not allowed in tag 'dprobs'");
    }
  }
  else if (isEqual(name,"dprob")) {
    string srating = getStringAttribute(attributes, "rating");
    Date t(date);
    const char *str = getAttributeValue(attributes, "t");
    if (isInterval(str)) {
      t.add(str);
    }
    else {
      t = Date(str);
    }
    double value = getDoubleAttribute(attributes, "value");
    insertValue(srating, t, value);
  }
  else {
    throw Exception("unexpected tag '" + string(name) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] name Element name.
 */
void ccruncher::DefaultProbabilities::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"dprobs")) {
    validate();
    setSplines();
  }
}

/**************************************************************************//**
 * @details Internal method. Sorts data, identifies default rating, fills
 *          info not entered by user, and checks data.
 * @throw Exception Validation error.
 */
void ccruncher::DefaultProbabilities::validate()
{
  // ranges are checked in insertValue() method. don't rechecked here

  // sorting values
  for(size_t i=0; i<ddata.size(); i++) {
    sort(ddata[i].begin(), ddata[i].end());
  }

  // finding default rating
  indexdefault = -1;
  for(int i=0; i<ratings.size(); i++)
  {
    if (ddata[i].size() == 0 || (ddata[i][0].day == 0 && ddata[i][0].prob > 1.0-EPSILON)) {
      indexdefault = i;
      break;
    }
  }
  if (indexdefault < 0) {
    throw Exception("default rating not found");
  }

  // checking that all ratings (except default) have pd function defined
  for(int i=0; i<ratings.size(); i++)
  {
    if (i == indexdefault) continue;
    if(ddata[i].size() == 0 || (ddata[i].size() == 1 && ddata[i][0].day == 0)) {
      string msg = "rating " + ratings.getName(i) + " without 'dprob' tag";
      throw Exception(msg);
    }
  }

  // checking dprob[irating][0]=0 if rating != default
  for(int i=0; i<ratings.size(); i++)
  {
    if (i != indexdefault && ddata[i][0].day == 0 && ddata[i][0].prob > EPSILON)
    {
      string msg = "dprob[" + ratings.getName(i) + ",0] > 0";
      throw Exception(msg);
    }
  }

  // setting dprob[irating][0]=0 if not set
  for(int i=0; i<ratings.size(); i++)
  {
    if (i != indexdefault && ddata[i][0].day > 0)
    {
      ddata[i].insert(ddata[i].begin(), pd(0,0.0));
    }
  }

  // checking that dprob[default][t] = 1 for all t's
  for(size_t j=0; j<ddata[indexdefault].size(); j++)
  {
    if (ddata[indexdefault][j].prob < 1.0-EPSILON)
    {
      throw Exception("default rating have a dprob less than 1");
    }
  }

  // checking monotonic increasing
  for(int i=0; i<ratings.size(); i++)
  {
    if (i == indexdefault) continue;

    for(size_t j=1; j<ddata[i].size(); j++)
    {
      if (ddata[i][j].prob < ddata[i][j-1].prob)
      {
        string msg = "dprob[" + ratings.getName(i) +
                     "] is not monotonically increasing at t=" +
                     Format::toString(ddata[i][j].day) + "D";
        throw Exception(msg);
      }
    }
  }
}

/**************************************************************************//**
 * @details Internal method. For each rating default probability data
 *          creates a spline function. Determines linear/cubic splines
 *          flowing theses rules:
 *          - default rating -> none
 *          - less than 3 values -> linear
 *          - There are negative derivatives in the cubic spline -> linear
 *          - otherwise -> cubic
 *          Although data values are increasing, the cubic spline can be
 *          a non-increasing in the time range. This is the cause to use
 *          linear splines in this case.
 */
void ccruncher::DefaultProbabilities::setSplines()
{
  assert(indexdefault >= 0);
  assert(ddata.size() > 0);

  splines.assign(ddata.size(), nullptr);
  accels.assign(ddata.size(), nullptr);

  for(size_t i=0; i<ddata.size(); i++)
  {
    if (i == (size_t) indexdefault) continue;

    vector<double> x;
    vector<double> y;

    for(size_t j=0; j<ddata[i].size(); j++)
    {
      if (ddata[i][j].prob < 0.0)
      {
        assert(false);
      }
      else if (ddata[i][j].prob == 0.0)
      {
        // because a dprob can have dprob[irating][0:4] = 0
        // in these cases, cubic spline can have negative derivatives
        // if we use all the points. For this reason we remove the
        // starting values with prob=0
        if (j+1 == ddata[i].size() || ddata[i][j+1].prob != 0)
        {
          x.push_back(ddata[i][j].day);
          y.push_back(ddata[i][j].prob);
        }
      }
      else if (ddata[i][j].prob <= 1.0)
      {
        x.push_back(ddata[i][j].day);
        y.push_back(ddata[i][j].prob);
      }
      else
      {
        assert(false);
      }
    }

    if (y.size() == 1 && y[0] == 0.0) {
      x.insert(x.begin(), 0.0);
      y.insert(y.begin(), 0.0);
    }

    size_t n = x.size();
    assert(n >= 2);

    bool iscspline = false;

    if (gsl_interp_type_min_size(gsl_interp_cspline) <= n)
    {
      splines[i] = gsl_spline_alloc(gsl_interp_cspline, n);
      gsl_spline_init(splines[i], &(x[0]), &(y[0]), n);
      iscspline = true;

      for(size_t j=0; j<n; j++)
      {
        double deriv = gsl_spline_eval_deriv(splines[i], x[j], nullptr);
        if (deriv < 0.0) {
          gsl_spline_free(splines[i]);
          splines[i] = nullptr;
          iscspline = false;
          break;
        }
      }
    }

    if (!iscspline)
    {
      splines[i] = gsl_spline_alloc(gsl_interp_linear, n);
      gsl_spline_init(splines[i], &(x[0]), &(y[0]), n);
    }

    assert(splines[i]->size == n);
    accels[i] = gsl_interp_accel_alloc();
  }
}

/**************************************************************************//**
 * @param[in] irating Index (0-based) of rating.
 * @param[in] t Time in days from starting date.
 * @return Probability of default in [0,1].
 */
double ccruncher::DefaultProbabilities::evalue(int irating, double t) const
{
  assert(!splines.empty() && !accels.empty());
  assert(splines.size() == accels.size());
  assert(0 <= irating && irating < ratings.size());

  if (irating == indexdefault) {
    return 1.0;
  }

  if (t < 0) {
    assert(false);
    return 0.0;
  }

  assert(splines[irating] != nullptr);
  int n = splines[irating]->size - 1;

  if (splines[irating]->x[n] <= t) {
    return splines[irating]->y[n];
  }
  else if (t <= splines[irating]->x[0]) {
    return splines[irating]->y[0];
  }
  else {
    double ret = gsl_spline_eval(splines[irating], t, accels[irating]);
    assert(0.0 <= ret && ret <= 1.0);
    return ret;
  }
}

/**************************************************************************//**
 * @param[in] irating Index (0-based) of rating.
 * @param[in] d Date to determine time (t = d - startg date).
 * @return Probability of default in [0,1].
 */
double ccruncher::DefaultProbabilities::evalue(int irating, const Date &d) const
{
  return evalue(irating, d-date);
}

/**************************************************************************//**
 * @details Evalue inv_pd(prob;r), where prob is the probability. Returns
 *          the time in days from starting date. Default probability
 *          are invertible because they are strictly increasing functions.
 *          Inverse value is obtained by root-finding methods.
 * @note    Inverse of a spline isn't a spline.
 * @note    inv(spline(x,y)) != spline(y,x)
 * @note    This is not a high-performance method
 * @param[in] irating Index (0-based) of rating.
 * @param[in] prob Probability, value in range [0,1].
 * @return Days from starting date.
 */
double ccruncher::DefaultProbabilities::inverse(int irating, double prob) const
{
  assert(!splines.empty() && !accels.empty());
  assert(splines.size() == accels.size());
  assert(0 <= irating && irating < ratings.size());

  if (irating == indexdefault) {
    return 0.0;
  }

  if (prob < 0.0 || prob > 1.0) {
    assert(false);
    return 0.0;
  }

  assert(splines[irating] != nullptr && splines[irating]->size > 0);

  if (ddata[irating].back().prob < prob) {
    return ddata[irating].back().day + 1.0;
  }
  else {
    if (splines[irating]->interp->type == gsl_interp_cspline) {
      return inverse_cspline(splines[irating], prob, accels[irating]);
    }
    else if (splines[irating]->interp->type == gsl_interp_linear) {
      return inverse_linear(splines[irating], prob, accels[irating]);
    }
    else {
      assert(false);
      return NAN;
    }
  }
}

/**************************************************************************//**
 * @details Evaluate PD(x;r) taking care in the extremes.
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] x Time from starting date.
 * @param[in] params Root-finding function parameters (includes splines to use).
 * @return PD evaluated at time x.
 */
double ccruncher::DefaultProbabilities::f(double x, void *params)
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
 * @details Evaluate the derivative of PD(x;r) taking care in the extremes.
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] x Time from starting date.
 * @param[in] params Root-finding function parameters (includes splines to use).
 * @return PD evaluated at time x.
 */
double ccruncher::DefaultProbabilities::df(double x, void *params)
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
 * @details Evaluate PD(x;r) and its derievative simultaneously.
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] x Time from starting date.
 * @param[in] params Root-finding function parameters (includes splines to use).
 * @param[out] y Function PD(,r) evaluated at time x.
 * @param[out] dy Derivative of PD(;r) at time x
 */
void ccruncher::DefaultProbabilities::fdf (double x, void *params, double *y, double *dy)
{
  *y = f(x, params);
  *dy = df(x, params);
}

/**************************************************************************//**
 * @see http://www.gnu.org/software/gsl/manual/html_node/One-dimensional-Root_002dFinding.html
 * @param[in] spline Default probability function spline to use.
 * @param[in] y Probability in [0,1].
 * @param[in,out] accel Spline accelerator.
 * @return Value x satisfying PD(x,r)=y
 */
double ccruncher::DefaultProbabilities::inverse_linear(gsl_spline *spline, double y,
                  gsl_interp_accel *accel) const
{
  assert(spline != nullptr);
  assert(accel != nullptr);

  int status;
  int iter = 0;
  double root = 0;
  double x_lo = spline->x[0];
  double x_hi = spline->x[spline->size-1];
  gsl_function F;
  fparams params = {spline, accel, y};

  F.function = &ccruncher::DefaultProbabilities::f;
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
 * @param[in] spline Default probability function spline to use.
 * @param[in] y Probability in [0,1].
 * @param[in,out] accel Spline accelerator.
 * @return Value x satisfying PD(x,r)=y
 */
double ccruncher::DefaultProbabilities::inverse_cspline(gsl_spline *spline, double y,
                  gsl_interp_accel *accel) const
{
  assert(spline != nullptr);
  assert(accel != nullptr);

  int status;
  int iter = 0;
  double root = (spline->x[spline->size-1]+spline->x[0])/2.0;
  double x0 = 0.0;
  gsl_function_fdf FDF;
  fparams params = {spline, accel, y};

  FDF.f = &ccruncher::DefaultProbabilities::f;
  FDF.df = &ccruncher::DefaultProbabilities::df;
  FDF.fdf = &ccruncher::DefaultProbabilities::fdf;
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
    return inverse_linear(spline, y, accel);
  }
}

/**************************************************************************//**
 * @param[in] irating Index (0-based) of rating.
 * @return Maximum user defined date.
 */
Date ccruncher::DefaultProbabilities::getMaxDate(int irating) const
{
  assert(date != NAD);
  assert(0 <= irating && irating < ratings.size());
  if (irating < 0 || ratings.size() <= irating) {
    return NAD;
  }
  else {
    return date + ddata[irating].back().day;
  }
}

/**************************************************************************//**
 * @param[in] irating Index (0-based) of rating.
 * @return List of times (in days from starting date) where PD(t;r) is
 *         user defined.
 */
vector<int> ccruncher::DefaultProbabilities::getDays(int irating) const
{
  assert(!splines.empty());
  if (splines[irating] == nullptr) return vector<int>();
  vector<int> ret(splines[irating]->size, 0);
  for(size_t i=0; i<splines[irating]->size; i++) {
    assert(splines[irating]->x[i] >= 0);
    // rounded to nearest integer (positive values)
    ret[i] = (int)(splines[irating]->x[i]+0.5);
  }
  return ret;
}

