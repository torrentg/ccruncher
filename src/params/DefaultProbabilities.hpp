
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

#ifndef _DefaultProbabilities_
#define _DefaultProbabilities_

#include <string>
#include <vector>
#include <gsl/gsl_spline.h>
#include "params/Ratings.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Default probabilities functions.
 *
 * @details These functions provides the probability of default (PD) at
 *          a given time (t) for a fixed rating (r): PD(t;r).
 *          This class provides methods to retrieve them from the xml
 *          input file, and evaluate them using spline interpolation.
 *
 * @see http://ccruncher.net/ifileref.html#dprobs
 */
class DefaultProbabilities : public ExpatHandlers
{

  private:

    //! Internal struct
    struct pd
    {
      //! Time in days from starting date
      int day;
      //! Probability of default
      double prob;
      //! Constructor
      pd(int d, double p) : day(d), prob(p) {}
      //! Less-than operator
      bool operator < (const pd &obj) const {
       return day < obj.day;
      }
    };

    //! Internal struct (root-finding)
    struct fparams
    {
      gsl_spline *spline;
      gsl_interp_accel *accel;
      double y;
    };

  private:

    //! Initial date
    Date date;
    //! Default probabilities for each rating
    std::vector<std::vector<pd>> ddata;
    //! List of ratings
    Ratings ratings;
    //! Index of the default rating
    int indexdefault;
    //! Spline for each rating
    std::vector<gsl_spline *> splines;
    //! Splines accelerators
    std::vector<gsl_interp_accel *> accels;

  private:

    //! Root finding solver function
    static double f(double x, void *params);
    //! Root finding solver function
    static double df(double x, void *params);
    //! Root finding solver function
    static void fdf (double x, void *params, double *y, double *dy);

    //! Reset object content
    void reset();
    //! Insert a data value
    void insertValue(const std::string &r1, const Date &t, double val) throw(Exception);
    //! Validate object content
    void validate() throw(Exception);
    //! Set splines
    void setSplines();
    //! Inverse by root finding (bisection method)
    double inverse_linear(gsl_spline *, double, gsl_interp_accel *) const;
    //! Inverse by root finding (Newton method)
    double inverse_cspline(gsl_spline *, double, gsl_interp_accel *) const;

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:

    //! Default constructor
    DefaultProbabilities();
    //! Copy constructor
    DefaultProbabilities(const DefaultProbabilities &);
    //! Constructor
    DefaultProbabilities(const Ratings &, const Date &d) throw(Exception);
    //! Constructor
    DefaultProbabilities(const Ratings &, const Date &d, const std::vector<Date> &dates, const std::vector<std::vector<double>> &values) throw(Exception);
    //! Destructor
    ~DefaultProbabilities();
    //! Assignment operator
    DefaultProbabilities & operator=(const DefaultProbabilities &);
    //! Number of default probabilities functions
    int size() const;
    //! Set initial date
    void setDate(const Date &);
    //! Return initial date
    Date getDate() const;
    //! Set ratings
    void setRatings(const Ratings &) throw(Exception);
    //! Return ratings
    const Ratings & getRatings() const;
    //! Return index of default rating
    int getIndexDefault() const;
    //! Evalue pd for i-th rating at t
    double evalue(int irating, double t) const;
    //! Evalue pd for i-th rating at t
    double evalue(int irating, const Date &d) const;
    //! Evalue pd inverse for i-th rating at given probability
    double inverse(int irating, double val) const;
    //! Return maximum defined date
    Date getMaxDate(int irating) const;
    //! Return list of days where dprob is defined
    std::vector<int> getDays(int irating) const;
    //! Return type of interpolation
    std::string getInterpolationType(int i) const;

};

} // namespace

#endif

