
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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
#include <cassert>
#include "params/Transitions.hpp"
#include "utils/Format.hpp"
#include "utils/PowMatrix.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-14

/**************************************************************************/
ccruncher::Transitions::Transitions()
{
  mPeriod = -1;
  mIndexDefault = -1;
  mRegularizationError = 0.0;
}

/**************************************************************************//**
 * @param[in] ratings List of ratings.
 * @throw Exception Empty ratings list.
 */
ccruncher::Transitions::Transitions(const Ratings &ratings)
{
  setRatings(ratings);
  mPeriod = -1;
  mIndexDefault = -1;
  mRegularizationError = 0.0;
}

/**************************************************************************//**
 * @details Create a transition matrix providing matrix values as a whole.
 * @param[in] ratings List of ratings.
 * @param[in] matrix Matrix values.
 * @param[in] period Period (in months) covered by this matrix.
 * @throw Exception Error validating data.
 */
ccruncher::Transitions::Transitions(const Ratings &ratings,
   const std::vector<std::vector<double>> &matrix, int period)
{
  assert(period > 0);
  assert(ratings.size() == matrix.size());
  setRatings(ratings);
  mPeriod = period;
  mMatrix = matrix;
  mRegularizationError = 0.0;
  validate();
}

/**************************************************************************//**
 * @param[in] ratings List of ratings.
 * @throw Exception Void ratings list.
 */
void ccruncher::Transitions::setRatings(const Ratings &ratings)
{
  if (ratings.size() <= 0) {
    throw Exception("ratings not found");
  }

  mRatings = ratings;
  mMatrix.assign(size(), vector<double>(size(), NAN));
}

/**************************************************************************//**
 * @return Matrix dimension.
 */
size_t ccruncher::Transitions::size() const
{
  return mRatings.size();
}

/**************************************************************************//**
 * @return Period in months.
 */
int ccruncher::Transitions::getPeriod() const
{
  return mPeriod;
}

/**************************************************************************//**
 * @details Create a transition matrix providing matrix values as a whole.
 * @param[in] rating1 Starting rating identifier.
 * @param[in] rating2 Ending rating identifier.
 * @param[in] value Probability to migrate from rating1 to rating2 in period
 *                  months. Value in range [0,1].
 * @throw Exception Error validating data.
 */
void ccruncher::Transitions::insertTransition(const std::string &rating1,
    const std::string &rating2, double value)
{
  assert(size() > 0);

  size_t row = mRatings.indexOf(rating1);
  size_t col = mRatings.indexOf(rating2);

  // validating value
  if (value < -EPSILON || value > (1.0+EPSILON))
  {
    string msg = "transition[" + rating1 + "," + rating2 + "] is out of range: " +
                 Format::toString(value);
    throw Exception(msg);
  }

  // checking that it is not previously defined
  if (!std::isnan(mMatrix[row][col]))
  {
    string msg = "transition[" + rating1 + "," + rating2 + "] redefined";
    throw Exception(msg);
  }

  // insert value into matrix
  mMatrix[row][col] = value;
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Transitions::epstart(ExpatUserData &, const char *tag, const char **attributes)
{
  if (isEqual(tag,"transitions")) {
    if (getNumAttributes(attributes) != 1) {
      throw Exception("invalid number of attributes in tag transitions");
    }
    else {
      mPeriod = getIntAttribute(attributes, "period");
      if (mPeriod <= 0) throw Exception("attribute 'period' out of range");
    }
  }
  else if (isEqual(tag,"transition")) {
    string from = getStringAttribute(attributes, "from");
    string to = getStringAttribute(attributes, "to");
    double value = getDoubleAttribute(attributes, "value");
    insertTransition(from, to, value);
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 */
void ccruncher::Transitions::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"transitions"))
  {
    // non-informed elements are 0
    for(size_t i=0; i<size(); i++)
    {
      for(size_t j=0; j<size(); j++)
      {
        if (std::isnan(mMatrix[i][j]))
        {
          mMatrix[i][j] = 0.0;
        }
      }
    }

    validate();
  }
}

