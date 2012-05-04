
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

#ifndef _BlockMatrixCholInv_
#define _BlockMatrixCholInv_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"
#include "math/BlockMatrixChol.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class BlockMatrixCholInv
{

  private:

    // number of sectors (with more than 0 elements)
    int M;
    // number of elements (Cholesky matrix size = NxN)
    int N;
    // number of elements in each sector (array size = M)
    int *n;
    // cholesky coeficients (matrix size = NxM)
    double *coefs;
    // diagonal cholesky coeficients (array size = N)
    double *diag;

  private:

    // adapted cholesky algorithm
    void invert(const double *H, const double *v);

  public:

    // copy constructor
    BlockMatrixCholInv(const BlockMatrixChol &x);
    // destructor
    ~BlockMatrixCholInv();
    // returns matrix dimension (N)
    int getDim() const;
    // returns matrix element
    double get(int row, int col) const;
    // multiplies this matrix by a vector
    void mult(double *x, double *ret) const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
