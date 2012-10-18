
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

#ifndef _GMFCopula_
#define _GMFCopula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include "math/Copula.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class GMFCopula : public Copula
{

  protected:

    // number of elements per factor
    unsigned int *n;
    // factor loadings
    double *w;
    // cholesky matrix
    gsl_matrix *chol;
    // memory owner flag
    bool owner;
    // number of factors
    unsigned int k;
    // random number generator
    gsl_rng *rng;
    // simulated factors
    gsl_vector *aux;
    // simulated values
    vector<double> values;

  protected:

    // dealloc buffers
    void finalize();
    // simulate a multivariate gaussian
    void rmvnorm();

  public:

    // constructor
    GMFCopula(const vector<vector<double> > &M, const vector<unsigned int> &dims) throw(Exception);
    // copy constructor
    GMFCopula(const GMFCopula &, bool alloc=true) throw(Exception);
    // destructor
    ~GMFCopula();
    // clone
    Copula* clone(bool alloc=true);
    // returns the copula size (n)
    int size() const;
    // simulates a copula realization
    void next();
    // returns i-th component
    double get(int) const;
    // returns simulated values
    const double* get() const;
    // random number generator seed
    void setSeed(long);
    // returns the Random Number Generator
    gsl_rng* getRng();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
