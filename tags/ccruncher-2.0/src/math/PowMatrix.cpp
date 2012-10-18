
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
#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include "math/PowMatrix.hpp"
#include "utils/Format.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//---------------------------------------------------------------------------

using namespace std;

//===========================================================================
// return x^y
// the difference with std::pow is that allow negatives x
// example: std::pow(-8.0, 1/3)=nan, ccruncher::fpow(-8.0, 1/3)=-2
//===========================================================================
double ccruncher::fpow(double x, double y) throw(Exception)
{
  if (x >= 0.0)
  {
    return std::pow(x, y);
  }
  else
  {
    if (std::fabs(y) < EPSILON)
    {
       return 1.0; // x^0 = 1
    }
    else
    {
      double z = (std::fabs(y)>(1.0-EPSILON) ? y : 1.0/y);
      int aux = (int) z;

      if (((double) aux - z) < EPSILON)
      {
        return -std::pow(-x, y);
      }
      else
      {
        throw Exception("unable to pow this negative number: " + Format::toString(x) + "^" + Format::toString(y));
      }
    }
  }
}

//===========================================================================
// returns a matrix powered to an exponent: ret=a^x (where a is a nxn matrix)
// ret[0..n-1][0..n-1] = (a[0..n-1][0..n-1])^x
// atention: use with low dimensions (pe. n < 100)
// atention: Exception if complex eigenvalues
//===========================================================================
void ccruncher::PowMatrix::pow(double **a, double x, int n, double **ret) throw(Exception)
{
  int rc=0;

  // allocating memory
  gsl_matrix *M = gsl_matrix_alloc(n, n);
  gsl_vector_complex *vaps = gsl_vector_complex_alloc(n);
  gsl_matrix_complex *VAPS = gsl_matrix_complex_alloc(n, n);
  gsl_matrix_complex *VEPS = gsl_matrix_complex_alloc(n, n);

  // preparing matrix
  for(int i=0; i<n; i++) {
    for(int j=0; j<n; j++) {
      gsl_matrix_set(M, i, j, a[i][j]);
    }
  }

  // computing eigenvalues (vaps) and eigenvectors (VEPS)
  gsl_eigen_nonsymmv_workspace *W1 = gsl_eigen_nonsymmv_alloc(n);
  rc = gsl_eigen_nonsymmv(M, vaps, VEPS, W1);
  gsl_eigen_nonsymmv_free(W1);
  if (rc) {
    gsl_matrix_free(M);
    gsl_vector_complex_free(vaps);
    gsl_matrix_complex_free(VAPS);
    gsl_matrix_complex_free(VEPS);
    throw Exception("unable to compute eigenvalues: " + string(gsl_strerror(rc)));
  }
  gsl_matrix_free(M);

  // raising eigenvalues (vaps) to the x-th power (VAPS)
  gsl_matrix_complex_set_zero(VAPS);
  for(int i=0; i<n; i++) {
    // checks that eigenvalues are non-imaginary
    gsl_complex z = gsl_vector_complex_get(vaps, i);
    if (fabs(GSL_IMAG(z)) >= EPSILON) {
      gsl_vector_complex_free(vaps);
      gsl_matrix_complex_free(VAPS);
      gsl_matrix_complex_free(VEPS);
      throw Exception("can't pow matrix due to imaginary eigenvalue");
    }
    else {
      GSL_REAL(z) = ccruncher::fpow(GSL_REAL(z), x);
      GSL_IMAG(z) = 0.0;
      gsl_matrix_complex_set(VAPS, i, i, z);
    }
  }
  gsl_vector_complex_free(vaps);

  // computing the LU decomposition of eigenvectors (VEPS)
  int signum=0;
  gsl_matrix_complex *LU = gsl_matrix_complex_alloc(n, n);
  gsl_matrix_complex_memcpy(LU, VEPS);
  gsl_permutation *W2 = gsl_permutation_alloc(n);
  rc = gsl_linalg_complex_LU_decomp(LU, W2, &signum);
  if (rc) {
    gsl_matrix_complex_free(VAPS);
    gsl_matrix_complex_free(VEPS);
    gsl_matrix_complex_free(LU);
    gsl_permutation_free(W2);
    throw Exception("unable to inverse matrix: " + string(gsl_strerror(rc)));
  }

  // computing the inverse of eigenvectors (SPEV)
  gsl_matrix_complex *SPEV = gsl_matrix_complex_alloc(n, n);
  rc = gsl_linalg_complex_LU_invert(LU, W2, SPEV);
  gsl_permutation_free(W2);
  if (rc) {
    gsl_matrix_complex_free(VAPS);
    gsl_matrix_complex_free(VEPS);
    gsl_matrix_complex_free(SPEV);
    gsl_matrix_complex_free(LU);
    throw Exception("unable to inverse matrix: " + string(gsl_strerror(rc)));
  }

  // computing VEPS*VAPS*SPEV
  gsl_complex z0, z1;
  GSL_REAL(z0)=0.0; GSL_IMAG(z0) = 0.0;
  GSL_REAL(z1)=1.0; GSL_IMAG(z1) = 0.0;
  gsl_matrix_complex_set_zero(LU);
  gsl_blas_zgemm(CblasNoTrans, CblasNoTrans, z1, VEPS, VAPS, z0, LU  );
  gsl_matrix_complex_set_zero(VAPS);
  gsl_blas_zgemm(CblasNoTrans, CblasNoTrans, z1, LU  , SPEV, z0, VAPS);
  gsl_matrix_complex_free(VEPS);
  gsl_matrix_complex_free(SPEV);
  gsl_matrix_complex_free(LU);

  // preparing result
  for(int i=0; i<n; i++) {
    for(int j=0; j<n; j++) {
      ret[i][j] = GSL_REAL(gsl_matrix_complex_get(VAPS, i, j));
    }
  }
  gsl_matrix_complex_free(VAPS);
}

//===========================================================================
// matrix pow function (using vector instead of arrays)
//===========================================================================
void ccruncher::PowMatrix::pow(const vector<vector<double> > &a, double x, vector<vector<double> > &ret) throw(Exception)
{
  int n = a.size();
  double **A = new double*[n];
  double **C = new double*[n];

  for(int i=0; i<n; i++) {
    A[i] = (double *) &(a[i][0]);
    C[i] = &(ret[i][0]);
  }

  PowMatrix::pow(A, x, n, C);

  delete[] A;
  delete[] C;
}

