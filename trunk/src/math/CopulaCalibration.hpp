
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

#ifndef _CopulaCalibration_
#define _CopulaCalibration_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class CopulaCalibration
{

  private:

    // internal struct
    struct fparams
    {
      int k; // number of sectors (eg. 10)
      int t; // number of historical rows (eg. 20)
      int *n; // number of indiviudals per sector (size=k)
      double *p // 1-period probability default per sector (size=k)
      double **h; // h[i,j] = pct defaults in sector j at period i (size=txk)
    };

  private:

    // function params
    fparams params;
    // optimal correlation matrix (size=kxk)
    double **M;
    // optimal ndf
    double ndf;

  private:

    // reset
    void reset();
    // serialize params
    void serialize(const double **matrix, double value, gsl_vector *ret);
    // deserialize params
    void deserialize(const gsl_vector *ret, double **matrix, double *value);

    // minimized function
    static double f(const gsl_vector *v, void *params);

  public:

    // constructor
    CopulaCalibration();
    // destructor
    ~CopulaCalibration();
    // set function params
    void setParams(int k, int *n, double **h, int t, double *p) throw(Exception);
    // calibrate copula using MLE
    void run(const double **sigma0, double ndf0) throw(Exception);
    // return estimated ndf
    double getNdf() const;
    // return estimated correlations
    const double** getCorrelations() const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

