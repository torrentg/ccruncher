
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

#include <cmath>
#include <vector>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include "math/BlockMatrixChol.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define EPSILON 1E-12

//===========================================================================
// Description:
// ------------
// implements Cholesky decomposition matrix for a type of block matrix
//
// Cholesky algorithm description:
// -------------------------------
// Given a squared, simetric and definite positive matrix A, Cholesky
// algorithm computes L where A = L.L' and L lower triangular matrix
// You can found Cholesky algorithm explaned  at
// 'Numerical Recipes in C' (see http://www.nr.com)
//
// Problem:
// --------
// a 50.000 x 50.000 matrix requires too much memory and computations
// memory size:
//   50.000 x 50.000 matrix => 50.000 x 50.000 x 8 bytes / (1024*1024) = 19.073 Mb = 19 Gb RAM
// number of operations (matrix x vector):
//   50.000 x 50.000 matrix, 50.000 vector => 50.000 x 50.000 multiplications = 2.500.000.000 products
// size and computations have order N^2, where N is the matrix dimension
//
// Solution:
// ---------
// If the matrix is a block matrix, the cholesky decomposition have
// elements repeated and we can store only the distincts elements.
//
// example of block matrix:
//   1.0  0.5  0.5  0.5  0.1  0.1  0.1
//   0.5  1.0  0.5  0.5  0.1  0.1  0.1
//   0.5  0.5  1.0  0.5  0.1  0.1  0.1
//   0.5  0.5  0.5  1.0  0.1  0.1  0.1
//   0.1  0.1  0.1  0.1  1.0  0.3  0.3
//   0.1  0.1  0.1  0.1  0.3  1.0  0.3
//   0.1  0.1  0.1  0.1  0.3  0.3  1.0
//
// the previous matrix can be writen as (in submatrix form):
//   D1 A
//   A' D2
// where ' denote transposed and A is a matrix where all elements
// have the same value (0.1) and D1, D2 are square matrix where all
// elements have the same value (0.5 and 0.3 respectively) except
// in diagonal (diagonal elements allways are 1). Note that Dx
// matrix are squared, but the other submatrix not necesariely.
// Note that D1 and D2 can have distinct dimension (in the example,
// dim(D1)=4 and dim(D2)=3.
//
// the Cholesky decomposition is:
//   1.00000  0.00000  0.00000  0.00000  0.00000  0.00000  0.00000
//   0.50000  0.86603  0.00000  0.00000  0.00000  0.00000  0.00000
//   0.50000  0.28868  0.81650  0.00000  0.00000  0.00000  0.00000
//   0.50000  0.28868  0.20412  0.79057  0.00000  0.00000  0.00000
//   0.10000  0.05774  0.04082  0.03162  0.99197  0.00000  0.00000
//   0.10000  0.05774  0.04082  0.03162  0.28630  0.94975  0.00000
//   0.10000  0.05774  0.04082  0.03162  0.28630  0.21272  0.92563
//
// observe that each row have repeated elements. The algoritm implemented
// in this class store only M+1 elements per row (where M is the dimension of
// matrix of submatrix, 2 in the example).
//
// With this algorithm a 50.000 x 50.000 block matrix matrix, with 10 sectors have:
// memory size:
//   50.000 x 50.000 matrix => 50.000 x (10+1) x 8 bytes / (1024*1024) = 4.20 Mb RAM
// number of operations (matrix x vector):
//   50.000 x 50.000 matrix, 50.000 vector => 50.000 x (10+1) multiplications = 500.000 products
// size and computations have order N*(M+1), where N is the matrix dimension and M the number of sectors
//
// Notes:
// ------
// the assumption that diagonal is always 1 isn't a required condition. All
// previous explanation works with diagonal elements d1,d2,...,dn.
// This feature dirts the interface (force to add the diagonal values at
// constructor, etc). For this reason is suposed that all diagonal elements
// have value = 1.0 (the value that we need for solve borrowers correlation matrix
// factorization problem)
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
  spe = NULL;
  cnum = 0.0;
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
  spe = NULL;
  cnum = 0.0;
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
  coefs = Arrays<double>::allocMatrix(N, M, x.coefs);
  diag = Arrays<double>::allocVector(N, x.diag);
  spe = Arrays<int>::allocVector(N, x.spe);
  cnum = x.cnum;
}

