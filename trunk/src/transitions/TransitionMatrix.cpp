
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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
#include <cfloat>
#include <climits>
#include "transitions/TransitionMatrix.hpp"
#include "utils/Format.hpp"
#include "utils/Arrays.hpp"
#include "utils/Strings.hpp"
#include "math/PowMatrix.hpp"
#include <cassert>

#define EPSILON 1e-14

//===========================================================================
// default constructor
//===========================================================================
ccruncher::TransitionMatrix::TransitionMatrix()
{
  n = 0;
  ratings = NULL;
  period = -1;
  indexdefault = -1;
  matrix = NULL;
  rerror = 0.0;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::TransitionMatrix::TransitionMatrix(const Ratings &ratings_) throw(Exception)
{
  matrix = NULL;
  setRatings(ratings_);
  period = -1;
  indexdefault = -1;
  rerror = 0.0;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::TransitionMatrix::TransitionMatrix(const Ratings &ratings_, double ** matrix_, int period_) throw(Exception)
{
  matrix = NULL;
  setRatings(ratings_);
  period = period_;
  Arrays<double>::copyMatrix(matrix_, n, n, matrix);
  rerror = 0.0;
  validate();
}

//===========================================================================
// copy constructor
//===========================================================================
ccruncher::TransitionMatrix::TransitionMatrix(const TransitionMatrix &otm) throw(Exception) : ExpatHandlers()
{
  matrix = NULL;
  setRatings(*(otm.ratings));
  period = otm.period;
  indexdefault = otm.indexdefault;
  Arrays<double>::copyMatrix(otm.getMatrix(), n, n, matrix);
  rerror = 0.0;
}

//===========================================================================
// destructor
//===========================================================================
ccruncher::TransitionMatrix::~TransitionMatrix()
{
  assert(n >= 0);
  if (matrix != NULL) {
    Arrays<double>::deallocMatrix(matrix, n);
    matrix = NULL;
  }
}

//===========================================================================
// assignement operator
//===========================================================================
TransitionMatrix& ccruncher::TransitionMatrix::operator = (const TransitionMatrix &otm)
{
  if (this != &otm) // protect against invalid self-assignment
  {
    if (matrix != NULL) {
      Arrays<double>::deallocMatrix(matrix, n);
      matrix = NULL;
    }
    setRatings(*(otm.ratings));
    period = otm.period;
    indexdefault = otm.indexdefault;
    Arrays<double>::copyMatrix(otm.getMatrix(), n, n, matrix);
  }

  // by convention, always return *this
  return *this;
}

//===========================================================================
// setRatings
//===========================================================================
void ccruncher::TransitionMatrix::setRatings(const Ratings &ratings_)
{
  ratings = (Ratings *) &ratings_;
  n = ratings->size();
  if (n <= 0)
  {
    throw Exception("invalid transition matrix dimension (" + Format::toString(n) + " <= 0)");
  }
  if (matrix != NULL) {
    Arrays<double>::deallocMatrix(matrix, n);
    matrix = NULL;    
  }
  matrix = Arrays<double>::allocMatrix(n, n, NAN);
}

//===========================================================================
// size
//===========================================================================
int ccruncher::TransitionMatrix::size() const
{
  return n;
}

//===========================================================================
// getPeriod
//===========================================================================
int ccruncher::TransitionMatrix::getPeriod() const
{
  return period;
}

//===========================================================================
// getMatrix
//===========================================================================
double ** ccruncher::TransitionMatrix::getMatrix() const
{
  return matrix;
}

//===========================================================================
// inserts an element into transition matrix
//===========================================================================
void ccruncher::TransitionMatrix::insertTransition(const string &rating1, const string &rating2, double value) throw(Exception)
{
  assert(n >= 0);
  assert(matrix != NULL);

  int row = (*ratings).getIndex(rating1);
  int col = (*ratings).getIndex(rating2);

  // validating ratings
  if (row < 0 || col < 0)
  {
    throw Exception("undefined rating at <transition> " + rating1 + " -> " + rating2);
  }

  // validating value
  if (value < -EPSILON || value > (1.0+EPSILON))
  {
    string msg = " transition value[" + rating1 + "][" + rating2 + "] out of range: " + 
                 Format::toString(value);
    throw Exception(msg);
  }

  // checking that not exist
  if (!isnan(matrix[row][col]))
  {
    string msg = "redefined transition [" + rating1 + "][" + rating2 + "] in <mtransitions>";
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
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name,"mtransitions")) {
    if (getNumAttributes(attributes) != 1) {
      throw Exception("invalid number of attributes in tag mtransitions");
    }
    else {
      period = getIntAttribute(attributes, "period", INT_MAX);
      if (period == INT_MAX) {
        throw Exception("invalid period at <mtransitions>");
      }
    }
  }
  else if (isEqual(name,"transition")) {
    string from = getStringAttribute(attributes, "from", "");
    string to = getStringAttribute(attributes, "to", "");
    double value = getDoubleAttribute(attributes, "value", DBL_MAX);

    if (from == "" || to == "" || value == DBL_MAX) {
      throw Exception("invalid values at <transition>");
    }
    else {
      insertTransition(from, to, value);
    }
  }
  else {
    throw Exception("unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::TransitionMatrix::epend(ExpatUserData &eu, const char *name)
{
  assert(eu.getCurrentHandlers() != NULL);
  if (isEqual(name,"mtransitions")) {
    validate();
  }
  else if (isEqual(name,"transition")) {
    // nothing to do
  }
  else {
    throw Exception("unexpected end tag " + string(name));
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
      if (isnan(matrix[i][j]))
      {
        throw Exception("transition matrix have an undefined element [" + Format::toString(i+1) + "][" + Format::toString(j+1) + "]");
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

    if (sum < (1.0-EPSILON) || sum > (1.0+EPSILON))
    {
      throw Exception("transition matrix row " + Format::toString(i+1) + " does not add up to 1");
    }
  }

  // finding default rating
  indexdefault = -1;

  for (int i=0;i<n;i++)
  {
    if (matrix[i][i] > (1.0-EPSILON) && matrix[i][i] < (1.0+EPSILON))
    {
      if (indexdefault < 0)
      {
        indexdefault = i;
      }
      else
      {
        throw Exception("found 2 or more default ratings in transition matrix");
      }
    }
  }
  if (indexdefault < 0)
  {
    throw Exception("default rating not found");
  }

  // TODO: check that any rating can be defaulted
}

//===========================================================================
// return default rating index
//===========================================================================
int ccruncher::TransitionMatrix::getIndexDefault() const
{
  return indexdefault;
}

//===========================================================================
// given a rating and a random number in [0,1] return final rating
// @param irating initial rating
// @param val random number in [0,1]
// @return final rating
//===========================================================================
int ccruncher::TransitionMatrix::evalue(const int irating, const double val) const
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
string ccruncher::TransitionMatrix::getXML(int ilevel) const throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<mtransitions period='" + Format::toString(period) + "' >\n";

  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      ret += spc2 + "<transition ";
      ret += "from ='" + (*ratings)[i].name + "' ";
      ret += "to ='" + (*ratings)[j].name + "' ";
      ret += "value ='" + Format::toString(matrix[i][j]) + "'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</mtransitions>\n";

  return ret;
}

//===========================================================================
// regularize the transition matrix using QOM (Quasi Optimizacion of the root Matrix)
// algorithm extracted from paper:
// title = 'Regularization Algorithms for Transition Matrices'
// authors = Alezander Kreinin, Marina Sidelnikova
// editor = Algo Research Quarterly, Vol. 4, Nos. 1/2, March/June 2001
//===========================================================================
void ccruncher::TransitionMatrix::regularize() throw(Exception)
{

  // computes the regularization error (sub-inf matrix norm)
  // note: regularized matrix has sub-inf norm = 1
  rerror = 0.0;
  for(int i=0; i<n; i++)
  {
    double sum = 0.0;
    for(int j=0; j<n; j++)
    {
      sum += abs(matrix[i][j]);
    }
    if (sum-1.0 > rerror)
    {
      rerror = sum-1.0;
    }
  }

  // matrix regularization
  for(int i=0; i<n; i++)
  {
    bool stop = false;
    double lambda = 0.0;
    while(!stop || fabs(lambda)>1e-14)
    {
      // step 1. find the row projection on the hyperplane
      lambda = 0.0;
      for(int j=0; j<n; j++)
      {
        lambda += matrix[i][j];
      }
      lambda = (lambda-1.0)/(double)(n);
      for(int j=0; j<n; j++)
      {
        if (matrix[i][j] != 0.0)
        {
          matrix[i][j] -= lambda;
        }
      }

      // step 2 + step 3'. checking termination criteria
      stop = true;
      for(int j=0; j<n; j++)
      {
        if (matrix[i][j] < 0.0) 
        {
          matrix[i][j] = 0.0;
          stop = false;
        }
      }
    }
  }
}

//===========================================================================
// given the transition matrix for time T1, compute the transition
// matrix for a new time, t
// @param t period (in months) of the new transition matrix
// @return transition matrix for period t
//===========================================================================
TransitionMatrix ccruncher::TransitionMatrix::scale(int t) const throw(Exception)
{
  TransitionMatrix ret(*this);
  PowMatrix::pow(getMatrix(), double(t)/double(getPeriod()), size(), ret.matrix);
  ret.period = t;
  ret.regularize();
  return ret;
}

//===========================================================================
// Given a transition matrix return the Cumulated Forward Default Rate
//===========================================================================
void ccruncher::TransitionMatrix::cdfr(int steplength, int numrows, double **ret) const throw(Exception)
{
  // making assertions
  assert(numrows >= 0);
  assert(numrows < 15000);
  assert(steplength >= 0);
  assert(steplength < 15000);

  // building 1-year transition matrix
  TransitionMatrix tmone = scale(steplength);
  double **one = tmone.getMatrix();

  // building Id-matrix of size nxn
  double **aux = Arrays<double>::allocMatrix(n, n, 0.0);
  for(int i=0;i<n;i++)
  {
    aux[i][i] = 1.0;
  }

  // auxiliary matrix
  double **tmp = Arrays<double>::allocMatrix(n, n, 0.0);

  // filling CDFR(.,0)
  for(int i=0;i<n;i++)
  {
    ret[i][0] = aux[i][n-1];
  }

  // filling CDFR(.,t)
  for(int t=1;t<numrows;t++)
  {
    Arrays<double>::prodMatrixMatrix(aux, one, n, n, n, tmp);

    for(int i=0;i<n;i++)
    {
      ret[i][t] = tmp[i][n-1];
    }

    for(int i=0;i<n;i++) for(int j=0;j<n;j++) aux[i][j] = tmp[i][j];
  }

  // exit function
  Arrays<double>::deallocMatrix(aux, n);
  Arrays<double>::deallocMatrix(tmp, n);
}

//===========================================================================
// returns the Survival Function (1-CDFR[i][j])
//===========================================================================
Survival ccruncher::TransitionMatrix::getSurvival(int steplength, int numrows) const throw(Exception)
{
  // memory allocation
  double **aux = Arrays<double>::allocMatrix(n, numrows);
  int *itime = Arrays<int>::allocVector(numrows);
  for (int i=0;i<numrows;i++) {
    itime[i] = i;
  }

  // computing CDFR
  cdfr(steplength, numrows, aux);

  // building survival function
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<numrows;j++)
    {
      aux[i][j] = 1.0 - aux[i][j];
      if (aux[i][j] < 0.0) aux[i][j] = 0.0;
      if (aux[i][j] > 1.0) aux[i][j] = 1.0;
    }
  }

  // creating survival function object
  Survival ret(*ratings, numrows, itime, aux);
  Arrays<double>::deallocMatrix(aux, n);
  Arrays<int>::deallocVector(itime);
  return ret; 
}

//===========================================================================
// returns the regularization error (|non_regularized| - |regularized|)
//===========================================================================
double  ccruncher::TransitionMatrix::getRegularizationError() const
{
  return rerror;
}

