
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2008 Gerard Torrent
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
//
// TStudent.hpp - TStudent header - $Rev: 435 $
// --------------------------------------------------------------------------
//
// 2008/12/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _TStudent_
#define _TStudent_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cmath>

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class TStudent
{

  private:

    // non-instantiable class
    TStudent() {};
    static double bd0(double x, double np);
    static double stirlerr(double n);
    static double chebyshev_eval(double x, const double *a, const int n);
    static double gammafn(double x);
    static double lgammacor(double x);
    static double lgammafn_sign(double x, int *sgn);
    static double lgammafn(double x);
    static double lbeta(double a, double b);
    static double pbeta_raw(double x, double pin, double qin, int lower_tail, int log_p);
    static double pbeta(double x, double pin, double qin, int lower_tail, int log_p);


  public:

    static double pdf(double x, double ndf);
    static double cdf(double x, double ndf);
    static double cdfinv(double p, double ndf);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

