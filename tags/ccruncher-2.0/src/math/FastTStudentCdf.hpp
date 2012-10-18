
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

#ifndef _FastTStudentCdf_
#define _FastTStudentCdf_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <gsl/gsl_cdf.h>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class FastTStudentCdf
{

  private:

    // degrees of freedom
    double ndf;
    // values
    vector<double> breaks;
    // splines coefficients
    vector<double> coeffs;

  private:

    // utility function
    int sign(double x) const;
    // compute interpolating polynomial of degree 3
    double getCoeffs3(double x1, double x2, double *ret) const throw(Exception);
    // find interval [x0,rc] where spline error is less than EPSILON
    double bisection(double x0, double *ret) const throw(Exception);

  public:

    // constructor
    FastTStudentCdf(double nu) throw(Exception);
    // evaluate cdf at x
    double eval(double x) const;

};

//---------------------------------------------------------------------------

//===========================================================================
// evaluate cdf at x
//===========================================================================
inline double FastTStudentCdf::eval(double x) const
{
  double z = fabs(x);

  for (unsigned int i=0; i<breaks.size(); i++)
  {
    if (z < breaks[i]) 
    {
      const double *a = &coeffs[i*4];
      double ret = a[0]+z*(a[1]+z*(a[2]+z*a[3]));
      if (x < 0.0) return 1.0-ret;
      else return ret;
    }
  }

  return gsl_cdf_tdist_P(x, ndf);
}

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

