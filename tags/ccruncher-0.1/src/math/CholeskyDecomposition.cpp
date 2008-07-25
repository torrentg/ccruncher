
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// CholeskyDecomposition.cpp - CholeskyDecomposition code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (Extracted from 'Numerical Recipes in C')
//
//===========================================================================

#include <cmath>
#include <iostream>
#include "CholeskyDecomposition.hpp"

//===========================================================================
// implements cholesky decomposition matrix
// A = L.L' (where L lower triangular matrix)
// @return false=KO (not definite positiva), true=OK
// Cholesky factor (L) is returned as upper triangle of given matrix
// except for this diagonal elements which are returned in p
// extracted from 'Numerical Recipes in C'
//===========================================================================
bool ccruncher::CholeskyDecomposition::choldc(double **a, double *p, int n)
{
  int i, j, k;
  double sum;

  for(i=0;i<n;i++)
  {
    for(j=i;j<n;j++)
    {
      for(sum=a[i][j],k=i-1;k>=0;k--)
      {
        sum -= a[i][k]*a[j][k];
      }

      if(i == j)
      {
        if(sum <= 0.0) 
        {
          return false;
        }
        else
        {
          p[i] = std::sqrt(sum);
        }
      }
      else 
      {
        a[j][i] = sum/p[i];
      }
    }
  }

  return true;
}
