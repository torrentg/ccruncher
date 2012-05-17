
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
#include <vector>
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
      vector<int> n; // number of individuals per sector (size=k)
      vector<double> p; // 1-period probability default per sector (size=k)
      vector<vector<hdata> > h; // h[i,j] = observation at sector j at period i (size=txk)
      double **sigma; // correlation matrix (size=kxk)

      int dim; // auxiliar value (=sum(n))
      double **M; // auxiliar mem used by f (to avoid alloc/dealloc) (size=kxk)
      double *x; // auxiliar mem used by f (to avoid alloc/dealloc) (size=dim)
      double *y; // auxiliar mem used by f (to avoid alloc/dealloc) (size=dim)
      bool coerced; // auxiliar, true if coerced in last evaluation

      // constructor
      fparams() : k(0), sigma(NULL), dim(0), M(NULL), x(NULL), y(NULL), coerced(false) {}
    };

  private:

    // non-instantiable class
    CopulaCalibration() {}

    // returns observation
    static void getObservation(int row, const fparams *p, double *ret);
    // function to minimize
    static double f(double nu, void *params_);

  public:

    // estimate correlations
    static void correls(const vector<vector<hdata> > &h, double **ret) throw(Exception);
    // estimate ndf
    static double ndf(double **sigma, const vector<int> &n, const vector<double> &p, const vector<vector<hdata> > &h) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

