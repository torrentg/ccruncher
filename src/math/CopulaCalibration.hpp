
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
#include <gsl/gsl_vector.h>
#include "params/Defaults.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class CopulaCalibration
{

  private:

    // internal struct
    struct fparams
    {
      int k; // number of sectors (eg. 10)
      int *n; // number of individuals per sector (size=k)
      double *p; // 1-period probability default per sector (size=k)
      vector<vector<hdata> > h; // h[i,j] = observation at sector j at period i (size=txk)

      int dim; // auxiliar value (=sum(n))
      double **M; // auxiliar mem used by f (to avoid alloc/dealloc)
      double *x; // auxiliar mem used by f (to avoid alloc/dealloc)
      double *y; // auxiliar mem used by f (to avoid alloc/dealloc)
      bool coerced; // auxiliar, true if coerced in last evaluation

      // constructor
      fparams() : k(0), n(NULL), p(NULL), M(NULL), x(NULL), y(NULL) {}
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
    static void serialize(int k, const double **matrix, double value, gsl_vector *ret);
    // deserialize params
    static void deserialize(int k, const gsl_vector *ret, double **matrix, double *value);
    // returns observation
    static void getObservation(int row, const fparams *p, double *ret);
    // minimized function
    static double f(const gsl_vector *v, void *params_);
    static void df(const gsl_vector * x, void * params_, gsl_vector * g);
    static void fdf(const gsl_vector * x, void * params_, double *f, gsl_vector *g);

  public:

    // constructor
    CopulaCalibration();
    // destructor
    ~CopulaCalibration();
    // set function params
    void setParams(int k, int *n, const vector<vector<hdata> > &h, double *p) throw(Exception);
    // calibrate copula using MLE
    void run() throw(Exception);
    // return estimated ndf
    double getNdf() const;
    // return estimated correlations
    double** getCorrelations() const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