//===========================================================================
// Constructor
// A: matrix with correlations between sectors (simmetric with size = mxm)
// n: number of elements in each sector (size = m)
// m: number of sectors
// Example: A = { {0.2,0.1}, {0.1, 0.3}}, n={2,3}, m=2
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
    // compute condition number of Cholesky matrix
    cnum = cond(A_, n_, m_);
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
      // compute condition number of Cholesky matrix
      cnum = cond(nA, nn, nm);

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
      throw e;
    }
  }
}

//===========================================================================
// init
//
// A: matrix of coeficients
// m: number of sectors
// n: dimension of each sector
//
// means that the input matrix is:
//
//   1   A11 ... A11         A1m   ...   A1m
//   A11  1  ... A11          .           .
//    .   .       .    ...    .           .
//    .   .       .           .           .
//    .   .       .           .           .
//   A11 A11 ...  1          A1m   ...   A1m
//         .        .               .
//         .          .             .
//         .            .           .
//         .              .         .
//         .                .       .
//   Am1   ...   Am1          1  Amm ... Amm
//    .           .          Amm  1  ... Amm
//    .           .           .   .       .
//    .           .    ...    .   .       .
//    .           .           .   .       .
//   A11   ...   Am1         Amm Amm ...  1
//
// Note: take care, we check that Aij = Aji
//
//===========================================================================
void ccruncher::BlockMatrixChol::init(double **A_, int *n_, int m_) throw(Exception)
{
  // initializations
  N = 0;
  n = NULL;
  coefs = NULL;
  diag = NULL;
  spe = NULL;

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
  coefs = Arrays<double>::allocMatrix(N, M, 0.0);

  // allocating memory for diagonal cholesky coeficients
  diag = Arrays<double>::allocVector(N, 0.0);

  // allocating memory for spe array
  spe = Arrays<int>::allocVector(N, 0);

  // filling spe (sector per element)
  for (int i=0,j=0;j<M;j++)
  {
    for(int k=0;k<n[j];k++)
    {
      spe[i] = j;
      i++;
    }
  }

  // call adapted Cholesky algorithm
  try
  {
    chold(A_);
  }
  catch(Exception &e)
  {
    reset();
    throw e;
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
    Arrays<double>::deallocMatrix(coefs, N);
    coefs = NULL;
  }

  // deallocating diagonal matrix coefs
  if (diag != NULL) {
    Arrays<double>::deallocVector(diag);
    diag = NULL;
  }

  // deallocating vector spe
  if (spe != NULL) {
    Arrays<int>::deallocVector(spe);
    spe = NULL;
  }

  // rest of values
  M = 0;
  N = 0;
  cnum = 0.0;
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
//===========================================================================
double ccruncher::BlockMatrixChol::get(int row, int col)
{
  assert(row >= 0 && row < N);
  assert(col >= 0 && col < N);

  if (col == row)
  {
    return diag[col];
  }
  else if (col < row)
  {
    return coefs[col][spe[row]];
  }
  else
  {
    return 0.0;
  }
}

//===========================================================================
// returns cholesky matrix multiplied by vector x
// puts result in vector ret (allocated by caller)
// does the multiplication with the minimum operations posible
//===========================================================================
void ccruncher::BlockMatrixChol::mult(double *x, double *ret)
{
  assert(x != NULL);
  assert(ret != NULL);

  // setting ret to 0
  for(int i=0;i<N;i++)
  {
    ret[i] = 0.0;
  }

  // computing the first element of each sector (diagonal product not included)
  for(int s=0,i=0;s<M;s++)
  {
    for(int j=0;j<i;j++)
    {
      ret[i] += get(i,j) * x[j];
    }
    i += n[s];
  }

  // computing the rest of elements for each sector (diagonal product not included)
  for(int s=0,i=0;s<M;s++)
  {
    i++;
    for(int k=1;k<n[s];k++)
    {
      ret[i] += ret[i-1] + get(i,i-1)*x[i-1];
      i++;
    }
  }

  // adding diagonal products
  for(int i=0;i<N;i++)
  {
    ret[i] += diag[i] * x[i];
  }
}

//===========================================================================
// adapted cholesky for block matrix (see header of this file)
// TODO: it is still posible to reduce the number of multiplications
//===========================================================================
void ccruncher::BlockMatrixChol::chold(double **A) throw(Exception)
{
  double sum;
  int iaux, jaux;

  // doing assertions
  assert(A != NULL);

  // filling cholesky coeficients
  for(int i=0;i<N;i++)
  {
    // retrieving sector of element i
    iaux = spe[i];

    // computing diagonal element
    {
      // we asume that A[i][i] = 1.0
      sum = 1.0 - aprod(i, i, i);

      // checking if is non-definite positive matrix
      if (sum <= 0.0)
      {
        throw Exception("non definite positive block matrix");
      }
      else
      {
        diag[i] = std::sqrt(sum);
      }
    }

    // exit if last element reached
    if (i == N-1)
    {
      break;
    }

    // computing [i][spe[i]] coeficient
    if (spe[i] == spe[i+1])
    {
      // retrieving A[i][j] value
      sum = A[iaux][iaux] - aprod(i, i+1, i);

      coefs[i][iaux] = sum/diag[i];
    }

    // move to next sector
    int j=i+1;
    for(int k=i+1;k<N;k++) {
      if (spe[k] != spe[i])
      {
        j = k;
        break;
      }
    }

    // computing [i][j] coeficients where j = spe[i]+1 ... M
    for(j=j;j<N;j=j+n[spe[j]]) //int j=i+n[spe[i]]
    {
      // retrieving sector
      jaux = spe[j];

      // retrieving A[i][j] value
      sum = A[iaux][jaux] - aprod(i, j, i);

      coefs[i][jaux] = sum/diag[i];
    }
  }
}

//===========================================================================
// internal function
// does product of cholesky row by cholesky col (only first order elements)
//===========================================================================
double ccruncher::BlockMatrixChol::aprod(int row, int col, int order)
{
  double ret;
  int sper, spec;

  // doing assertions
  assert(row >= 0 && row < N);
  assert(col >= 0 && col < N);
  assert(order >=0 && order < N);
  assert(row <= col);
  assert(order <= row);

  // initializing values
  ret = 0.0;
  sper = spe[row];
  spec = spe[col];

  // computing internal auto-prod
  for(int i=0;i<order;i++)
  {
    ret += coefs[i][sper] * coefs[i][spec];
  }

  // return value
  return ret;
}

//===========================================================================
// internal function
// computes cholesky matrix condition number (2-norm)
//
// if C is the cholesky decomposition of a matrix A composed by blocks
// then the condition number of C is computed as follows:
//   sqrt(max(eig(A)),min(eig(A))
//
// the (N_1+N_2+...+N_m) eigenvalues of A are:
//   * 1-A11 with multiplicity (N1)-1
//   * 1-A22 with multiplicity (N2)-1
//   *  ...
//   * 1-Amm with multiplicity (Nk)-1
//   * eig1 (see below)
//   * eig2 (see below)
//   *  ...
//   * eigm (see below)
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
double ccruncher::BlockMatrixChol::cond(double **A_, int *n_, int m_) throw(Exception)
{
  vector<double> eig = vector<double>();

  // filling direct eigenvalues
  for(int i=0; i<m_; i++) 
  {
    if (1 < n_[i])
    {
      double val = 1.0 - A_[i][i];
      eig.push_back(val);;
    }
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
    throw Exception("cond: unable to compute eigenvalues: " + string(gsl_strerror(rc)));
  }

  // retrieving eigenvalues
  for(int i=0; i<m_; i++) {
    gsl_complex z = gsl_vector_complex_get(vaps, i);
    if (fabs(GSL_IMAG(z)) >= EPSILON) {
      gsl_vector_complex_free(vaps);
      throw Exception("cond: imaginary eigenvalue computing eigenvalues");
    }
    else {
      double val = GSL_REAL(z);
      eig.push_back(val);
    }
  }
  gsl_vector_complex_free(vaps);

  // compute condition number
  double eigmin = +1e100;
  double eigmax = -1e100;
  for(int i=0; i<(int)eig.size(); i++) 
  {
    double val = std::fabs(eig[i]);
    if (val < eigmin) eigmin = val;
    if (eigmax < val) eigmax = val;
  }
  return std::sqrt(eigmax/eigmin);
}

//===========================================================================
// returns the condition number (2-norm) of the Cholesky matrix
//===========================================================================
double ccruncher::BlockMatrixChol:: getConditionNumber()
{
  return cnum;
}

