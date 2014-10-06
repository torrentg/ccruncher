
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

#ifndef _Inverse_
#define _Inverse_

#include <cmath>
#include <vector>
#include <cassert>
#include <gsl/gsl_spline.h>
#include "params/CDF.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Function used to simulate the default times.
 *
 * @details Function PDinv(t(x)) where:
 *          - t(x) is the cdf of the univariate t-student with ndf degrees
 *            of freedom.
 *          - PD_inv is the inverse of the default probability function of
 *            rating r.
 *          - x is the i-th component of a simulated multivariate t-student
 *            variable.
 *          the evaluation of this function is expensive. For this
 *          reason we compose it, and create a spline function in order to
 *          speed-up the evaluation.
 *
 * @see CCruncher's Technical Document. (section Simulation internals).
 */
class Inverse
{

  private:

    //! Ending date (in days from starting date)
    double mMaxT;
    //! Degrees of freedom (gaussian = negative or 0)
    double mNdf;
    //! Spline function
    gsl_spline *mSpline;

  private:

    //! Inverse cumulative distribution function (gaussian/t-student)
    double icdf(double) const;
    //! Return minimum day
    int getMinDay(const CDF &cdf) const;
    //! Set splines
    void setSpline(const CDF &cdf);
    //! Set spline
    void setSpline(std::vector<int> &days, std::vector<double> &cache);
    //! Return the worst unaccurate day
    int getWorstDay(std::vector<double> &cache);

  public:

    //! Default constructor
    Inverse();
    //! Constructor
    Inverse(double ndf, double maxt, const CDF &cdf);
    //! Copy constructor
    Inverse(const Inverse &);
    //! Destructor
    ~Inverse();
    //! Assignment operator
    Inverse & operator=(const Inverse &);
    //! Initialize
    void init(double ndf, double maxt, const CDF &cdf);
    //! Evalue (return days from t0)
    double evalue(double val) const;
    //! Returns interpolation type (l=linear, c=cubic, n=none)
    std::string getInterpolationType() const;

};

/**************************************************************************//**
 * @details Evalue function at x.
 * @param[in] val Component of the multivariate t-student corresponding
 *            to the obligors factor.
 * @return Simulated default date in days from initial simulation date. If
 *         simulated default date is far from ending simulation date then
 *         this method returns ending simulation 'date + 100 days'.
 */
inline double ccruncher::Inverse::evalue(double val) const
{
  if (mSpline == nullptr)
  {
    // default rating
    return 0.0;
  }

  size_t n = mSpline->size-1;
  if (mSpline->x[n] < val)
  {
    // default date bigger than maximum date
    return mSpline->y[n] + 100.0;
  }
  else if (val <= mSpline->x[0])
  {
    // default in less than 1 day (or minday)
    return mSpline->y[0];
  }
  else
  {
    // we don't use accel because values are random
    return gsl_spline_eval(mSpline, val, nullptr);
  }
}

} // namespace

#endif

