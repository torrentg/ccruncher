
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

#ifndef _BlockGaussianCopula_
#define _BlockGaussianCopula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <gsl/gsl_rng.h>
#include "math/Copula.hpp"
#include "math/BlockMatrixChol.hpp"
#include "utils/LookupTable.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class BlockGaussianCopula : public Copula
{

  private:

    // number of elements
    int n;
    // number of sectors
    int m;
    // random number generator
    gsl_rng *rng;
    // cholesky matrix
    BlockMatrixChol *chol;
    // internal buffer
    double *aux1;
    // internal buffer
    double *aux2;
    // lookup table
    LookupTable *lut;
    // chol & lut allocation flag
    bool owner;

  private:
  
    // reset member values
    void reset();
    // dealloc buffers
    void finalize();
    // transform correlation
    double transform(double val);
    // generates correlated normal numbers
    void randNm();
    // initialize the lookup table
    void initLUT() throw(Exception);

  public:

    // constructor
    BlockGaussianCopula(double **C, int *n, int m) throw(Exception);
    // copy constructor
    BlockGaussianCopula(const BlockGaussianCopula &, bool alloc=true) throw(Exception);
    // destructor
    ~BlockGaussianCopula();
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
    // returns the cholesky condition number
    double getConditionNumber();
    // returns the Random Number Generator
    gsl_rng* getRng();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
