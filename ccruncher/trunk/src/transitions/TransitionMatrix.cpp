
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
//   . changed period time resolution (year->month)
//   . added steplength parameter at tma, tmaa and survival methods
//
// 2005/05/20 - Gerard Torrent [gerard@fobos.generacio.com]
//   . implemented Arrays class
//   . implemented Strings class
//
// 2005/05/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added property 4 check
//
//===========================================================================

#include <cmath>
#include <cfloat>
#include <cassert>
#include "transitions/TransitionMatrix.hpp"
#include "utils/Parser.hpp"
#include "utils/Arrays.hpp"
#include "utils/Strings.hpp"
#include "math/PowMatrix.hpp"

//===========================================================================
// private initializator
//===========================================================================
void ccruncher::TransitionMatrix::init(Ratings *ratings_) throw(Exception)
{
  period = 0;
  epsilon = -1.0;
  ratings = ratings_;

  n = ratings->getRatings()->size();

  if (n <= 0)
  {
    throw Exception("TransitionMatrix::init(): invalid matrix range");
  }

  // initializing matrix
  matrix = Arrays<double>::allocMatrix(n, n, NAN);
}

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::TransitionMatrix::TransitionMatrix(TransitionMatrix &otm) throw(Exception) : ExpatHandlers() 
{
  period = otm.period;
  ratings = otm.ratings;
  indexdefault = otm.indexdefault;
  epsilon = otm.epsilon;
  n = otm.n;

  // initializing matrix
  matrix = Arrays<double>::allocMatrix(n, n);
  Arrays<double>::copyMatrix(otm.getMatrix(), n, n, matrix);
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
  Arrays<double>::deallocMatrix(matrix, n);
}

//===========================================================================
// size
//===========================================================================
int ccruncher::TransitionMatrix::size()
{
  return n;
}

//===========================================================================
// getMatrix
//===========================================================================
double ** ccruncher::TransitionMatrix::getMatrix()
{
  return matrix;
}

//===========================================================================
// inserts an element into transition matrix 
//===========================================================================
void ccruncher::TransitionMatrix::insertTransition(const string &rating1, const string &rating2, double value) throw(Exception)
{
  int row = ratings->getIndex(rating1);
  int col = ratings->getIndex(rating2);

  // validating ratings
  if (row < 0 || col < 0)
  {
    string msg = "TransitionMatrix::insertTransition(): undefined rating at <transition> ";
    msg += rating1;
    msg += " -> ";
    msg += rating2;
    throw Exception(msg);
  }

  // validating value
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

  // checking that not exist 
  if (!isnan(matrix[row][col]))
  {
    string msg = "TransitionMatrix::insertTransition(): redefined element [";
    msg += rating1;
    msg += "][";
    msg += rating2;
    msg += "] in <mtransitions>";
    throw Exception(msg);
  }

  // insert value into matrix
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
      period = getIntAttribute(attributes, "period", INT_MAX);
      epsilon = getDoubleAttribute(attributes, "epsilon", 1e-12);
      if (period == INT_MAX || epsilon < 0.0 || epsilon > 1.0) {
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
// validate class content
//===========================================================================
void ccruncher::TransitionMatrix::validate() throw(Exception)
{
  // checking that all elements exists
  for (int i=0;i<n;i++)
  {
    for (int j=0;j<n;j++)
    {
      if (matrix[i][j] == NAN)
      {
        throw Exception("TransitionMatrix::validate(): transition matrix have an undefined element");
      }
    }
  }

  // checking that all rows sum 1
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

  // finding default rating
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
  if (indexdefault < 0)
  {
    throw Exception("TransitionMatrix::validate(): default rating not found");
  }

  // checking property 4 (all rating can be defaulted)
  for (int i=0;i<n;i++) 
  {
    bool bcon=false;

    for (int j=0;j<n;j++)
    {
      if (matrix[i][j] > epsilon && matrix[j][indexdefault] > epsilon)
      {
        bcon = true;
        break;
      }
    }
    
    if (bcon == false)
    {
      throw Exception("TransitionMatrix::validate(): property 4 not satisfied"); 
    }
  }
}

//===========================================================================
// return default rating index
//===========================================================================
int ccruncher::TransitionMatrix::getIndexDefault()
{
  return indexdefault;
}

//===========================================================================
// given a rating and a random number in [0,1] return final rating 
// @param irating initial rating
// @param val random number in [0,1]
// @return final rating
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
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<mtransitions period='" + Parser::int2string(period) + "' ";
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
// @param t period (in months) of the new transition matrix
// @return transition matrix for period t
//===========================================================================
TransitionMatrix * ccruncher::translate(TransitionMatrix *otm, int t) throw(Exception)
{
  TransitionMatrix *ret = new TransitionMatrix(*otm);

  PowMatrix::pow(otm->matrix, double(t)/double(otm->period), otm->n, ret->matrix);

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
void ccruncher::tma(TransitionMatrix *tm, int steplength, int numrows, double **ret) throw(Exception)
{
  // making assertions
  assert(numrows >= 0);
  assert(numrows < 15000);
  assert(steplength >= 0);
  assert(steplength < 15000);

  int n = tm->n;
  
  // building 1-year transition matrix
  TransitionMatrix *tmone = translate(tm, steplength);
  double **one = tmone->getMatrix();

  // building Id-matrix of size nxn
  double **aux = Arrays<double>::allocMatrix(n, n, 0.0);
  for(int i=0;i<n;i++)
  {
    aux[i][i] = 1.0;
  }

  // auxiliary matrix
  double **tmp = Arrays<double>::allocMatrix(n, n, 0.0);
      
  // filling TMAA(.,0)
  for(int i=0;i<n;i++)
  {
    ret[i][0] = aux[i][n-1];
  }
  
  for(int t=1;t<=numrows;t++)
  {
    Arrays<double>::prodMatrixMatrix(aux, one, n, n, n, tmp);

    // filling TMA(.,t)
    for(int i=0;i<n;i++)
    {
      ret[i][t] = tmp[i][n-1] - aux[i][n-1];
    }

    for(int i=0;i<n;i++) for(int j=0;j<n;j++) aux[i][j] = tmp[i][j];
  }
  
  // exit function
  Arrays<double>::deallocMatrix(one, n);
  Arrays<double>::deallocMatrix(aux, n);
  Arrays<double>::deallocMatrix(tmp, n);
}

//===========================================================================
// Given a transition matrix and the maximum number of years, return the 
// Cumulated Forward Default Rate in the ret matrix
// @param tm transition matrix
// @param numyears number of years (min=0, max=999)
// @param ret allocated space of size numratings x (numyears+1)
// ret[i][j] = probability initial rating r_i default between year 0 and year j
//===========================================================================
void ccruncher::tmaa(TransitionMatrix *tm, int steplength, int numrows, double **ret) throw(Exception)
{
  int n = tm->n;
  
  // computing TMA
  tma(tm, steplength, numrows, ret);
  
  // building acumulateds
  for(int i=0;i<n;i++)
  {
    for(int j=1;j<=numrows;j++)
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
void ccruncher::survival(TransitionMatrix *tm, int steplength, int numrows, double **ret) throw(Exception)
{
  int n = tm->n;
  
  // computing TMA
  tmaa(tm, steplength, numrows, ret);
  
  // building survival function
  for(int i=0;i<n;i++)
  {
    for(int j=1;j<=numrows;j++)
    {
      ret[i][j] = 1.0 - ret[i][j];
    }
  }
}
