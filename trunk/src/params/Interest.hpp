
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

#ifndef _Interest_
#define _Interest_

#include <string>
#include <vector>
#include <gsl/gsl_spline.h>
#include "utils/ExpatHandlers.hpp"
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
class Interest : public ExpatHandlers
{

  public:

    //! Type of interest
    enum class Type
    {
      Simple,    //!< Simple interest
      Compound,  //!< Compounded (yearly) interest
      Continuous //!< Continuous interest
    };

  private:

    //! Internal class
    class Rate
    {
      public:

        //! Time (in days from interest curve date)
        int d;
        //! Time (in years from interest curve date)
        double y;
        //! Yearly rate value (0.025 -> 2.5% per year)
        double r;

      public:

        //! Constructor
        Rate(int d_, double y_=0.0, double r_=0.0) : d(d_), y(y_), r(r_) {}
    };

  private:

    //! Interest type
    Type mType;
    //! Interest curve date
    Date mDate;
    //! Rate values
    std::vector<Rate> mRates;
    //! Spline type
    bool mCubicSpline;
    //! Spline curve
    gsl_spline *mSpline;
    //! Spline accelerator
    gsl_interp_accel *mAccel;

  private:

    //! Insert a rate to list
    void insertRate(const Rate &);
    //! Given a time, returns the rate (interpolated)
    void getValues(int, double *, double *) const;
    //! Create spline curve
    void setSpline();

  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:

    //! Constructor
    Interest(const Date &date=NAD, Type type=Type::Compound);
    //! Copy constructor
    Interest(const Interest &);
    //! Destructor
    virtual ~Interest() override;
    //! Assignment operator
    Interest & operator=(const Interest &);
    //! Return interest type
    Type getType() const;
    //! Numbers of user-defined rates
    int size() const;
    //! Set initial date
    void setDate(const Date &date);
    //! Returns rate at date
    double getValue(const Date &date) const;
    //! Returns factor to apply Net Present Value at time date1
    double getFactor(const Date &date) const;

};

} // namespace

#endif

