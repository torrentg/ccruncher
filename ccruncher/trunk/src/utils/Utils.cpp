
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
// Utils.cpp - Utils code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cctype>
#include <cassert>
#include <algorithm>
#include "Utils.hpp"

//===========================================================================
// allocVectors
//===========================================================================
double * ccruncher::Utils::allocVector(int n, double val) throw(Exception)
{
  double *ret = NULL;

  try
  {
    ret = new double[n];

    for (int i=0;i<n;i++)
    {
      ret[i] = val;
    }

    return ret;
  }
  catch(std::exception &e)
  {
    throw Exception(e, "Utils::allocVector(): not enougth space");
  }
}

//===========================================================================
// allocVectors and initialize with x content
//===========================================================================
double * ccruncher::Utils::allocVector(int n, double *x) throw(Exception)
{
  double *ret = allocVector(n);

  for (int i=0;i<n;i++)
  {
    ret[i] = x[i];
  }

  return ret;
}

//===========================================================================
// allocMatrix
//===========================================================================
double ** ccruncher::Utils::allocMatrix(int n, int m, double val) throw(Exception)
{
  double **ret = NULL;

  try
  {
    ret = new double *[n];

    for(int i=0;i<n;i++)
    {
      ret[i] = NULL;
    }
  }
  catch(std::exception &e)
  {
    throw Exception(e, "Utils::allocMatrix(): not enougth space");
  }

  try
  {
    for(int i=0;i<n;i++)
    {
      ret[i] = new double[m];

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
    throw Exception(e, "Utils::allocMatrix(): not enougth space");
  }
}

//===========================================================================
// allocMatrix and initializes with x content (nxm array)
//===========================================================================
double ** ccruncher::Utils::allocMatrix(int n, int m, double *x) throw(Exception)
{
  double **ret = allocMatrix(n, m);

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
double ** ccruncher::Utils::allocMatrix(int n, int m, double **x) throw(Exception)
{
  double **ret = allocMatrix(n, m);

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
void ccruncher::Utils::deallocMatrix(double **x, int n)
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
// deallocVector
//===========================================================================
void ccruncher::Utils::deallocVector(double *x)
{
  if (x != NULL)
  {
    delete [] x;
    x = NULL;
  }
}

//===========================================================================
// tokenize
//===========================================================================
void ccruncher::Utils::tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
  {
     // Found a token, add it to the vector.
     tokens.push_back(str.substr(lastPos, pos - lastPos));
     // Skip delimiters.  Note the "not_of"
     lastPos = str.find_first_not_of(delimiters, pos);
     // Find next "non-delimiter"
     pos = str.find_first_of(delimiters, lastPos);
  }
}

//===========================================================================
// trim
//===========================================================================
string ccruncher::Utils::rtrim(string s)
{
  string::size_type pos = s.find_last_not_of(" \t\n");
  return s.substr( 0, pos+1 );
}

//===========================================================================
// trim
//===========================================================================
string ccruncher::Utils::ltrim(string s)
{
  string::size_type pos = s.find_first_not_of(" \t\n");
  return s.substr( pos, s.size()-pos );
}

//===========================================================================
// trim
//===========================================================================
string ccruncher::Utils::trim(string s)
{
  string::size_type lpos = s.find_first_not_of(" \t\n");
  string::size_type rpos = s.find_last_not_of(" \t\n");
  return s.substr( lpos, rpos-lpos+1 );
}

//===========================================================================
// uppercase
//===========================================================================
string ccruncher::Utils::uppercase(string str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), ccruncher::ltoupper);

  return res;
}

//===========================================================================
// uppercase
//===========================================================================
string ccruncher::Utils::lowercase(string str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), ccruncher::ltolower);

  return res;
}

//===========================================================================
// filler
//===========================================================================
string ccruncher::Utils::filler(int n, char c)
{
  return string(n, c);
}

//===========================================================================
// blanks
//===========================================================================
string ccruncher::Utils::blanks(int n)
{
  return filler(n, ' ');
}
