
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
#include "params/Transitions.hpp"
#include "utils/Format.hpp"
#include "utils/PowMatrix.hpp"
#include <cassert>

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-14

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Transitions::Transitions()
{
  period = -1;
  indexdefault = -1;
  rerror = 0.0;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Transitions::Transitions(const Ratings &ratings_) throw(Exception)
{
  setRatings(ratings_);
  period = -1;
  indexdefault = -1;
  rerror = 0.0;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::Transitions::Transitions(const Ratings &ratings_, const vector<vector<double> > &matrix_, int period_) throw(Exception)
{
  assert(period_ > 0);
  assert(ratings_.size() == (int)matrix_.size());
  setRatings(ratings_);
  period = period_;
  matrix = matrix_;
  rerror = 0.0;
  validate();
}

//===========================================================================
// setRatings
//===========================================================================
void ccruncher::Transitions::setRatings(const Ratings &ratings_)
{
  if (ratings_.size() <= 0) {
    throw Exception("ratings not found");
  }

  ratings = ratings_;
  matrix.assign(size(), vector<double>(size(), NAN));
}

//===========================================================================
// size
//===========================================================================
size_t ccruncher::Transitions::size() const
{
  return ratings.size();
}

//===========================================================================
// getPeriod
//===========================================================================
int ccruncher::Transitions::getPeriod() const
{
  return period;
}

//===========================================================================
// inserts an element into transition matrix
//===========================================================================
void ccruncher::Transitions::insertTransition(const string &rating1, const string &rating2, double value) throw(Exception)
{
  assert(size() > 0);

  int row = ratings.getIndex(rating1);
  int col = ratings.getIndex(rating2);

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
    string msg = "redefined transition [" + rating1 + "][" + rating2 + "] in <transitions>";
    throw Exception(msg);
  }

  // insert value into matrix
  matrix[row][col] = value;
}

//===========================================================================
// epstart - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Transitions::epstart(ExpatUserData &, const char *name, const char **attributes)
{
  if (isEqual(name,"transitions")) {
    if (getNumAttributes(attributes) != 1) {
      throw Exception("invalid number of attributes in tag transitions");
    }
    else {
      period = getIntAttribute(attributes, "period");
      if (period <= 0) throw Exception("attribute 'period' out of range");
    }
  }
  else if (isEqual(name,"transition")) {
    string from = getStringAttribute(attributes, "from");
    string to = getStringAttribute(attributes, "to");
    double value = getDoubleAttribute(attributes, "value");
    insertTransition(from, to, value);
  }
  else {
    throw Exception("unexpected tag " + string(name));
  }
}

//===========================================================================
// epend - ExpatHandlers method implementation
//===========================================================================
void ccruncher::Transitions::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"transitions")) {
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
void ccruncher::Transitions::validate() throw(Exception)
{
  // checking that all elements exists
  for (size_t i=0; i<size(); i++)
  {
    for (size_t j=0; j<size(); j++)
    {
      if (isnan(matrix[i][j]))
      {
        throw Exception("transition matrix have an undefined element [" + Format::toString(i+1) + "][" + Format::toString(j+1) + "]");
      }
    }
  }

  // checking that all rows sum 1
  for (size_t i=0; i<size(); i++)
  {
    double sum = 0.0;

    for (size_t j=0; j<size(); j++)
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

  for (size_t i=0; i<size(); i++)
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
int ccruncher::Transitions::getIndexDefault() const
{
  return indexdefault;
}

//===========================================================================
// regularize the transition matrix using QOM (Quasi Optimizacion of the root Matrix)
// algorithm extracted from paper:
// title = 'Regularization Algorithms for Transition Matrices'
// authors = Alezander Kreinin, Marina Sidelnikova
// editor = Algo Research Quarterly, Vol. 4, Nos. 1/2, March/June 2001
//===========================================================================
void ccruncher::Transitions::regularize() throw(Exception)
{
  // computes the regularization error (sub-inf matrix norm)
  // note: regularized matrix has sub-inf norm = 1
  rerror = 0.0;
  for(size_t i=0; i<size(); i++)
  {
    double sum = 0.0;
    for(size_t j=0; j<size(); j++)
    {
      sum += abs(matrix[i][j]);
    }
    if (sum-1.0 > rerror)
    {
      rerror = sum-1.0;
    }
  }

  // matrix regularization
  for(size_t i=0; i<size(); i++)
  {
    bool stop = false;
    double lambda = 0.0;
    while(!stop || fabs(lambda)>1e-14)
    {
      // step 1. find the row projection on the hyperplane
      lambda = 0.0;
      for(size_t j=0; j<size(); j++)
      {
        lambda += matrix[i][j];
      }
      lambda = (lambda-1.0)/(double)(size());
      for(size_t j=0; j<size(); j++)
      {
        if (matrix[i][j] != 0.0)
        {
          matrix[i][j] -= lambda;
        }
      }

      // step 2 + step 3'. checking termination criteria
      stop = true;
      for(size_t j=0; j<size(); j++)
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
Transitions ccruncher::Transitions::scale(int t) const throw(Exception)
{
  try
  {
    Transitions ret(*this);
    PowMatrix::pow(matrix, double(t)/double(getPeriod()), ret.matrix);
    ret.period = t;
    ret.regularize();
    return ret;
  }
  catch(Exception &e)
  {
    throw Exception(e, "invalid transition matrix");
  }
}

//===========================================================================
// Given a transition matrix return the Cumulated Forward Default Rate
//===========================================================================
void ccruncher::Transitions::cdfr(size_t numrows, vector<vector<double> > &ret) const throw(Exception)
{
  // making assertions
  assert(indexdefault >= 0);
  assert(numrows > 1);
  assert(numrows < 15000);
  assert(ret.size() == size());
  for(size_t i=0; i<size(); i++) {
    assert(ret[i].size() == numrows);
  }

  // building Id-matrix of size nxn
  vector<vector<double> > aux(size(), vector<double>(size(),0.0));
  for(size_t i=0; i<size(); i++) {
    aux[i][i] = 1.0;
  }

  // auxiliary matrix
  vector<vector<double> > tmp(size(), vector<double>(size(),0.0));

  // filling CDFR(.,0)
  for(size_t i=0; i<size(); i++)
  {
    ret[i][0] = aux[i][indexdefault];
  }

  // filling CDFR(.,t)
  for(size_t t=1; t<numrows; t++)
  {
    prod(aux, matrix, tmp);

    for(size_t i=0; i<size(); i++)
    {
      ret[i][t] = tmp[i][indexdefault];
    }

    for(size_t i=0; i<size(); i++) for(size_t j=0; j<size(); j++) aux[i][j] = tmp[i][j];
  }
}

//===========================================================================
// computes default probabilities functions related to this transition matrix
//===========================================================================
DefaultProbabilities ccruncher::Transitions::getDefaultProbabilities(const Date &date, int numrows) const throw(Exception)
{
  // computing CDFR
  vector<vector<double> > values(size(), vector<double>(numrows,NAN));
  cdfr(numrows, values);

  // creating dprobs function object
  vector<Date> dates(numrows);
  for(int i=0; i<numrows; i++) dates[i] = add(date, i*period, 'M');
  DefaultProbabilities ret(ratings, date, dates, values);
  return ret;
}

//===========================================================================
// returns the regularization error (|non_regularized| - |regularized|)
//===========================================================================
double ccruncher::Transitions::getRegularizationError() const
{
  return rerror;
}

//===========================================================================
// matrix product (M3 = M1·M2)
//===========================================================================
void ccruncher::Transitions::prod(const vector<vector<double> > &M1, const vector<vector<double> > &M2, vector<vector<double> > &M3)
{
  assert(M1.size() > 0 && M1[0].size() > 0);
  assert(M1.size() == M2.size() && M1[0].size() == M2[0].size());
  assert(M1.size() == M3.size() && M1[0].size() == M3[0].size());

  unsigned int n = M1.size();

  for(unsigned int i=0; i<n; i++)
  {
    for(unsigned int j=0; j<n; j++)
    {
      double val = 0.0;
      for(unsigned int k=0; k<n; k++)
      {
        val += M1[i][k]*M2[k][j];
      }
      M3[i][j] = val;
    }
  }
}

//===========================================================================
// matrix element access
//===========================================================================
const vector<double>& ccruncher::Transitions::operator[] (int row) const
{
  assert(row >= 0 && row < (int)matrix.size());
  return matrix[row];
}

