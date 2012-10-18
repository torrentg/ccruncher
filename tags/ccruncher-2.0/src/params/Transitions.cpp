
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
#include "utils/Strings.hpp"
#include "math/PowMatrix.hpp"
#include <cassert>

#define EPSILON 1e-14

//===========================================================================
// default constructor
//===========================================================================
ccruncher::Transitions::Transitions()
{
  n = 0;
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
    throw Exception("invalid transition matrix dimension (" + Format::toString(n) + " <= 0)");
  }

  ratings = ratings_;
  n = ratings.size();
  matrix.assign(n, vector<double>(n, NAN));
}

//===========================================================================
// size
//===========================================================================
int ccruncher::Transitions::size() const
{
  return n;
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
  assert(n >= 0);

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
int ccruncher::Transitions::getIndexDefault() const
{
  return indexdefault;
}

//===========================================================================
// given a rating and a random number in [0,1] return final rating
// @param irating initial rating
// @param val random number in [0,1]
// @return final rating
//===========================================================================
int ccruncher::Transitions::evalue(const int irating, const double val) const
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
string ccruncher::Transitions::getXML(int ilevel) const throw(Exception)
{
  string spc1 = Strings::blanks(ilevel);
  string spc2 = Strings::blanks(ilevel+2);
  string ret = "";

  ret += spc1 + "<transitions period='" + Format::toString(period) + "' >\n";

  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      ret += spc2 + "<transition ";
      ret += "from='" + ratings.getName(i) + "' ";
      ret += "to='" + ratings.getName(j) + "' ";
      ret += "value='" + Format::toString(100.0*matrix[i][j]) + "%'";
      ret += "/>\n";
    }
  }

  ret += spc1 + "</transitions>\n";

  return ret;
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
void ccruncher::Transitions::cdfr(int steplength, int numrows, vector<vector<double> > &ret) const throw(Exception)
{
  // making assertions
  assert(numrows >= 0);
  assert(numrows < 15000);
  assert(steplength >= 0);
  assert(steplength < 15000);

  // building 1-year transition matrix
  Transitions tmone = scale(steplength);

  // building Id-matrix of size nxn
  vector<vector<double> > aux(n, vector<double>(n,0.0));
  for(int i=0; i<n; i++) {
    aux[i][i] = 1.0;
  }

  // auxiliary matrix
  vector<vector<double> > tmp(n, vector<double>(n,0.0));

  // filling CDFR(.,0)
  for(int i=0; i<n; i++)
  {
    ret[i][0] = aux[i][n-1];
  }

  // filling CDFR(.,t)
  for(int t=1; t<numrows; t++)
  {
    prod(aux, tmone.matrix, tmp);

    for(int i=0; i<n; i++)
    {
      ret[i][t] = tmp[i][n-1];
    }

    for(int i=0; i<n; i++) for(int j=0;j<n;j++) aux[i][j] = tmp[i][j];
  }
}

//===========================================================================
// returns the Survival Function (1-CDFR[i][j])
//===========================================================================
Survivals ccruncher::Transitions::getSurvivals(int steplength, int numrows) const throw(Exception)
{
  // computing CDFR
  vector<vector<double> > aux(n, vector<double>(numrows,NAN));
  cdfr(steplength, numrows, aux);

  // building survival function
  for(int i=0; i<n; i++)
  {
    for(int j=0; j<numrows; j++)
    {
      aux[i][j] = 1.0 - aux[i][j];
      if (aux[i][j] < 0.0) aux[i][j] = 0.0;
      if (aux[i][j] > 1.0) aux[i][j] = 1.0;
    }
  }

  // creating survival function object
  vector<int> itime(numrows);
  for(int i=0; i<numrows; i++) itime[i] = i;
  Survivals ret(ratings, itime, aux);
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