/**************************************************************************//**
 * @details The validation process includes to determine the index of
 *          default rating.
 * @throw Exception Transition matrix is invalid.
 */
void ccruncher::Transitions::validate()
{
  // checking that all rows sum 1
  for(size_t i=0; i<size(); i++)
  {
    double sum = 0.0;

    for(size_t j=0; j<size(); j++)
    {
      sum += mMatrix[i][j];
    }

    if (fabs(sum-1.0) > EPSILON)
    {
      throw Exception("row transition[" + mRatings[i].name + ",.] does not add up to 1");
    }
  }

  // searching default rating
  mIndexDefault = -1;

  for(size_t i=0; i<size(); i++)
  {
    if (fabs(mMatrix[i][i]-1.0) < EPSILON)
    {
      if (mIndexDefault < 0)
      {
        mIndexDefault = i;
      }
      else
      {
        throw Exception("found 2 or more default ratings");
      }
    }
  }
  if (mIndexDefault < 0)
  {
    throw Exception("default rating not found");
  }

  // TODO: check that any rating can be defaulted
  vector<bool> defaultable(size(), false);
  defaultable[mIndexDefault] = true;
  size_t num = 1;
  for(size_t n=0; n<size(); n++) {
    for(size_t i=0; i<size(); i++) {
      if (!defaultable[i]) {
        for(size_t j=0; j<size(); j++) {
          if (mMatrix[i][j] > EPSILON && defaultable[j]) {
            defaultable[i] = true;
            num++;
            break;
          }
        }
      }
    }
    if (num == size()) break;
  }
  if (num != size()) {
    throw Exception("transition matrix is not an absorbing Markov chain");
  }
}

/**************************************************************************//**
 * @return Index of default rating.
 */
int ccruncher::Transitions::getIndexDefault() const
{
  return mIndexDefault;
}

/**************************************************************************//**
 * @details Regularize the transition matrix using QOM (Quasi Optimizacion of
 *          the root Matrix). Algorithm extracted from paper:
 *          - title: 'Regularization Algorithms for Transition Matrices'
 *          - authors: Alezander Kreinin, Marina Sidelnikova
 *          - editor: Algo Research Quarterly, Vol. 4, Nos. 1/2, March/June 2001.
 *          Computes the regularization error using the sub-inf matrix norm.
 * @see http://en.wikipedia.org/wiki/Matrix_norm
 */
void ccruncher::Transitions::regularize()
{
  // computes the regularization error (sub-inf matrix norm)
  // note: regularized matrix has sub-inf norm = 1
  mRegularizationError = 0.0;
  for(size_t i=0; i<size(); i++)
  {
    double sum = 0.0;
    for(size_t j=0; j<size(); j++) {
      sum += fabs(mMatrix[i][j]);
    }
    double error = fabs(sum-1.0);
    if (error > mRegularizationError) {
      mRegularizationError = error;
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
        lambda += mMatrix[i][j];
      }
      lambda = (lambda-1.0)/(double)(size());
      for(size_t j=0; j<size(); j++)
      {
        if (mMatrix[i][j] != 0.0)
        {
          mMatrix[i][j] -= lambda;
        }
      }

      // step 2 + step 3'. checking termination criteria
      stop = true;
      for(size_t j=0; j<size(); j++)
      {
        if (mMatrix[i][j] < 0.0)
        {
          mMatrix[i][j] = 0.0;
          stop = false;
        }
      }
    }
  }
}

/**************************************************************************//**
 * @details Given the transition matrix for period T1, compute the transition
 *          matrix for a new period, t.
 * @param[in] t Period (in months) of the new transition matrix.
 * @return Transition matrix for period t.
 * @throw Exception Error scaling matrix.
 */
Transitions ccruncher::Transitions::scale(int t) const
{
  try
  {
    Transitions ret(*this);
    PowMatrix::pow(mMatrix, double(t)/double(getPeriod()), ret.mMatrix);
    ret.mPeriod = t;
    ret.regularize();
    return ret;
  }
  catch(Exception &e)
  {
    throw Exception(e, "invalid transition matrix");
  }
}

