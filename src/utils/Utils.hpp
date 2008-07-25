
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
// Utils.hpp - Utils header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _Utils_
#define _Utils_

//---------------------------------------------------------------------------

#include "config.h"
#include <cmath>
#include <string>
#include <vector>
#include "Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Utils
{

  public:

    static double * allocVector(int n, double x = NAN) throw(Exception);
    static double * allocVector(int n, double *x) throw(Exception);
    static void deallocVector(double *x);

    static double ** allocMatrix(int n, int m, double x = NAN) throw(Exception);
    static double ** allocMatrix(int n, int m, double *x) throw(Exception);
    static double ** allocMatrix(int n, int m, double **x) throw(Exception);
    static void deallocMatrix(double **x, int n);

    static void prodMatrixVector(double **matriu, double *vector, int n, int m, double *ret);
    static void copyVector(double *x, int n, double *ret);
    static void copyMatrix(double **x, int n, int m, double **ret);
    static void initVector(double *x, int n, double val);
    static void initMatrix(double **x, int n, int m, double val);

    static void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");
    static string rtrim(string s);
    static string ltrim(string s);
    static string trim(string s);
    static string uppercase(string str);
    static string lowercase(string str);
    static string blanks(int);
};

//---------------------------------------------------------------------------

//===========================================================================
// tolower manage int. Wrapper for manage chars
//===========================================================================
inline char ltolower (char ch) { return tolower(ch); }

//===========================================================================
// toupper manage int. Wrapper for manage chars
//===========================================================================
inline char ltoupper (char ch) { return toupper(ch); }

//===========================================================================
// Producte de matriu per vector = A.x
//===========================================================================
inline void Utils::prodMatrixVector(double **matriu, double *vector, int n, int m, double *ret)
{
  double aux;

  for(register int i=0;i<n;i++)
  {
    aux = 0.0;

    for(register int j=0;j<m;j++)
    {
      aux += matriu[i][j]*vector[j];
    }

    ret[i] = aux;
  }
}

//===========================================================================
// copyVector
//===========================================================================
inline void Utils::copyVector(double *x, int n, double *ret)
{
  for(register int i=0;i<n;i++)
  {
    ret[i] = x[i];
  }
}

//===========================================================================
// copyMatrix
//===========================================================================
inline void Utils::copyMatrix(double **x, int n, int m, double **ret)
{
  for(register int i=0;i<n;i++)
  {
    for(register int j=0;j<m;j++)
    {
      ret[i][j] = x[i][j];
    }
  }
}

//===========================================================================
// initVector
//===========================================================================
inline void Utils::initVector(double *x, int n, double val)
{
  for(register int i=0;i<n;i++)
  {
    x[i] = val;
  }
}

//===========================================================================
// initMatrix
//===========================================================================
inline void Utils::initMatrix(double **x, int n, int m, double val)
{
  for(register int i=0;i<n;i++)
  {
    for(register int j=0;j<m;j++)
    {
      x[i][j] = val;
    }
  }
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
