
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
// CopulaNormal.cpp - CopulaNormal code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cmath>
#include <ctime>
#include <cfloat>
#include <cstdlib>
#include "Normal.hpp"
#include "CopulaNormal.hpp"
#include "CholeskyDecomposition.hpp"
#include "utils/Utils.hpp"

//---------------------------------------------------------------------------

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884197196
#endif

//===========================================================================
// init
//===========================================================================
void ccruncher::CopulaNormal::init()
{
  owner = false;
  aux1 = NULL;
  aux2 = NULL;
  sigmas = NULL;
}

//===========================================================================
// finalize
//===========================================================================
void ccruncher::CopulaNormal::finalize()
{
  if (owner)
  {
    Utils::deallocMatrix(sigmas, n);
  }

  Utils::deallocVector(aux1);
  Utils::deallocVector(aux2);
}

//===========================================================================
// constructor de copia
//===========================================================================
ccruncher::CopulaNormal::CopulaNormal(const CopulaNormal &x) throw(Exception)
{
  init();
  n = x.n;
  aux1 = Utils::allocVector(n);
  aux2 = Utils::allocVector(n);

  // definim sigmas
  owner = false;
  sigmas = x.sigmas;
  next();
}

//===========================================================================
// constructor
// modifica contingut de mcorrels
// es responsable d'alliberar mcorrels
//===========================================================================
ccruncher::CopulaNormal::CopulaNormal(int n_, double **mcorrels) throw(Exception)
{
  try
  {
    init();
    n = n_;
    aux1 = Utils::allocVector(n);
    aux2 = Utils::allocVector(n);

    // omplim sigmas
    owner = true;
    sigmas = mcorrels;
    correls2sigmas(mcorrels);
    next();
  }
  catch(Exception &e)
  {
    finalize();
    throw e;
  }
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::CopulaNormal::~CopulaNormal()
{
  finalize();
}

//===========================================================================
// size. returns number of components
//===========================================================================
int ccruncher::CopulaNormal::size()
{
  return n;
}

//===========================================================================
// transformacio
// @param val correlacio a satisfer
// @return sigma a acomplir
//===========================================================================
double ccruncher::CopulaNormal::transform(double val)
{
  return 2.0*sin(val*M_PI/6.0);
}

//===========================================================================
// Donada una matriu de correlacio observada retorna la matriu de taus-k
// adecuada per a la generacio de la copula normal
// destrueix la matriu correls per sigma
//===========================================================================
void ccruncher::CopulaNormal::correls2sigmas(double **correlations) throw(Exception)
{
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      sigmas[i][j] = transform(correlations[i][j]);
    }
  }

  bool def_pos = CholeskyDecomposition::choldc(sigmas, aux1, n);

  if (!def_pos)
  {
    throw Exception("CopulaNormal::correls2sigmas(): correlation matrix not definite positive");
  }
  else
  {
    for(int i=0;i<n;i++)
    {
      sigmas[i][i] = aux1[i];
    }

    for(int i=n-1;i>=0;i--)
    {
      for(int j=0;j<i;j++)
      {
        sigmas[j][i] = 0.0;
      }
    }
  }
}

//===========================================================================
// randNm
// omple el vector aux1 amb N(0,1) independents
// omple el vector aux2 amb N(0,1) correlacionades per sigmas
//===========================================================================
void ccruncher::CopulaNormal::randNm()
{
  for(int i=0;i<n;i++)
  {
    aux1[i] = mtrand.randNorm();
  }

  Utils::prodMatrixVector(sigmas, aux1, n, n, aux2);
}

//===========================================================================
// Retorna un vector aleatori on les marginals segueixen una distribucio
// uniforme [0,1] relacionades per una copula de tipus normal.
// sigma es una matriu nxn corresponent a la sigma de la normal emprada
//===========================================================================
void ccruncher::CopulaNormal::next()
{
  // generem v.a. amb distribucio N(0,sigmas) en aux2
  randNm();

  // posem en aux1 la copula generada
  for(int i=0;i<n;i++)
  {
    aux1[i] = Normal::cdf(aux2[i]);
    //aux1[i] = Normal::cdf(aux2[i], 0.0, sigmas[i][i]); //(les covars=1.0)
  }
}


//===========================================================================
// Retornem el component i de la copula en curs
// @param i component a recuperar
// @return valor de la component i de la copula simulada
//===========================================================================
double ccruncher::CopulaNormal::get(int i)
{
  if (i < 0 || i >= n)
  {
    return NAN;
  }
  else
  {
    return aux1[i];
  }
}

//===========================================================================
// Set new seed in the number generator
// @param x seed to be set
//===========================================================================
void ccruncher::CopulaNormal::setSeed(long k)
{
  mtrand.seed((const unsigned long) k);
}
