
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
    enum InterestType
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

        //! Time (as string)
        std::string t_str;
        //! Time (in days from interest curve date)
        int d;
        //! Time (in years from interest curve date)
        double y;
        //! Yearly rate value (0.025 -> 2.5% per year)
        double r;

      public:

        //! Constructor
        Rate(int d_, double y_=0.0, double r_=0.0, const std::string &str="") :
             t_str(str), d(d_), y(y_), r(r_) {}
        //! Less-than operator
        bool operator<(const Rate &right ) const { return (d < right.d); }
    };

  private:

    //! Interest type
    InterestType type;
    //! Interest curve date
    Date date;
    //! Rate values
    std::vector<Rate> rates;
    //! Spline type
    bool is_cubic_spline;
    //! Spline curve
    gsl_spline *spline;
    //! Spline accelerator
    gsl_interp_accel *accel;

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
    explicit Interest(const Date &date_=NAD, InterestType type=Compound);
    //! Copy constructor
    Interest(const Interest &);
    //! Destructor
    ~Interest();
    //! Assignment operator
    Interest & operator=(const Interest &);
    //! Return interest type
    InterestType getType() const;
    //! Numbers of user-defined rates
    int size() const;
    //! Set initial date
    void setDate(const Date &d);
    //! Returns rate at date
    double getValue(const Date &date1) const;
    //! Returns factor to apply Net Present Value at time date1
    double getFactor(const Date &date1) const;

};

} // namespace

#endif

