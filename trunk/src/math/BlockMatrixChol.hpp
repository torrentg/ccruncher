
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

#ifndef _BlockMatrixChol_
#define _BlockMatrixChol_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class BlockMatrixChol
{

  private:

    // number of sectors
    int M;
    // number of elements
    int N;
    // number of elements in each sector (array size = M)
    int *n;
    // cholesky coeficients (matrix size = NxM)
    double *coefs;
    // diagonal cholesky coeficients (array size = N)
    double *diag;
    // for each element (0...N), gives his sector (array size = N)
    int *spe; // (Sector Per Element)
    // condition number of the cholesky matrix
    double cnum;

  private:
  
    // alloc memory, does checks, etc.
    void init(double **A, int *n, int m) throw(Exception);
    // adapted cholesky algorithm
    void chold(double **A) throw(Exception);
    // internal function
    double aprod(int row, int col, int order);
    // computes condition number (2-norm)
    double cond(double **A, int *n, int m) throw(Exception);
    // dealloc memory
    void reset();

  public:

    // default constructor
    BlockMatrixChol();
    // copy constructor
    BlockMatrixChol(const BlockMatrixChol &x);
    // constructor
    BlockMatrixChol(double **A, int *n, int m) throw(Exception);
    // destructor
    ~BlockMatrixChol();
    // assignement operator
    BlockMatrixChol& operator = (const BlockMatrixChol &x);
    // returns matrix dimension (N)
    int getDim();
    // returns matrix condition number
    double getConditionNumber();
    // returns matrix element
    double get(int row, int col);
    // multiplies this matrix by a vector
    void mult(double *x, double *ret);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

