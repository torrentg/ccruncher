
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
#include <gsl/gsl_cdf.h>
#include "kernel/Inverses.hpp"
#include "utils/Format.hpp"
#include "utils/Timer.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

// maximum error = 1 hour
#define MAX_ERROR 1.0/24.0

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Inverses::Inverses()
{
  ndf = NAN;
  t0 = NAD;
  t1 = NAD;
}

//===========================================================================
// constructor
// if ndf <= 0 then gaussian, t-Student otherwise
//===========================================================================
ccruncher::Inverses::Inverses(double ndf_, const Date &maxdate, const DefaultProbabilities &dprobs) throw(Exception)
{
  init(ndf_, maxdate, dprobs);
}

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::Inverses::Inverses(const Inverses &o)
{
  *this = o;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::Inverses::~Inverses()
{
  for(size_t i=0; i<splines.size(); i++) {
    if (splines[i] != NULL) {
      gsl_spline_free(splines[i]);
    }
  }
}

//===========================================================================
// assignment operator
//===========================================================================
Inverses & ccruncher::Inverses::operator=(const Inverses &o)
{
  t0 = o.t0;
  t1 = o.t1;
  ndf = o.ndf;

  splines = o.splines;
  for(size_t i=0; i<splines.size(); i++)
  {
    if (o.splines[i] != NULL) {
      splines[i] = gsl_spline_alloc(o.splines[i]->interp->type, o.splines[i]->size);
      gsl_spline_init(splines[i], o.splines[i]->x, o.splines[i]->y, o.splines[i]->size);
    }
  }

  return *this;
}

//===========================================================================
// initialize
// if ndf <= 0 then gaussian, t-Student otherwise
//===========================================================================
void ccruncher::Inverses::init(double ndf_, const Date &maxdate, const DefaultProbabilities &dprobs) throw(Exception)
{
  ndf = ndf_;
  t0 = dprobs.getDate();
  t1 = maxdate;

  if (t1 <= t0+1) {
    throw Exception("maxdate out of range");
  }

  setSplines(dprobs);
}

//===========================================================================
// return minimum day
// return the first day bigger than 0 where dprobs(day) > 0
//===========================================================================
int ccruncher::Inverses::getMinDay(int irating, const DefaultProbabilities &dprobs) const
{
  for(int day=1; day<t1-t0; day++)
  {
    if (1e-8 < dprobs.evalue(irating, (double)day))
    {
      return day;
    }
  }
  return t1-t0;
}

//===========================================================================
// inverse cumulative distribution function (gaussian/t-student)
//===========================================================================
double ccruncher::Inverses::icdf(double u) const
{
  assert(0.0 <= u && u <= 1.0);
  if (ndf <= 0.0) return gsl_cdf_ugaussian_Pinv(u);
  else return gsl_cdf_tdist_Pinv(u, ndf);
}

//===========================================================================
// set splines
// try to determine a splines with the minimum number of nodes
// nodes coming from dprobs have precedence over other points
//===========================================================================
void ccruncher::Inverses::setSplines(const DefaultProbabilities &dprobs) throw(Exception)
{
  int nratings = dprobs.getRatings().size();
  splines.assign((size_t)nratings, NULL);

  for(int irating=0; irating<nratings; irating++)
  {
    if (irating == dprobs.getIndexDefault()) {
      continue;
    }

    if (dprobs.getMaxDate(irating) < t1) {
      throw Exception("dprob[" + dprobs.getRatings().getName(irating) + "] not defined at " + t1.toString());
    }

    int minday = getMinDay(irating, dprobs);
    int maxday = t1-t0;

    if (maxday <= minday) {
      // constant function
      splines[irating] = gsl_spline_alloc(gsl_interp_linear, 2);
      splines[irating]->x[0] = icdf(dprobs.evalue(irating, maxday));
      splines[irating]->x[1] = icdf(dprobs.evalue(irating, maxday));
      splines[irating]->y[0] = maxday;
      splines[irating]->y[1] = maxday;
      continue;
    }

    vector<int> nodes = dprobs.getDays(irating);
    while(!nodes.empty() && nodes[0] < minday) nodes.erase(nodes.begin());
    if (nodes.empty() || nodes[0] != minday) nodes.insert(nodes.begin(), minday);
    while(!nodes.empty() && nodes.back() > maxday) nodes.erase(nodes.end()-1);
    if (nodes.empty() || nodes.back() != maxday) nodes.insert(nodes.end(), maxday);
    assert(nodes.size() >= 2);

    vector<int> days(1, minday);
    int dayko = maxday;

    // we create a cache because icdf is very expensive
    vector<double> cache(maxday+1, NAN);
    for(int i=minday; i<=maxday; i++) {
      cache[i] = icdf(dprobs.evalue(irating, (double)i));
    }

    do
    {
      vector<int>::iterator pos1 = lower_bound(days.begin(), days.end(), dayko);
      assert(days.begin() < pos1);
      vector<int>::iterator pos2 = lower_bound(nodes.begin(), nodes.end(), dayko);
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
      setSpline(irating, days, cache);
      dayko = getWorstDay(irating, cache);
    }
    while(dayko > 0 && (int)days.size() < (t1-t0));
    //cout << "splines[" << irating << "].size = " << splines[irating]->size << endl;
  }
}

//===========================================================================
// set spline
//===========================================================================
void ccruncher::Inverses::setSpline(int irating, vector<int> &days, vector<double> &cache)
{
  assert(days.size() >= 2);

  if (splines[irating] != NULL) {
    gsl_spline_free(splines[irating]);
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
    splines[irating] = gsl_spline_alloc(gsl_interp_cspline, x.size());
  }
  else {
    splines[irating] = gsl_spline_alloc(gsl_interp_linear, x.size());
  }
  gsl_spline_init(splines[irating], &(x[0]), &(y[0]), x.size());
}

//===========================================================================
// return the worst unaccurate day
// if all days are accurate, returns 0
//===========================================================================
int ccruncher::Inverses::getWorstDay(int irating, vector<double> &cache)
{
  int ret=0;
  double val=0.0;

  int n = splines[irating]->size - 1;
  assert(splines[irating]->y[0] >= 0);
  // rounded to nearest integer (positive values)
  int d1 = (int)(splines[irating]->y[0] + 0.5);
  assert(splines[irating]->y[n] >= 0);
  // rounded to nearest integer (positive values)
  int d2 = (int)(splines[irating]->y[n] + 0.5);

  for(int i=d1+1; i<d2; i++)
  {
    double x = cache[i];
    double days = evalue(irating, x);
    double err = fabs(days-i);
    if (err > MAX_ERROR && err > val) {
      val = err;
      ret = i;
    }
  }

  return ret;
}

