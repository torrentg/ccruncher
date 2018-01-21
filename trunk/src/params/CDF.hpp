
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

#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <gsl/gsl_spline.h>

namespace ccruncher {

/**************************************************************************//**
 * @brief User-defined CDF (Cumulative Distribution Function).
 *
 * @details This class allows to evaluate the cdf function interpolating
 *          the user-defined points using linear or cubic splines.
 *
 * @see http://ccruncher.net/ifileref.html#dprobs
 */
class CDF
{

  private:

    //! Minimum x value
    double mXmin = -INFINITY;
    //! Maximum x value
    double mXmax = +INFINITY;
    //! Values provided by user
    std::vector<std::pair<double,double>> mData;
    //! Spline curve
    mutable gsl_spline *mSpline = nullptr;
    //! Spline accelerator
    mutable gsl_interp_accel *mAccel = nullptr;

  private:

    //! Set spline
    void setSpline() const;
    //! Remove spline (free memory)
    void freeSpline();

  public:

    //! Default constructor
    CDF(double xmin=-INFINITY, double xmax=+INFINITY);
    //! Copy constructor
    CDF(const CDF &);
    //! Constructor
    CDF(const std::vector<std::pair<double,double>> &,
        double xmin=-INFINITY, double xmax=+INFINITY);
    //! Constructor
    CDF(const std::vector<double> &, const std::vector<double> &,
        double xmin=-INFINITY, double xmax=+INFINITY);
    //! Destructor
    ~CDF();
    //! Assignment operator
    CDF & operator=(const CDF &);
    //! Returns the list of PDs provided by user
    const std::vector<std::pair<double,double>>& getPoints() const { return mData; }
    //! Set x range
    void setRange(double xmin, double xmax);
    //! Add a cdf point
    void add(double x, double prob);
    //! Cdf value at time t (in days from starting date)
    double evalue(double x) const;
    //! Returns interpolation type (l=linear, c=cubic, n=none)
    std::string getInterpolationType() const;

};

} // namespace
