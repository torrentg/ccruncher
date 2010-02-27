
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#ifndef _Arrays_
#define _Arrays_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"
#include <cassert>

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

template <class T>
class Arrays
{

  private:

    // non-instantiable class
    Arrays() {};

  public:

    // alloc/dealloc array methods
    static T * allocVector(int n) throw(Exception);
    static T * allocVector(int n, T x) throw(Exception);
    static T * allocVector(int n, T *x) throw(Exception);
    static void deallocVector(T *x);

    // alloc/dealloc matrix methods
    static T ** allocMatrix(int n, int m) throw(Exception);
    static T ** allocMatrix(int n, int m, T x) throw(Exception);
    static T ** allocMatrix(int n, int m, T *x) throw(Exception);
    static T ** allocMatrix(int n, int m, T **x) throw(Exception);
    static void deallocMatrix(T **x, int n);

    // related methods
    static void prodMatrixVector(T **A, T *x, int n1, int n2, T *ret);
    static void prodMatrixMatrix(T **A, T **B, int n1, int n2, int n3, T **ret);
    static void copyVector(T *x, int n, T *ret);
    static void copyMatrix(T **x, int n, int m, T **ret);
    static void initVector(T *x, int n, T val);
    static void initMatrix(T **x, int n, int m, T val);

};

//---------------------------------------------------------------------------

//===========================================================================
// allocVector
//===========================================================================
template <class T>
T * ccruncher::Arrays<T>::allocVector(int n) throw(Exception)
{
  T aux = T();
  return allocVector(n, aux);
}

//===========================================================================
// allocVector
//===========================================================================
template <class T>
T * ccruncher::Arrays<T>::allocVector(int n, T val) throw(Exception)
{
  T *ret = NULL;

  try
  {
    ret = new T[n];

    for (int i=0;i<n;i++)
    {
      ret[i] = val;
    }

    return ret;
  }
  catch(std::exception &e)
  {
    throw Exception(e, "Arrays<T>::allocVector(): not enougth space");
  }
}

//===========================================================================
// allocVector
//===========================================================================
template <class T>
T * ccruncher::Arrays<T>::allocVector(int n, T *x) throw(Exception)
{
  T *ret = allocVector(n);

  for (int i=0;i<n;i++)
  {
    ret[i] = x[i];
  }

  return ret;
}

//===========================================================================
// deallocVector
//===========================================================================
template <class T>
void ccruncher::Arrays<T>::deallocVector(T *x)
{
  if (x != NULL)
  {
    delete [] x;
    x = NULL;
  }
}

//===========================================================================
// allocMatrix
//===========================================================================
template <class T>
T ** ccruncher::Arrays<T>::allocMatrix(int n, int m) throw(Exception)
{
  T aux = T();
  return allocMatrix(n, m, aux);
}

//===========================================================================
// allocMatrix
//===========================================================================
template <class T>
T ** ccruncher::Arrays<T>::allocMatrix(int n, int m, T val) throw(Exception)
{
  T **ret = NULL;

  try
  {
    ret = new T *[n];

    for(int i=0;i<n;i++)
    {
      ret[i] = NULL;
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, "Arrays<T>::allocMatrix(): not enougth space");
  }

  try
  {
    for(int i=0;i<n;i++)
    {
      ret[i] = new T[m];

      for(int j=0;j<m;j++)
      {
        ret[i][j] = val;
      }
    }

    return ret;
  }
  catch(std::exception &e)
  {
    deallocMatrix(ret, n);
    throw Exception(e, "Arrays<T>::allocMatrix(): not enougth space");
  }
}

//===========================================================================
// allocMatrix and initializes with x content (nxm array)
//===========================================================================
template <class T>
T ** ccruncher::Arrays<T>::allocMatrix(int n, int m, T *x) throw(Exception)
{
  T **ret = allocMatrix(n, m);

  for(int i=0;i<n;i++)
  {
    for(int j=0;j<m;j++)
    {
      ret[i][j] = x[j+m*i];
    }
  }

  return ret;
}

//===========================================================================
// allocMatrix and initializes with x content
//===========================================================================
template <class T>
T ** ccruncher::Arrays<T>::allocMatrix(int n, int m, T **x) throw(Exception)
{
  T **ret = allocMatrix(n, m);

  for(int i=0;i<n;i++)
  {
    for(int j=0;j<m;j++)
    {
      ret[i][j] = x[i][j];
    }
  }

  return ret;
}

//===========================================================================
// deallocMatrix
//===========================================================================
template <class T>
void ccruncher::Arrays<T>::deallocMatrix(T **x, int n)
{
  if (x != NULL)
  {
    for(int i=0;i<n;i++)
    {
      if (x[i] != NULL)
      {
        delete [] x[i];
        x[i] = NULL;
      }
    }
    delete [] x;
    x = NULL;
  }
}

//===========================================================================
// Product matriu per vector: ret = A.x (A is n1xn2, x is n2x1, ret is n1x1)
//===========================================================================
template <class T>
inline void ccruncher::Arrays<T>::prodMatrixVector(T **A, T *x, int n1, int n2, T *ret)
{
  // making assertions
  assert(n1 > 0); assert(n2 > 0);
  assert(A != NULL); assert(x != NULL); assert(ret != NULL);

  T aux;
  T *currentrow;

  for(register int i=0;i<n1;i++)
  {
    aux = 0.0;
    currentrow = A[i];

    for(register int j=0;j<n2;j++)
    {
      aux += currentrow[j]*x[j];
    }

    ret[i] = aux;
  }
}

//===========================================================================
// Matrix product: ret = A.B (A is a n1xn2, B is n2xn3 and ret is n1xn3)
//===========================================================================
template <class T>
inline void ccruncher::Arrays<T>::prodMatrixMatrix(T **A, T **B, int n1, int n2, int n3, T **ret)
{
  // making assertions
  assert(n1 > 0); assert(n2 > 0); assert(n3 > 0);
  assert(A != NULL); assert(B != NULL); assert(ret != NULL);

  T *aux1 = allocVector(n1);
  T *aux2 = allocVector(n2);

  for(register int i=0;i<n3;i++)
  {
    for(register int j=0;j<n2;j++)
    {
      aux2[j] = B[j][i];
    }

    prodMatrixVector(A, aux2, n1, n2, aux1);

    for(register int j=0;j<n1;j++)
    {
      ret[j][i] = aux1[j];
    }
  }

  deallocVector(aux1);
  deallocVector(aux2);
}

//===========================================================================
// copyVector
//===========================================================================
template <class T>
inline void ccruncher::Arrays<T>::copyVector(T *x, int n, T *ret)
{
  for(register int i=0;i<n;i++)
  {
    ret[i] = x[i];
  }
}

//===========================================================================
// copyMatrix
//===========================================================================
template <class T>
inline void ccruncher::Arrays<T>::copyMatrix(T **x, int n, int m, T **ret)
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
template <class T>
inline void ccruncher::Arrays<T>::initVector(T *x, int n, T val)
{
  for(register int i=0;i<n;i++)
  {
    x[i] = val;
  }
}

//===========================================================================
// initMatrix
//===========================================================================
template <class T>
inline void ccruncher::Arrays<T>::initMatrix(T **x, int n, int m, T val)
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
