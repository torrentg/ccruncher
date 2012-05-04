
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
#include "math/BlockMatrixCholInv.hpp"
#include "utils/Arrays.hpp"
#include <cassert>

// --------------------------------------------------------------------------

// to access to Cholesky matrix coefficients
#define COEFS1(i,j) (H[(i)*N+(j)])
// to access to inverse matrix coefficients
#define COEFS2(i,j) (coefs[(i)*M+(j)])

//===========================================================================
// Description:
// ------------
// computes the inverse of a Cholesky matrix for block matrices
// described in paper 'Simulation of High-Dimensional t-Student Copula
// Copulas with a Given Block Matrix Correlation Matrix' by
// Gerard Torrent-Gironella and Josep Fortiana
//
//===========================================================================

//===========================================================================
// constructor
//===========================================================================
ccruncher::BlockMatrixCholInv::BlockMatrixCholInv(const BlockMatrixChol &L) :
    n(NULL), coefs(NULL), diag(NULL)
{
  M = L.M;
  N = L.N;
  n = Arrays<int>::allocVector(M, L.n);
  coefs = Arrays<double>::allocVector(N*M, 0.0);
  diag = Arrays<double>::allocVector(N, 0.0);
  invert(L.coefs, L.diag);
}

//===========================================================================
// invert the block Cholesky matrix
// H = Cholesky coefficients (size = MxN)
// y = Cholesky diagonal (size = N)
//===========================================================================
void ccruncher::BlockMatrixCholInv::invert(const double *H, const double *v)
{
  int i, j, r, q, s, t;
  double sum;

  for(i=-1, r=0; r<M; r++) for(q=0; q<n[r]; q++)
  {
    i++;
    diag[i] = 1.0/v[i];
  }

  for(i=0, r=0; r<M; r++)
  {
    for(j=i-1, s=r; s<M; s++)
    {
      sum = -COEFS1(s,i)*diag[i];
      for(t=i+1; t<j; t++)
      {
        sum -= COEFS1(s,t)*COEFS2(t,r);
      }

      for(q=0; q<n[s]; q++)
      {
        j++;
        if (j==i && s==r) continue;
        sum -= COEFS1(s,j-1)*COEFS2(j-1,r);
        COEFS2(j,r) = sum/v[j];
      }
    }
    i += n[r];
  }
}

//===========================================================================
// Destructor
//===========================================================================
ccruncher::BlockMatrixCholInv::~BlockMatrixCholInv()
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
}

//===========================================================================
// return the matrix dimension
//===========================================================================
int ccruncher::BlockMatrixCholInv::getDim() const
{
  return N;
}

//===========================================================================
// returns cholesky matrix multiplied by vector x
//===========================================================================
void ccruncher::BlockMatrixCholInv::mult(double *x, double *ret) const
{
  int i,j,r,q,num;
  double sum=0.0;

  for(num=0,r=0; r<k; r++) for(q=0; q<n[r]; q++)
  {
    ret[num] = diag[num]*x[num];
    num++;
  }

  for(i=-1,r=0; r<k; r++)
  {
    for(sum=0.0, q=0; q<n[r]; q++)
    {
      i++;
      if (q > 0)
      {
        ret[i] += sum*COEFS2(i,r);
      }
      sum += x[i];
    }
    for(j=i+1; j<num; j++)
    {
      ret[j] += sum*COEFS2(j,r);
    }
  }
}
