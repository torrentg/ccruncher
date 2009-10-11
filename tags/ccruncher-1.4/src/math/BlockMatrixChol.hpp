
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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
    double **coefs;
    // diagonal cholesky coeficients (array size = N)
    double *diag;
    // for each element (0...N), gives his sector (array size = N)
    int *spe; // (Sector Per Element)

    // alloc memory, does checks, etc.
    void init(double **C, int *n, int m) throw(Exception);
    // adapted cholesky algorithm
    void chold(double **C) throw(Exception);
    // internal function
    double aprod(int row, int col, int order);
    // dealloc memory
    void reset();


  public:

    BlockMatrixChol(double **C, int *n, int m) throw(Exception);
    ~BlockMatrixChol();

    int getDim();
    double get(int row, int col);
    void mult(double *x, double *ret);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