/**************************************************************************//**
 * @details Obtains the PD functions for each rating from the transition
 *          matrix doing M^t.
 * @see http://en.wikipedia.org/wiki/Matrix_norm
 * @param[in] numrows Number of steps (step size = period).
 * @param[out] ret Matrix of return values (ratings-PD).
 */
void ccruncher::Transitions::cdfr(size_t numrows, std::vector<std::vector<double>> &ret) const
{
  // making assertions
  assert(mIndexDefault >= 0);
  assert(numrows > 1);
  assert(numrows < 15000);
  assert(ret.size() == size());
  for(size_t i=0; i<size(); i++) {
    assert(ret[i].size() == numrows);
  }

  // building Id-matrix of size nxn
  vector<vector<double>> aux(size(), vector<double>(size(),0.0));
  for(size_t i=0; i<size(); i++) {
    aux[i][i] = 1.0;
  }

  // auxiliary matrix
  vector<vector<double>> tmp(size(), vector<double>(size(),0.0));

  // filling CDFR(.,0)
  for(size_t i=0; i<size(); i++)
  {
    ret[i][0] = aux[i][mIndexDefault];
  }

  // filling CDFR(.,t)
  for(size_t t=1; t<numrows; t++)
  {
    prod(aux, mMatrix, tmp);

    for(size_t i=0; i<size(); i++)
    {
      ret[i][t] = tmp[i][mIndexDefault];
    }

    for(size_t i=0; i<size(); i++) {
      for(size_t j=0; j<size(); j++) {
        aux[i][j] = tmp[i][j];
      }
    }
  }
}

/**************************************************************************//**
 * @details Use the transition matrix to infere the default probabilities
 *          curves computing the default probabilities at each month.
 * @param[in] date Starting date of this transition matrix.
 * @param[in] numrows Number of months to compute.
 * @return DefaultProbabilities related to this transition matrix.
 * @throw Exception Error creating DefaultProbabilities object.
 */
vector<CDF> ccruncher::Transitions::getCDFs(const Date &date, int numrows) const
{
  // computing CDFR
  vector<vector<double>> values(size(), vector<double>(numrows,NAN));
  cdfr(numrows, values);

  // creating dprobs function object
  vector<CDF> dprobs(size());
  for(int i=0; i<numrows; i++)
  {
    double t = add(date, i*mPeriod, 'M') - date;
    for(size_t irating=0; irating < mRatings.size(); irating++) {
      dprobs[irating].add(t, values[irating][i]);
    }
  }

  return dprobs;
}

/**************************************************************************//**
 * @see Transitions::regularize()
 * @return Regularization error using the sub-inf matrix norm.
 */
double ccruncher::Transitions::getRegularizationError() const
{
  return mRegularizationError;
}

/**************************************************************************//**
 * @param[in] M1 Left-hand matrix.
 * @param[in] M2 Right-hand matrix.
 * @param[out] M3 Result Product matrix (M1·M2).
 */
void ccruncher::Transitions::prod(const vector<vector<double>> &M1,
                const vector<vector<double>> &M2, vector<vector<double>> &M3)
{
  assert(M1.size() > 0 && M1[0].size() > 0);
  assert(M1.size() == M2.size() && M1[0].size() == M2[0].size());
  assert(M1.size() == M3.size() && M1[0].size() == M3[0].size());

  size_t n = M1.size();

  for(size_t i=0; i<n; i++)
  {
    for(size_t j=0; j<n; j++)
    {
      double val = 0.0;
      for(size_t k=0; k<n; k++)
      {
        val += M1[i][k]*M2[k][j];
      }
      M3[i][j] = val;
    }
  }
}

/**************************************************************************//**
 * @details Return the transition probabilities of the given rating index.
 * @param[in] row Rating index.
 * @return i-th row of the transition matrix.
 */
const vector<double>& ccruncher::Transitions::operator[] (int row) const
{
  assert(row >= 0 && row < (int)mMatrix.size());
  return mMatrix[row];
}

