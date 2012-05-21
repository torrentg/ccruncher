
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

#ifndef _BlockMatrixChol_
#define _BlockMatrixChol_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

// forward declaration
class BlockMatrixCholInv;

//---------------------------------------------------------------------------

struct eig
{
  double value;
  int multiplicity;
  eig(double x, int n) : value(x), multiplicity(n) {}
};

class BlockMatrixChol
{

  private:

    // number of sectors (with more than 0 elements)
    int M;
    // number of elements (Cholesky matrix size = NxN)
    int N;
    // correlations between sectors (matrix size = MxM)
    double **A;
    // number of elements in each sector (array size = M)
    int *n;
    // cholesky coeficients (matrix size = MxN)
    double *coefs;
    // diagonal cholesky coeficients (array size = N)
    double *diag;
    // list of eigenvalues
    vector<eig> eigenvalues;
    // coerced flag
    bool coerced;

  private:
  
    // dealloc memory
    void reset();
    // adapted cholesky algorithm
    void chold() throw(Exception);
    // coerce, computes condition number (2-norm) and determinant
    void prepare(bool coerce) throw(Exception);
    // multiply VEPS·diag(vaps)·inv(VEPS)
    void prod(gsl_matrix_complex *VEPS, gsl_vector_complex *vaps, gsl_matrix_complex *R) const throw(Exception);

  public:

    // copy constructor
    BlockMatrixChol(const BlockMatrixChol &x);
    // constructor
    BlockMatrixChol(double **A, int *n, int m, bool coerce=false) throw(Exception);
    // destructor
    ~BlockMatrixChol();
    // assignement operator
    BlockMatrixChol& operator = (const BlockMatrixChol &x);
    // returns matrix dimension (N)
    int getDim() const;
    // returns matrix inverse
    BlockMatrixCholInv* getInverse() const;
    // returns matrix element
    double get(int row, int col) const;
    // multiplies this matrix by a vector
    void mult(double *x, double *ret) const;
    // returns matrix condition number
    double getConditionNumber() const;
    // returns matrix determinant
    double getDeterminant() const;
    // indicates if given correlations has been modified
    // to avoid a non-definite-positive correlation matrix
    bool isCoerced() const;
    // return correlations matrix (used only to debug)
    double** getCorrelations() const;
    // return correlations matrix eigenvalues (not Cholesky)
    const vector<eig>& getEigenvalues() const;

  public:

    // friend class
    friend class BlockMatrixCholInv;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
