
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#ifndef _Interest_
#define _Interest_

#include <string>
#include <vector>
#include <gsl/gsl_spline.h>
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Yield curve.
 *
 * @details This class provides methods to retrieve the curve from the
 *          xml input file, and evaluate it using spline interpolation.
 *
 * @see http://ccruncher.net/ifileref.html#interest
 */
class Interest
{

  public:

    //! Type of interest
    enum class InterestType
    {
      Simple,    //!< Simple interest
      Compound,  //!< Compounded (yearly) interest
      Continuous //!< Continuous interest
    };

    //! Type of interpolation
    enum class SplineType
    {
      Linear, //!< Linear splines
      Cubic   //!< Cubic splines
    };

    //! Data-rate pair
    class Rate
    {
      public:

        //! Rate date
        Date date;
        //! Yearly rate value (0.025 -> 2.5% per year)
        double value;

      public:

        //! Constructor
        Rate(Date d, double r) : date(d), value(r) {}
        //! Less-than operator
        bool operator<(const Rate &o) const { return date < o.date; }
    };

  private:

    //! Interest type
    InterestType mInterestType;
    //! Spline type
    SplineType mSplineType;
    //! Interest curve date
    Date mDate;
    //! Rate values
    std::vector<Rate> mRates;
    //! Spline curve
    mutable gsl_spline *mSpline;
    //! Spline accelerator
    mutable gsl_interp_accel *mAccel;
    //! Need to recompute spline flag
    mutable bool isDirty;

  private:

    //! Create spline curve
    void setSpline() const;

  public:

    //! Constructor
    Interest(const Date &date=NAD, InterestType interestType=InterestType::Compound, SplineType splineType=SplineType::Linear);
    //! Copy constructor
    Interest(const Interest &o);
    //! Destructor
    virtual ~Interest();
    //! Assignment operator
    Interest & operator=(const Interest &);
    //! Set the curve date
    void setDate(const Date &date) { mDate = date; isDirty = true; }
    //! Return the curve date
    const Date & getDate() const { return mDate; }
    //! Set the interest type
    void setInterestType(InterestType interestType) { mInterestType = interestType; }
    //! Set the spline type
    void setSplineType(SplineType splineType) { mSplineType = splineType; isDirty = true; }
    //! Insert user-defined rate at given date
    void insertRate(Date t, double r);
    //! Insert a user-defined rate
    void insertRate(const Rate &rate);
    //! Insert a list of rates
    void insertRates(const std::vector<Rate> &rates);
    //! Returns interpolated rate at date
    double getRate(const Date &date) const;
    //! Returns factor to apply the Net Present Value at the given date
    double getFactor(const Date &date) const;

    //! Parse a interest type
    static InterestType getInterestType(const std::string &str);
    //! Parse a spline type
    static SplineType getSplineType(const std::string &str);
};

} // namespace

#endif

