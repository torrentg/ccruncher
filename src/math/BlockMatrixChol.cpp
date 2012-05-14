
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

#include <cmath>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include "math/BlockMatrixChol.hpp"
#include "math/BlockMatrixCholInv.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define EPSILON 1E-12
#define MIN_EIGENVALUE 1e-4
#define COEFS(i,j) (coefs[(i)*N+(j)])

//===========================================================================
// Description:
// ------------
// implements the Cholesky decomposition algorithm for block matrices
// described in paper 'Simulation of High-Dimensional t-Student Copula
// Copulas with a Given Block Matrix Correlation Matrix' by
// Gerard Torrent-Gironella and Josep Fortiana
//
// Notes:
// ------
// assumes that diagonal elements are always 1
//
//===========================================================================

//===========================================================================
// Default constructor
//===========================================================================
ccruncher::BlockMatrixChol::BlockMatrixChol()
{
  M = 0;
  N = 0;
  A = NULL;
  n = NULL;
  coefs = NULL;
  diag = NULL;
}

//===========================================================================
// Copy constructor
//===========================================================================
ccruncher::BlockMatrixChol::BlockMatrixChol(const BlockMatrixChol &x)
{
  M = 0;
  N = 0;
  A = NULL;
  n = NULL;
  coefs = NULL;
  diag = NULL;
  *this = x;
}

//===========================================================================
// assignement operator
//===========================================================================
BlockMatrixChol& ccruncher::BlockMatrixChol::operator = (const BlockMatrixChol &x)
{
  reset();
  M = x.M;
  N = x.N;
  A = Arrays<double>::allocMatrix(M, M, x.A);
  n = Arrays<int>::allocVector(M, x.n);
  coefs = Arrays<double>::allocVector(N*M, x.coefs);
  diag = Arrays<double>::allocVector(N, x.diag);
  return *this;
}

//===========================================================================
// Constructor
// A: matrix with correlations between sectors (simmetric with size = mxm)
// n: number of elements in each sector (size = m)
// m: number of sectors
// Example: A = {{0.2,0.1},{0.1, 0.3}}, n={2,3}, m=2
// indicates the following matrix:
//    1.0 0.2 0.1 0.1 0.1
//    0.2 1.0 0.1 0.1 0.1
//    0.1 0.1 1.0 0.3 0.3
//    0.1 0.1 0.3 1.0 0.3
//    0.1 0.1 0.3 0.3 1.0
// first we asure that input arguments are valids and  don't exist sectors
// without elements. If exist, they are removed
//===========================================================================
ccruncher::BlockMatrixChol::BlockMatrixChol(double **A_, int *n_, int m_) throw(Exception)
{
  // assertions
  assert(A_ != NULL);
  assert(n_ != NULL);
  assert(m_ > 0);

  // initializations
  M = 0;
  N = 0;
  A = NULL;
  n = NULL;
  coefs = NULL;
  diag = NULL;

  // checking that matrix is symmetric
  for(int i=0; i<m_; i++)
  {
    for(int j=0; j<m_; j++)
    {
      if(fabs(A_[i][j]-A_[j][i]) > EPSILON)
      {
        throw Exception("trying to perform a Cholesky decomposition for a non-symmetric matrix");
      }
    }
  }

  // dealing with n[i]=0's
  for(int j=0; j<m_; j++)
  {
    if (n_[j] < 0) {
      throw Exception("error: exist a sector with a negative number of elements");
    }
    if (n_[j] == 0) {
      //nm don't increment
    }
    else {
      M++;
      N += n_[j];
    }
  }

  if (M == 0 || m_ <= 0)
  {
    throw Exception("input matrix dimension have dimension 0");
  }
  else
  {
    try
    {
      A = Arrays<double>::allocMatrix(M, M, 0.0);
      n = Arrays<int>::allocVector(M, 0);

      // filling A and n removing void sectors
      for(int i=0,ni=0; i<m_; i++)
      {
        if (n_[i] != 0)
        {
          for(int j=0,nj=0; j<m_; j++)
          {
            if (n_[j] != 0)
            {
              A[ni][nj] = A_[i][j];
              nj++;
            }
          }
          n[ni] = n_[i];
          ni++;
        }
      }

      // compute A eigenvalues and coerce if required
      prepare();

      // allocating memory for cholesky coeficients
      coefs = Arrays<double>::allocVector(N*M, 0.0);

      // allocating memory for diagonal cholesky coeficients
      diag = Arrays<double>::allocVector(N, 0.0);

      // doing Cholesky factorization
      chold();
    }
    catch(Exception &e)
    {
      reset();
      throw;
    }
  }
}

//===========================================================================
// Destructor
//===========================================================================
ccruncher::BlockMatrixChol::~BlockMatrixChol()
{
  reset();
}

//===========================================================================
// reset
//===========================================================================
void ccruncher::BlockMatrixChol::reset()
{
  // deallocating correlations
  if (A != NULL && M > 0) {
    Arrays<double>::deallocMatrix(A, M);
    A = NULL;
  }

  // deallocating vector n
  if (n != NULL) {
    Arrays<int>::deallocVector(n);
    n = NULL;
  }

  // deallocating matrix coefs
  if (coefs != NULL) {
    Arrays<double>::deallocVector(coefs);
    coefs = NULL;
  }

  // deallocating diagonal matrix coefs
  if (diag != NULL) {
    Arrays<double>::deallocVector(diag);
    diag = NULL;
  }

  // rest of values
  M = 0;
  N = 0;
}

//===========================================================================
// return the matrix dimension
//===========================================================================
int ccruncher::BlockMatrixChol::getDim() const
{
  return N;
}

//===========================================================================
// returns element at row,col of cholesky matrix (lower matrix)
// non-optimal function, use only to debug or to test code
//===========================================================================
double ccruncher::BlockMatrixChol::get(int row, int col) const
{
  assert(row >= 0 && row < N);
  assert(col >= 0 && col < N);

  if (col < row)
  {
    // find sector of the element
    int sector = -1;
    for(int sum=0,i=0; i<M; i++)
    {
      sum += n[i];
      if (row < sum) {
        sector = i;
        break;
      }
    }
    assert(sector >= 0);
    return COEFS(sector, col);
  }
  else if (col == row)
  {
    return diag[col];
  }
  else
  {
    return 0.0;
  }
}

//===========================================================================
// returns cholesky matrix multiplied by vector x
//===========================================================================
void ccruncher::BlockMatrixChol::mult(double *x, double *ret) const
{
  int i, j, r;
  double *ptr;

  assert(x != NULL);
  assert(ret != NULL);

  for(i=-1, r=0; r<M; r++)
  {
    i++;
    ret[i] = 0.0;
    
    // computing the first element of this sector (diagonal product not included)
    ptr = coefs+r*N;
    for(j=0; j<i; j++) 
    {
      ret[i] += ptr[j]*x[j]; // COEFS(r,j)*x[j];
    }

    // computing the rest of elements of this sector (diagonal product not included)
    for(j=1; j<n[r]; j++)
    {
      i++;
      ret[i] = ret[i-1] + ptr[i-1]*x[i-1]; // ret[i-1] + COEFS(r,i-1)*x[i-1];
    }
  }

  // adding diagonal products
  for(j=0; j<=i; j++)
  {
    ret[j] += diag[j]*x[j];
  }
}

//===========================================================================
// adapted cholesky decomposition for block matrix
//===========================================================================
void ccruncher::BlockMatrixChol::chold() throw(Exception)
{
  int i, j, r, s, q;
  double val1=0.0, val2=0.0;

  assert(A != NULL);

  diag[0] = 1.0;

  for(r=0; r<M; r++)
  {
    COEFS(r,0) = A[r][0]/diag[0];
  }

  for(i=-1, r=0; r<M; r++)
  {
    for(q=0; q<n[r]; q++)
    {
      i++;
      if (i==0) continue;
      if (q == 0)
      {
        for(val1=0.0, j=0; j<i; j++)
        {
          val1 += COEFS(r,j)*COEFS(r,j);
        }
      }
      else
      {
        val1 += COEFS(r,i-1)*COEFS(r,i-1);
      }
      if (1.0-val1 < 0.0) {
        throw Exception("non definite positive block matrix");
      }
      else {
        diag[i] = sqrt(1.0-val1);
      }
      for(s=r; s<M; s++)
      {
        if (q==n[r]-1 && s==r) continue;
        for(val2=0.0, j=0; j<i; j++)
        {
          val2 += COEFS(r,j)*COEFS(s,j);
        }
        COEFS(s,i) = (A[s][r]-val2)/diag[i];
      }
    }
  }
}

//===========================================================================
// internal function
// computes eigenvalues of the matrix A=L*L' (where L is the Cholesky matrix)
//
// the (N_1+N_2+...+N_m) eigenvalues of A are:
//   * 1-A11 with multiplicity (N1)-1
//   * 1-A22 with multiplicity (N2)-1
//   *  ...
//   * 1-Amm with multiplicity (Nk)-1
//   * eig_1 (see below)
//   * eig_2 (see below)
//   *  ...
//   * eig_m (see below)
// where:
//   * m is the number of blocks
//   * Ni is the dimension of i-th block
//   * Aij is the value of the i-j block (note: Aij=Aji)
//
// eig_i are the eigenvalues of the following deflated matrix:
//
//   (1+(N1-1)*A11) (N2*A12)       ... (Nm*A1m)
//   (N1*A21)       (1+(N2-1)*A22) ... (Nm*A2m)
//     ...              ...            ...  ...
//   (N1*Am1)       (N2*Am2)       ... (1+(Nm-1)*A1m)
//
//
// given a non-valid correlation matrix (definite-positive with 1's in
// the diagonal), finds a valid correlation matrix close to the given one.
//
// based on 'Quantitative Risk Management: Concepts, Techniques and Tools'
// by Alexander J. McNeil, Rudiger Frey, Paul Embrechts (see algorithm 5.55)
//
// parameters:
//   * A: correlation matrix with Aij=Aji (size=m)
//   * n: number of elements per sector (size=m)
//   * m: number of sectors
//
// return:
//   * a valid correlation matrix, condition number and determinant
//
//===========================================================================
void ccruncher::BlockMatrixChol::prepare() throw(Exception)
{
  bool coerced1=false, coerced2=false;
  double *eigenvalues = Arrays<double>::allocVector(2*M, NAN);

  // defining variables to perform eigenvalues functions
  gsl_matrix *K = gsl_matrix_alloc(M, M);
  gsl_vector_complex *vaps = gsl_vector_complex_alloc(M);
  gsl_matrix_complex *VEPS = gsl_matrix_complex_alloc(M, M);

  // filling deflated matrix
  for(int i=0; i<M; i++)
  {
    for(int j=0; j<M; j++)
    {
      double val = 0.0;
      if (i == j) 
      {
        if (1.0-A[i][j] < MIN_EIGENVALUE)
        {
          A[i][j] = 1.0 - MIN_EIGENVALUE;
          coerced1 = true;
        }
        val = 1.0 + (n[j]-1)*A[i][j];
      }
      else 
      {
        val = n[j] * A[i][j];
      }
      gsl_matrix_set(K, i, j, val);
    }
  }

  // computing deflated matrix eigenvalues
  gsl_eigen_nonsymmv_workspace *w = gsl_eigen_nonsymmv_alloc(M);
  int rc = gsl_eigen_nonsymmv(K, vaps, VEPS, w);
  gsl_eigen_nonsymmv_free(w);
  if (rc) {
    gsl_matrix_free(K);
    gsl_vector_complex_free(vaps);
    gsl_matrix_complex_free(VEPS);
    Arrays<double>::deallocVector(eigenvalues);
    throw Exception("unable to compute eigenvalues: " + string(gsl_strerror(rc)));
  }

  // filling trivial eigenvalues
  for(int i=0; i<M; i++)
  {
    eigenvalues[i] = 1.0 - A[i][i];
  }

  // retrieving eigenvalues
  for(int i=0; i<M; i++)
  {
    gsl_complex z = gsl_vector_complex_get(vaps, i);
    if (fabs(GSL_IMAG(z)) >= EPSILON)
    {
      gsl_vector_complex_free(vaps);
      gsl_matrix_complex_free(VEPS);
      Arrays<double>::deallocVector(eigenvalues);
      throw Exception("imaginary eigenvalue computing eigenvalues");
    }
    else
    {
      if (GSL_REAL(z) < MIN_EIGENVALUE)
      {
        GSL_REAL(z) = MIN_EIGENVALUE;
        GSL_IMAG(z) = 0.0;
        gsl_vector_complex_set(vaps, i, z);
        coerced2 = true;
      }
      eigenvalues[M+i] = GSL_REAL(z);
    }
  }

  // coercing to a valid correlation matrix
  if (coerced2 == true)
  {
    gsl_matrix_complex *R = gsl_matrix_complex_alloc(M, M);
    prod(VEPS, vaps, R);
    for(int i=0; i<M; i++)
    {
      for(int j=0; j<=i; j++)
      {
        // undoing deflated matrix
        if (i == j)
        {
          if (n[i] > 1)
          {
            gsl_complex *val = gsl_matrix_complex_ptr(R, i, j);
            GSL_REAL(*val) -= 1.0;
            GSL_REAL(*val) /= (n[j]-1.0);
            GSL_IMAG(*val) /= (n[j]-1.0);
          }
        }
        else
        {
          gsl_complex *val = gsl_matrix_complex_ptr(R, i, j);
          GSL_REAL(*val) /= n[j];
          GSL_IMAG(*val) /= n[j];
        }

        // z1 and z2 are equals and reals
        // doing mean to round decimals
        gsl_complex z1 = gsl_matrix_complex_get(R, i, j);
        gsl_complex z2 = gsl_matrix_complex_get(R, j, i);
        assert(fabs(GSL_IMAG(z1)) < EPSILON);
        assert(fabs(GSL_IMAG(z2)) < EPSILON);
        A[i][j] = (GSL_REAL(z1) + GSL_REAL(z2))/2.0;
        A[j][i] = A[i][j];
      }
    }
    gsl_matrix_complex_free(R);
  }

  cond = getConditionNumber(eigenvalues);
  det = getDeterminant(eigenvalues);
  coerced = (coerced1||coerced2);

  gsl_matrix_free(K);
  gsl_vector_complex_free(vaps);
  gsl_matrix_complex_free(VEPS);
  Arrays<double>::deallocVector(eigenvalues);
}

//===========================================================================
// internal function
// given a matrix VEPS and a vector vaps, does the following operation:
//  R = VEPS·diag(vaps)·inv(VEPS)
// where inv() is the matrix inverse
//===========================================================================
void ccruncher::BlockMatrixChol::prod(gsl_matrix_complex *VEPS, gsl_vector_complex *vaps, gsl_matrix_complex *R) const throw(Exception)
{
  assert(VEPS != NULL && vaps != NULL && R != NULL);

  if (VEPS == NULL || vaps == NULL || R == NULL) throw Exception("invalid objects");
  if (VEPS->size1 <= 0 || VEPS->size1 != VEPS->size2) throw Exception("non valid matrix");
  if (VEPS->size1 != vaps->size) throw Exception("distinct sizes");
  if (VEPS->size1 != R->size1 || VEPS->size1 != R->size2) throw Exception("non valid matrix");

  int rc = 0;
  int k = VEPS->size1;

  // computing the LU decomposition of VEPS
  int signum=0;
  gsl_matrix_complex *LU = gsl_matrix_complex_alloc(k, k);
  gsl_matrix_complex_memcpy(LU, VEPS);
  gsl_permutation *w = gsl_permutation_alloc(k);
  rc = gsl_linalg_complex_LU_decomp(LU, w, &signum);
  if (rc) {
    gsl_matrix_complex_free(LU);
    gsl_permutation_free(w);
    throw Exception("unable to inverse matrix: " + string(gsl_strerror(rc)));
  }

  // computing the inverse of eigenvectors (SPEV)
  gsl_matrix_complex *SPEV = gsl_matrix_complex_alloc(k, k);
  rc = gsl_linalg_complex_LU_invert(LU, w, SPEV);
  gsl_permutation_free(w);
  if (rc) {
    gsl_matrix_complex_free(SPEV);
    gsl_matrix_complex_free(LU);
    throw Exception("unable to inverse matrix: " + string(gsl_strerror(rc)));
  }

  // creating diagonal matrix
  gsl_matrix_complex_set_zero(R);
  for(int i=0; i<k; i++) {
    gsl_complex z = gsl_vector_complex_get(vaps, i);
    gsl_matrix_complex_set(R, i, i, z);
  }

  // computing VEPS*VAPS*SPEV
  gsl_complex z0, z1;
  GSL_REAL(z0)=0.0; GSL_IMAG(z0) = 0.0;
  GSL_REAL(z1)=1.0; GSL_IMAG(z1) = 0.0;
  gsl_matrix_complex_set_zero(LU);
  gsl_blas_zgemm(CblasNoTrans, CblasNoTrans, z1, VEPS, R, z0, LU);
  gsl_matrix_complex_set_zero(R);
  gsl_blas_zgemm(CblasNoTrans, CblasNoTrans, z1, LU, SPEV, z0, R);
  gsl_matrix_complex_free(SPEV);
  gsl_matrix_complex_free(LU);
}

//===========================================================================
// returns the condition number (2-norm) of the Cholesky matrix
// if A = L*L' where L is the Cholesky matrix, then the condition number
// is sqrt(max(eig(A)),min(eig(A))
//===========================================================================
double ccruncher::BlockMatrixChol::getConditionNumber(const double *eigenvalues) const
{
  double eigmin = +1e100;
  double eigmax = -1e100;

  for(int i=0; i<2*M; i++)
  {
    if (i < M && n[i] <= 1) continue;
    double val = std::fabs(eigenvalues[i]);
    if (val < eigmin) eigmin = val;
    if (eigmax < val) eigmax = val;
  }

  return std::sqrt(eigmax/eigmin);
}

//===========================================================================
// returns the condition number (2-norm) of the Cholesky matrix
//===========================================================================
double ccruncher::BlockMatrixChol::getConditionNumber() const
{
  return cond;
}

//===========================================================================
// returns the determinant of the Cholesky matrix
// observe that det(L) = sqrt(det(A))
// note: equivalent to the multiplication of all Cholesky diagonal values
//===========================================================================
double ccruncher::BlockMatrixChol::getDeterminant(const double *eigenvalues) const
{
  double ret = 1.0;

  for(int i=0; i<2*M; i++)
  {
    if (i < M)
    {
      if (n[i] > 1) ret *= eigenvalues[i] * (n[i]-1.0);
    }
    else
    {
      ret *= eigenvalues[i];
    }
  }

  assert(ret > 0.0);
  return sqrt(ret);
}

//===========================================================================
// returns the determinant of the Cholesky matrix
//===========================================================================
double ccruncher::BlockMatrixChol::getDeterminant() const
{
  return det;
}

//===========================================================================
// returns matrix inverse
//===========================================================================
BlockMatrixCholInv* ccruncher::BlockMatrixChol::getInverse() const
{
  return new BlockMatrixCholInv(*this);
}

//===========================================================================
// isCoerced
//===========================================================================
bool ccruncher::BlockMatrixChol::isCoerced() const
{
  return coerced;
}
