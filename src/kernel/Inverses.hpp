
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

#ifndef _Inverses_
#define _Inverses_

#include <cmath>
#include <vector>
#include <cassert>
#include <gsl/gsl_spline.h>
#include "params/DefaultProbabilities.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Functions used to simulate the default dates.
 *
 * @details Functions (one per rating) PDinv(t(x)) where:
 *          - t(x) is the cdf of the univariate t-student with ndf degrees
 *            of freedom.
 *          - PD_inv is the inverse of the default probability function of
 *            rating r.
 *          - x is the i-th component of a simulated multivariate t-student
 *            variable.
 *          the evaluation of theses functions is expensive. For this
 *          reason we compose it, and create a spline functions in order to
 *          speed-up the evaluation.
 *
 * @see CCruncher's Technical Document. (section Simulation internals).
 */
class Inverses
{

  private:

    //! Initial date
    Date t0;
    //! Ending date
    Date t1;
    //! Degrees of freedom (gaussian = negative or 0)
    double ndf;
    //! Function per rating
    std::vector<gsl_spline*> splines;

  private:

    //! Inverse cumulative distribution function (gaussian/t-student)
    double icdf(double) const;
    //! Return minimum day
    int getMinDay(int irating, const DefaultProbabilities &dprobs) const;
    //! Set splines
    void setSplines(const DefaultProbabilities &dprobs) throw(Exception);
    //! Set spline
    void setSpline(int irating, std::vector<int> &days, std::vector<double> &cache);
    //! Return the worst unaccurate day
    int getWorstDay(int irating, std::vector<double> &cache);

  public:

    //! Default constructor
    Inverses();
    //! Constructor
    Inverses(double ndf, const Date &maxdate, const DefaultProbabilities &dprobs) throw(Exception);
    //! Copy constructor
    Inverses(const Inverses &);
    //! Destructor
    ~Inverses();
    //! Assignment operator
    Inverses & operator=(const Inverses &);
    //! Initialize
    void init(double ndf, const Date &maxdate, const DefaultProbabilities &dprobs) throw(Exception);
    //! Evalue (return days from t0)
    double evalue(int irating, double val) const;
    //! Evalue (return date)
    Date evalueAsDate(int irating, double val) const;

};

/**************************************************************************//**
 * @details Evalue function at x.
 * @param[in] irating Rating index.
 * @param[in] val Component of the multivariate t-student corresponding
 *            to the obligors factor.
 * @return Simulated default date as days from initial simulation date. If
 *         simulated default date is far from ending simulation date then
 *         this method returns ending simulation 'date + 100 days'.
 */
inline double ccruncher::Inverses::evalue(int irating, double val) const
{
  assert(irating < (int)splines.size());

  gsl_spline *spline = splines[irating];

  if (spline == nullptr)
  {
    // default rating
    return 0.0;
  }

  size_t n = spline->size-1;
  if (spline->x[n] < val)
  {
    // default date bigger than maximum date
    return spline->y[n] + 100.0;
  }
  else if (val <= spline->x[0])
  {
    // default in less than 1 day (or minday)
    return spline->y[0];
  }
  else
  {
    // we don't use accel because values are random
    return gsl_spline_eval(spline, val, nullptr);
  }
}

/**************************************************************************//**
 * @see Inverses::evalue()
 * @param[in] irating Rating index.
 * @param[in] val Component of the multivariate t-student corresponding
 *            to the obligors factor.
 * @return Simulated default date.
 */
inline Date ccruncher::Inverses::evalueAsDate(int irating, double val) const
{
  double days = evalue(irating, val);
  return t0 + (long)ceil(days);
}

} // namespace

#endif

