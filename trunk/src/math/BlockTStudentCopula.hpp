
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

#ifndef _BlockTStudentCopula_
#define _BlockTStudentCopula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <gsl/gsl_rng.h>
#include "math/Copula.hpp"
#include "math/BlockMatrixChol.hpp"
#include "math/FastTStudentCdf.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class BlockTStudentCopula : public Copula
{

  private:

    // number of elements
    int n;
    // number of degrees of freedom
    double ndf;
    // random number generator
    gsl_rng *rng;
    // cholesky matrix
    BlockMatrixChol *chol;
    // internal buffer
    double *aux1;
    // internal buffer
    double *aux2;
    // t-student cdf
    FastTStudentCdf tcdf;
    // chol allocation flag
    bool owner;

  private:

    // reset member values
    void reset();
    // dealloc buffers
    void finalize();

  public:

    // constructor
    BlockTStudentCopula(double **C, int *n, int m, double ndf, int type=1, bool coerce=false) throw(Exception);
    // copy constructor
    BlockTStudentCopula(const BlockTStudentCopula &, bool alloc=true) throw(Exception);
    // destructor
    ~BlockTStudentCopula();
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
    // returns the cholesky matrix
    const BlockMatrixChol* getCholesky() const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
