
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
// PowMatrix.cpp - PowMatrix code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include "jama_eig.h"
#include "jama_lu.h"
#include "PowMatrix.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//---------------------------------------------------------------------------

using namespace TNT;
using namespace JAMA;

//===========================================================================
// return x^y
// the difference with std::pow is that allow negatives x
// example: std::pow(-8.0, 1/3)=nan, PowMatrixx::pow(-8.0, 1/3)=-2
//===========================================================================
double PowMatrix::pow(double x, double y) throw(Exception)
{
  if (x >= 0.0)
  {
    return std::pow(x, y);
  }
  else
  {
    if (std::fabs(y) < EPSILON)
    {
       return 1.0; // x^0 = 0
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
        throw Exception("PowMatrix::pow(): unable to pow this negative number");
      }
    }
  }
}

//===========================================================================
// retorna ret[0..n-1][0..n-1] = a[0..n-][0..n-1]^x
// atencio: nomes dimensions petites (pe. n < 100)
// atencio: Exception si VAPS imaginaris
//===========================================================================
Array2D<double> ccruncher::PowMatrix::inverse(Array2D<double> &x) throw(Exception)
{
  try
  {
    int n = x.dim1();
    Array2D<double> Id = Array2D<double>(n, n, 0.0);
    
    for(int i=0;i<n;i++) 
    {
      Id[i][i] = 1.0;
    }

    LU<double> lu = LU<double>(x);

    return lu.solve(Id);  
  }
  catch(std::exception &e)
  {
    throw Exception(e, "PowMatrix::inverse(): unable to inverse matrix");
  }
  catch(...)
  {
    throw Exception("PowMatrix::inverse(): unable to inverse matrix");
  }
}

//===========================================================================
// retorna ret[0..n-1][0..n-1] = a[0..n-][0..n-1]^x
// atencio: nomes dimensions petites (pe. n < 100)
// atencio: Exception si VAPS imaginaris
//===========================================================================
void ccruncher::PowMatrix::pow(double **a, double x, int n, double **ret) throw(Exception)
{
  try
  {
    // vector auxiliar
    TNT::Array1D<double> V = TNT::Array1D<double>(n);    
    // declarem la matriu
    TNT::Array2D<double> M = TNT::Array2D<double>(n, n);
    // VAPS
    TNT::Array2D<double> VAPS = TNT::Array2D<double>(n, n);
    // VEPS
    TNT::Array2D<double> VEPS = TNT::Array2D<double>(n, n);

    // omplim la matriu original
    for(int i=0;i<n;i++)
    {
      for(int j=0;j<n;j++)
      {
        M[i][j] = a[i][j];
      }
    }

    // diagonalitzacio de la matriu (obtencio de VAPS i VEPS)  
    JAMA::Eigenvalue<double> eigen = JAMA::Eigenvalue<double>(M);

    // comprovem que no existeix part imaginaria
    eigen.getImagEigenvalues(V);
    double sum = 0.0;
    for(int i=0;i<n;i++) 
    {
      sum += fabs(V[i]);
    }
    if (sum >= EPSILON) 
    {
      throw Exception("PowMatrix::pow(): imag eigenvalues");
    }

    // recollim els VAPS i VEPS
    eigen.getD(VAPS);    
    eigen.getV(VEPS);

    // elevem la diagonal a x
    for(int i=0;i<n;i++) 
    {
       if (VAPS[i][i] < EPSILON)
       {
         VAPS[i][i] = -std::pow(std::fabs(VAPS[i][i]), x);
       }
       else
       {
         VAPS[i][i] = pow(VAPS[i][i], x);
       }
    }

    // trobem la inversa de VEPS
    TNT::Array2D<double> SPEV = inverse(VEPS);

    // calculem la matriu^x
    TNT::Array2D<double> K = matmult(matmult(VEPS, VAPS), SPEV);

    // recollim i sortim
    for(int i=0;i<n;i++)
    { 
      for(int j=0;j<n;j++)
      {
        ret[i][j] = K[i][j];
      }
    }          
  }
  catch(Exception &e)
  {
    throw e;
  }    
  catch(std::exception &e)
  {
    throw Exception(e, "PowMatrix::pow(): unable to pow matrix");
  }    
  catch(...)
  {
    throw Exception("PowMatrix::pow(): unable to pow matrix");
  }    
}
