
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include <gsl/gsl_spline.h>
#include "params/Ratings.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class DefaultProbabilities : public ExpatHandlers
{

  private:

    struct pd
    {
      int day;
      double prob;
      // constructor
      pd(int d, double p) : day(d), prob(p) {}
      // less-than operator
      bool operator < (const pd &obj) const {
       return day < obj.day;
      }
    };

    struct fparams
    {
      gsl_spline *spline;
      gsl_interp_accel *accel;
      double y;
    };

  private:

    // initial date
    Date date;
    // default probabilities for each rating
    vector<vector<pd> > ddata;
    // ratings table
    Ratings ratings;
    // index of default rating
    int indexdefault;
    // cubic spline engines
    vector<gsl_spline *> splines;
    // splines accelerators
    vector<gsl_interp_accel *> accels;
    // type of splines
    string type;

  private:

    // insert a data value
    void insertValue(const string &r1, const Date &t, double val) throw(Exception);
    // validate object content
    void validate() throw(Exception);
    // set splines
    void setSplines();
    // inverse by root finding (derivative method)
    double inverse_cspline(gsl_spline *, double, gsl_interp_accel *) const;
    // inverse by root finding (bracketing method)
    double inverse_linear(gsl_spline *, double, gsl_interp_accel *) const;
    // root-finding function
    static double f(double x, void *params);
    // root-finding function
    static double df(double x, void *params);
    // root-finding function
    static void fdf (double x, void *params, double *y, double *dy);

  protected:

    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // defaults constructor
    DefaultProbabilities();
    // constructor
    DefaultProbabilities(const Ratings &, const Date &d) throw(Exception);
    // constructor
    DefaultProbabilities(const Ratings &, const Date &d, const vector<Date> &dates, const vector<vector<double> > &values) throw(Exception);
    // destructor
    ~DefaultProbabilities();
    // returns ratings size
    int size() const;
    // set date
    void setDate(const Date &);
    // return date
    Date getDate() const;
    // set ratings
    void setRatings(const Ratings &) throw(Exception);
    // return ratings
    const Ratings & getRatings() const;
    // return index of default rating
    int getIndexDefault() const;
    // evalue pd for irating at t
    double evalue(int irating, double t) const;
    // evalue pd for irating at t
    double evalue(int irating, const Date &t) const;
    // evalue pd inverse for irating at t
    double inverse(int irating, double val) const;
    // return maximum defined date
    Date getMaxDate(int irating) const;
    // return type of interpolation
    string getInterpolationType(int i) const;
    // serialize object content as xml
    string getXML(int) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif
