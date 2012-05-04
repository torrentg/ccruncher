
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
#include "utils/Arrays.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define EPSILON 1E-12
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
  n = NULL;
  coefs = NULL;
  diag = NULL;
  eigenvalues = NULL;
}

//===========================================================================
// Copy constructor
//===========================================================================
ccruncher::BlockMatrixChol::BlockMatrixChol(const BlockMatrixChol &x)
{
  M = 0;
  N = 0;
  n = NULL;
  coefs = NULL;
  diag = NULL;
  eigenvalues = NULL;
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
  n = Arrays<int>::allocVector(M, x.n);
  coefs = Arrays<double>::allocVector(N*M, x.coefs);
  diag = Arrays<double>::allocVector(N, x.diag);
  eigenvalues = Arrays<double>::allocVector(2*M, x.eigenvalues);
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

  // new m
  int nm=0;

  // dealing with n[i]=0's
  for(int j=0;j<m_;j++)
  {
    if (n_[j] < 0) {
      throw Exception("error: exist a sector with a negative number of elements");
    }
    if (n_[j] == 0) {
      //nm don't increment
    }
    else {
      nm++;
    }
  }

  if (nm == m_ && m_ > 0)
  {
    // perform factorization
    init(A_, n_, m_);
    // compute A eigenvalues
    eigen(A_, n_, m_);
  }
  else if (nm == 0 || m_ <= 0)
  {
    throw Exception("input matrix dimension have dimension 0");
  }
  else // nm > 0 and exist sector/s with 0 elements
  {
    double **nA = NULL;
    int *nn = NULL;

    try
    {
      // alloc temporal memory
      nA = Arrays<double>::allocMatrix(nm, nm, 0.0);
      nn = Arrays<int>::allocVector(nm, 0);

      // filling A and n without void sectors
      for(int i=0,ni=0;i<m_;i++)
      {
        if (n_[i] != 0)
        {
          for(int j=0,nj=0;j<m_;j++)
          {
            if (n_[j] != 0)
            {
              nA[ni][nj] = A_[i][j];
              nj++;
            }
          }
          nn[ni] = n_[i];
          ni++;
        }
      }

      // perform factorization
      init(nA, nn, nm);
      // compute A eigenvalues
      eigen(nA, nn, nm);

      // release temporal memory
      Arrays<double>::deallocMatrix(nA, nm);
      nA = NULL;
      Arrays<int>::deallocVector(nn);
      nn = NULL;
    }
    catch(Exception &e)
    {
      if (nA != NULL) Arrays<double>::deallocMatrix(nA, nm);
      if (nn != NULL) Arrays<int>::deallocVector(nn);
      throw;
    }
  }
}

//===========================================================================
// init
// Note: take care, we check that Aij = Aji
//===========================================================================
void ccruncher::BlockMatrixChol::init(double **A_, int *n_, int m_) throw(Exception)
{
  // initializations
  N = 0;
  n = NULL;
  coefs = NULL;
  diag = NULL;
  eigenvalues = NULL;

  // assigning the number of sectors
  M = m_;

  // assigning the number of elements per sector
  n = Arrays<int>::allocVector(M, 0);

  for(int i=0;i<M;i++)
  {
    // filling internal vector named n
    n[i] = n_[i];

    // computing dimension matrix
    N += n[i];

    // checking that Aij = Aji
    for(int j=0;j<M;j++)
    {
      if(fabs(A_[i][j]-A_[j][i]) > EPSILON)
      {
        reset();
        throw Exception("trying to perform a Cholesky decomposition for a non-simetric matrix");
      }
    }
  }

  // allocating memory for cholesky coeficients
  coefs = Arrays<double>::allocVector(N*M, 0.0);

  // allocating memory for diagonal cholesky coeficients
  diag = Arrays<double>::allocVector(N, 0.0);

  // allocating memory for eigenvalues
  eigenvalues = Arrays<double>::allocVector(2*M, 0.0);

  // call adapted Cholesky algorithm
  try
  {
    chold(A_);
  }
  catch(Exception &e)
  {
    reset();
    throw;
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

  // deallocating eigenvalues values
  if (eigenvalues != NULL) {
    Arrays<double>::deallocVector(eigenvalues);
    eigenvalues = NULL;
  }

  // rest of values
  M = 0;
  N = 0;
}

//===========================================================================
// return the matrix dimension
//===========================================================================
int ccruncher::BlockMatrixChol::getDim()
{
  return N;
}

//===========================================================================
// returns element at row,col of cholesky matrix (lower matrix)
// non-optimal function, use only to debug or to test code
//===========================================================================
double ccruncher::BlockMatrixChol::get(int row, int col)
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
void ccruncher::BlockMatrixChol::mult(double *x, double *ret)
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
void ccruncher::BlockMatrixChol::chold(double **A) throw(Exception)
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
// eig_i are the eigenvalues of the following matrix (normal matrix, non composed by blocks):
//
//   (1+(N1-1)*A11) (N2*A12)       ... (Nm*A1m)
//   (N1*A21)       (1+(N2-1)*A22) ... (Nm*A2m)
//     ...              ...            ...  ...
//   (N1*Am1)       (N2*Am2)       ... (1+(Nm-1)*A1m)
//
// notes: 
//   * is mandatory that m > 0
//   * is mandatory that Ni > 0
//   * is mandatory than Aij = Aji
//
//===========================================================================
void ccruncher::BlockMatrixChol::eigen(double **A_, int *n_, int m_) throw(Exception)
{
  // filling trivial eigenvalues
  for(int i=0; i<m_; i++) 
  {
    eigenvalues[i] = 1.0 - A_[i][i];
  }

  // defining variables to perform eigenvalues functions
  gsl_matrix *K = gsl_matrix_alloc(m_, m_);
  gsl_vector_complex *vaps = gsl_vector_complex_alloc(m_);
  gsl_matrix_complex *VAPS = gsl_matrix_complex_alloc(m_, m_);
  gsl_matrix_complex *VEPS = gsl_matrix_complex_alloc(m_, m_);

  // filling deflated matrix
  for(int i=0; i<m_; i++) 
  {
    for(int j=0; j<m_; j++) 
    {
      double val = 0.0;
      if (i == j) 
      {
        val = 1.0 + (n_[j]-1)*A_[i][j];
      }
      else 
      {
        val = n_[j] * A_[i][j];
      }
      gsl_matrix_set(K, i, j, val);
    }
  }

  // computing eigenvalues for deflated matrix
  gsl_eigen_nonsymmv_workspace *W1 = gsl_eigen_nonsymmv_alloc(m_);
  int rc = gsl_eigen_nonsymmv(K, vaps, VEPS, W1);
  gsl_matrix_free(K);
  gsl_eigen_nonsymmv_free(W1);
  gsl_matrix_complex_free(VAPS);
  gsl_matrix_complex_free(VEPS);
  if (rc) {
    gsl_vector_complex_free(vaps);
    throw Exception("unable to compute eigenvalues: " + string(gsl_strerror(rc)));
  }

  // retrieving eigenvalues
  for(int i=0; i<m_; i++) {
    gsl_complex z = gsl_vector_complex_get(vaps, i);
    if (fabs(GSL_IMAG(z)) >= EPSILON) {
      gsl_vector_complex_free(vaps);
      throw Exception("imaginary eigenvalue computing eigenvalues");
    }
    else {
      eigenvalues[M+i] = GSL_REAL(z);
    }
  }
  gsl_vector_complex_free(vaps);
}

//===========================================================================
// returns the condition number (2-norm) of the Cholesky matrix
// if A = L*L' where L is the Cholesky matrix, then the condition number
// is sqrt(max(eig(A)),min(eig(A))
//===========================================================================
double ccruncher::BlockMatrixChol::getConditionNumber()
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
// returns the determinant of the Cholesky matrix
// observe that det(L) = sqrt(det(A))
// note: equivalent to the multiplication of all Cholesky diagonal values
//===========================================================================
double ccruncher::BlockMatrixChol::getDeterminant()
{
  double det = 1.0;

  for(int i=0; i<2*M; i++)
  {
    if (i < M)
    {
      if (n[i] > 1) det *= eigenvalues[i] * (n[i]-1.0);
    }
    else
    {
      det *= eigenvalues[i];
    }
  }

  return sqrt(det);
}
