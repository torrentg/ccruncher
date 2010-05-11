
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
// PowMatrix.cpp - PowMatrix code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/06/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . minor changes
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed scope of pow(x,y) [ccruncher::PowMatrix:: -> ccruncher::]
//
// 2005/08/09 - Gerard Torrent [gerard@fobos.generacio.com]
//   . pow method renamed: pow(x,y) -> fpow(x,y)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <cmath>
#include <jama_eig.h>
#include <jama_lu.h>
#include "math/PowMatrix.hpp"
#include "utils/Format.hpp"

//---------------------------------------------------------------------------

#define EPSILON 1E-14

//---------------------------------------------------------------------------

using namespace std;
using namespace TNT;
using namespace JAMA;

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
        throw Exception("unable to pow this negative number: " + Format::double2string(x) + "^" + Format::double2string(y));
      }
    }
  }
}

//===========================================================================
// given a matrix, returns the inverse
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
    throw Exception(e, "unable to inverse matrix");
  }
  catch(...)
  {
    throw Exception("unable to inverse matrix");
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
  try
  {
    // auxiliar vector
    TNT::Array1D<double> V = TNT::Array1D<double>(n);
    // matrix declaration
    TNT::Array2D<double> M = TNT::Array2D<double>(n, n);
    // eigenvalues
    TNT::Array2D<double> VAPS = TNT::Array2D<double>(n, n);
    // eigenvectors
    TNT::Array2D<double> VEPS = TNT::Array2D<double>(n, n);

    // filling matrix
    for(int i=0;i<n;i++)
    {
      for(int j=0;j<n;j++)
      {
        M[i][j] = a[i][j];
      }
    }

    // computing eigenvalues and eigenvectors
    JAMA::Eigenvalue<double> eigen = JAMA::Eigenvalue<double>(M);

    // checking that eigenvalues aren't complex
    eigen.getImagEigenvalues(V);
    double sum = 0.0;
    for(int i=0;i<n;i++)
    {
      sum += fabs(V[i]);
    }
    if (sum >= EPSILON)
    {
      throw Exception("can't pow matrix due to imaginary eigenvalues");
    }

    // retrieving eigenvalues and eigenvectors
    eigen.getD(VAPS);
    eigen.getV(VEPS);

    // raising diagonal to the power of x
    for(int i=0;i<n;i++)
    {
       VAPS[i][i] = ccruncher::fpow(VAPS[i][i], x);
    }

    // finding eigenvectors inverse
    TNT::Array2D<double> SPEV = inverse(VEPS);

    // computing a^x
    TNT::Array2D<double> K = matmult(matmult(VEPS, VAPS), SPEV);

    // taking values and exit
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
    throw Exception(e, "unable to pow matrix");
  }
  catch(...)
  {
    throw Exception("unable to pow matrix");
  }
}