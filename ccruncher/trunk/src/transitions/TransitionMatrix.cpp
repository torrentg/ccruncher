
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
// TransitionMatrix.cpp - TransitionMatrix code
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/04/22 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added tma (Forward Default Rate) and tmaa (Cumulated Forward Default Rate)
//
// 2005/05/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added survival function (1-TMAA)
//
//===========================================================================

#include <cfloat>
#include <cassert>
#include "transitions/TransitionMatrix.hpp"
#include "utils/Parser.hpp"
#include "utils/Utils.hpp"
#include "math/PowMatrix.hpp"

//===========================================================================
// inicialitzador privat
//===========================================================================
void ccruncher::TransitionMatrix::init(Ratings *ratings_) throw(Exception)
{
  period = 0.0;
  epsilon = -1.0;
  ratings = ratings_;

  n = ratings->getRatings()->size();

  if (n <= 0)
  {
    throw Exception("TransitionMatrix::init(): invalid matrix range");
  }

  // inicialitzem la matriu
  matrix = Utils::allocMatrix(n, n, NAN);
}

//===========================================================================
// constructor de copia
//===========================================================================
ccruncher::TransitionMatrix::TransitionMatrix(TransitionMatrix &otm) throw(Exception) : ExpatHandlers() 
{
  period = otm.period;
  ratings = otm.ratings;
  indexdefault = otm.indexdefault;
  epsilon = otm.epsilon;
  n = otm.n;

  // inicialitzem la matriu
  matrix = Utils::allocMatrix(n, n);
  Utils::copyMatrix(otm.getMatrix(), n, n, matrix);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::TransitionMatrix::TransitionMatrix(Ratings *ratings_) throw(Exception)
{
  // posem valors per defecte
  init(ratings_);
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::TransitionMatrix::~TransitionMatrix()
{
  Utils::deallocMatrix(matrix, n);
}

//===========================================================================
// metodes acces variable begindate
//===========================================================================
int ccruncher::TransitionMatrix::size()
{
  return n;
}

//===========================================================================
// metodes acces variable begindate
//===========================================================================
double ** ccruncher::TransitionMatrix::getMatrix()
{
  return matrix;
}

//===========================================================================
// inserta un element en la matriu de transicio
//===========================================================================
void ccruncher::TransitionMatrix::insertTransition(const string &rating1, const string &rating2, double value) throw(Exception)
{
  int row = ratings->getIndex(rating1);
  int col = ratings->getIndex(rating2);

  // validem ratings entrats
  if (row < 0 || col < 0)
  {
    string msg = "TransitionMatrix::insertTransition(): undefined rating at <transition> ";
    msg += rating1;
    msg += " -> ";
    msg += rating2;
    throw Exception(msg);
  }

  // validem valor entrat
  if (value < -epsilon || value > (1.0 + epsilon))
  {
    string msg = "TransitionMatrix::insertTransition(): value[";
    msg += rating1;
    msg += "][";
    msg += rating2;
    msg += "] out of range: ";
    msg += Parser::double2string(value);
    throw Exception(msg);
  }

  // comprovem que no es trobi definit
  if (!isnan(matrix[row][col]))
  {
    string msg = "TransitionMatrix::insertTransition(): redefined element [";
    msg += rating1;
    msg += "][";
    msg += rating2;
    msg += "] in <mtransitions>";
    throw Exception(msg);
  }

  // inserim en la matriu de transicio
  matrix[row][col] = value;
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::TransitionMatrix::epstart(ExpatUserData &eu, const char *name, const char **attributes)
{
  if (isEqual(name,"mtransitions")) {
    if (getNumAttributes(attributes) < 1 || 2 < getNumAttributes(attributes)) {
      throw eperror(eu, "invalid number of attributes in tag mtransitions");
    }
    else {
      period = getDoubleAttribute(attributes, "period", DBL_MAX);
      epsilon = getDoubleAttribute(attributes, "epsilon", 1e-12);
      if (period == DBL_MAX || epsilon < 0.0 || epsilon > 1.0) {
        throw eperror(eu, "invalid attributes at <mtransitions>");
      }      
    }
  }
  else if (isEqual(name,"transition")) {
    string from = getStringAttribute(attributes, "from", "");
    string to = getStringAttribute(attributes, "to", "");
    double value = getDoubleAttribute(attributes, "value", DBL_MAX);

    if (from == "" || to == "" || value == DBL_MAX) {
      throw eperror(eu, "invalid values at <transition>");
    }
    else {
      insertTransition(from, to, value);
    }
  }
  else {
    throw eperror(eu, "unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::TransitionMatrix::epend(ExpatUserData &eu, const char *name)
{
  if (isEqual(name,"mtransitions")) {
    validate();
  }
  else if (isEqual(name,"transition")) {
    // nothing to do
  }
  else {
    throw eperror(eu, "unexpected end tag " + string(name));
  }
}

//===========================================================================
// validacio del contingut de la classe
//===========================================================================
void ccruncher::TransitionMatrix::validate() throw(Exception)
{
  // comprovem que tots els elements es troben definits
  for (int i=0;i<n;i++)
  {
    for (int j=0;j<n;j++)
    {
      if (matrix[i][j] == DBL_MAX)
      {
        throw Exception("TransitionMatrix::validate(): transition matrix have an undefined element");
      }
    }
  }

  // comprovem que totes les files sumen 1
  for (int i=0;i<n;i++)
  {
    double sum = 0.0;

    for (int j=0;j<n;j++)
    {
      sum += matrix[i][j];
    }

    if (sum < (1.0-epsilon) || sum > (1.0+epsilon))
    {
      throw Exception("TransitionMatrix::validate(): transition matrix row " + Parser::int2string(i+1) + " not sums 1");
    }
  }

  // determinem el rating de default
  indexdefault = -1;

  for (int i=0;i<n;i++)
  {
    if (matrix[i][i] > (1.0-epsilon) && matrix[i][i] < (1.0+epsilon))
    {
      if (indexdefault < 0)
      {
        indexdefault = i;
      }
      else
      {
        throw Exception("TransitionMatrix::validate(): found 2 or more default ratings");
      }
    }
  }

  // comprovem que rating default es troba definit
  if (indexdefault < 0)
  {
    throw Exception("TransitionMatrix::validate(): default rating not found");
  }
}

//===========================================================================
// retorna el index del rating default
//===========================================================================
int ccruncher::TransitionMatrix::getIndexDefault()
{
  return indexdefault;
}

//===========================================================================
// donat un rating inicial i un valor aleatori en [0,1] proporciona rating final
// @param x valor aleatori entre [0,1]
// @param rating rating inicial
// @return rating al final
//===========================================================================
int ccruncher::TransitionMatrix::evalue(const int irating, const double val)
{
  double sum = 0.0;

  for (int i=0;i<n;i++)
  {
    sum += matrix[irating][i];

    if (val < sum)
    {
      return i;
    }
  }

  return n-1;
}

//===========================================================================
// getXML
//===========================================================================
string ccruncher::TransitionMatrix::getXML(int ilevel) throw(Exception)
{
  string spc1 = Utils::blanks(ilevel);
  string spc2 = Utils::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<mtransitions period='" + Parser::double2string(period) + "' ";
  ret += "epsilon='" + Parser::double2string(epsilon) + "'>\n";

  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      ret += spc2 + "<transition ";
      ret += "from ='" + ratings->getName(i) + "' ";
      ret += "to ='" + ratings->getName(j) + "' ";
      ret += "value ='" + Parser::double2string(matrix[i][j]) + "'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</mtransitions>\n";

  return ret;
}

//===========================================================================
// given the transition matrix for time T1, compute the transition
// matrix for a new time, t
// @param otm transition matrix
// @param t period of the new transition matrix
// @return transition matrix for period t
//===========================================================================
TransitionMatrix * ccruncher::translate(TransitionMatrix *otm, double t) throw(Exception)
{
  TransitionMatrix *ret = new TransitionMatrix(*otm);

  PowMatrix::pow(otm->matrix, t/otm->period, otm->n, ret->matrix);

  ret->period = t;

  return ret;
}

//===========================================================================
// Given a transition matrix and the maximum number of years, return the 
// Forward Default Rate in the ret matrix
// @param tm transition matrix
// @param numyears number of years (min=0, max=999)
// @param ret allocated space of size numratings x (numyears+1)
// ret[i][j] = probability initial rating r_i default between year 0 and year j
//===========================================================================
void ccruncher::tma(TransitionMatrix *tm, int numyears, double **ret) throw(Exception)
{
  // making assertions
  assert(numyears >= 0);
  assert(numyears < 1000);

  int n = tm->n;
  
  // building 1-year transition matrix
  TransitionMatrix *tmone = translate(tm, 1.0);
  double **one = tmone->getMatrix();

  // building Id-matrix of size nxn
  double **aux = Utils::allocMatrix(n, n, 0.0);
  for(int i=0;i<n;i++)
  {
    aux[i][i] = 1.0;
  }

  // auxiliary matrix
  double **tmp = Utils::allocMatrix(n, n, 0.0);
      
  // filling TMAA(.,0)
  for(int i=0;i<n;i++)
  {
    ret[i][0] = aux[i][n-1];
  }
  
  for(int t=1;t<=numyears;t++)
  {
    Utils::prodMatrixMatrix(aux, one, n, n, n, tmp);

    // filling TMA(.,t)
    for(int i=0;i<n;i++)
    {
      ret[i][t] = tmp[i][n-1] - aux[i][n-1];
    }

    for(int i=0;i<n;i++) for(int j=0;j<n;j++) aux[i][j] = tmp[i][j];
  }
  
  // exit function
  delete [] one;
  Utils::deallocMatrix(aux, n);
  Utils::deallocMatrix(tmp, n);
}

//===========================================================================
// Given a transition matrix and the maximum number of years, return the 
// Cumulated Forward Default Rate in the ret matrix
// @param tm transition matrix
// @param numyears number of years (min=0, max=999)
// @param ret allocated space of size numratings x (numyears+1)
// ret[i][j] = probability initial rating r_i default between year 0 and year j
//===========================================================================
void ccruncher::tmaa(TransitionMatrix *tm, int numyears, double **ret) throw(Exception)
{
  int n = tm->n;
  
  // computing TMA
  tma(tm, numyears, ret);
  
  // building acumulateds
  for(int i=0;i<n;i++)
  {
    for(int j=1;j<=numyears;j++)
    {
      ret[i][j] = ret[i][j] + ret[i][j-1];
    }
  }
}

//===========================================================================
// Given a transition matrix and the maximum number of years, return the 
// Survival Function  in the ret matrix
// @param tm transition matrix
// @param numyears number of years (min=0, max=999)
// @param ret allocated space of size numratings x (numyears+1)
// ret[i][j] = 1-TMAA[i][j]
//===========================================================================
void ccruncher::survival(TransitionMatrix *tm, int numyears, double **ret) throw(Exception)
{
  int n = tm->n;
  
  // computing TMA
  tmaa(tm, numyears, ret);
  
  // building survival function
  for(int i=0;i<n;i++)
  {
    for(int j=1;j<=numyears;j++)
    {
      ret[i][j] = 1.0 - ret[i][j];
    }
  }
}
