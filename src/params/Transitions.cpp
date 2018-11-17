
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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
#include <limits>
#include <cassert>
#include "params/Transitions.hpp"
#include "utils/PowMatrix.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-14

/**************************************************************************//**
 * @details Create a transition matrix = 0 (invalid values).
 * @param[in] numRatings Number of ratings (dim = numRatings x numRatings).
 * @param[in] period Period (in months) covered by this matrix.
 * @throw Exception Invalid size or period.
 */
ccruncher::Transitions::Transitions(unsigned char numRatings, int period)
{
  if (numRatings == 0) {
    throw Exception("invalid matrix size");
  }

  if (period <= 0) {
    throw Exception("invalid period (in months)");
  }

  mPeriod = period;
  mIndexDefault = -1;
  mMatrix.assign(numRatings, vector<double>(numRatings, 0.0));
  isDirty = true;
}

/**************************************************************************//**
 * @details Create a transition matrix providing matrix values as a whole.
 * @param[in] matrix Matrix values.
 * @param[in] period Period (in months) covered by this matrix.
 * @throw Exception Error validating data.
 */
ccruncher::Transitions::Transitions(const std::vector<std::vector<double>> &matrix, int period)
{
  if (period <= 0) {
    throw Exception("invalid period (in months)");
  }

  mPeriod = period;
  mIndexDefault = -1;
  setValues(matrix);
}

/**************************************************************************//**
 * @return Matrix dimension.
 */
unsigned char ccruncher::Transitions::size() const
{
  return mMatrix.size();
}

/**************************************************************************//**
 * @return Period in months.
 */
int ccruncher::Transitions::getPeriod() const
{
  return mPeriod;
}

/**************************************************************************//**
 * @return Index of default rating.
 * @throw Exception Invalid transition matrix.
 */
unsigned char ccruncher::Transitions::getIndexDefault() const
{
  if (isDirty) {
    validate();
  }
  return (unsigned char) mIndexDefault;
}

/**************************************************************************//**
 * @details Return the transition probabilities of the given rating index.
 * @param[in] row Rating index.
 * @return i-th row of the transition matrix.
 */
const vector<double>& ccruncher::Transitions::operator[] (unsigned char row) const
{
  assert(row < size());
  return mMatrix[row];
}

/**************************************************************************//**
 * @param[in] matrix Transition values (row=from, col=to).
 * @throw Exception Invalid matrix.
 */
void ccruncher::Transitions::setValues(const std::vector<std::vector<double>> &matrix)
{
  if (matrix.size() == 0 || matrix.size() > numeric_limits<unsigned char>::max()) {
    throw Exception("invalid matrix size");
  }

  unsigned char n = static_cast<unsigned char>(matrix.size());

  for(auto row : matrix) {
    if (row.size() != n) {
      throw Exception("non-square matrix");
    }
  }

  mMatrix.assign(n, vector<double>(n, 0.0));

  for(unsigned char i=0; i<n; i++) {
    for(unsigned char j=0; j<n; j++) {
      setValue(i, j, matrix[i][j]);
    }
  }

  validate();
}

/**************************************************************************//**
 * @details Create a transition matrix providing matrix values as a whole.
 * @param[in] row Row index.
 * @param[in] col Column index.
 * @param[in] value Probability to migrate from rating1 to rating2.
 * @throw Exception Error validating value.
 */
void ccruncher::Transitions::setValue(unsigned char row, unsigned char col, double value)
{
  if (size() == 0 || row >= size() || col >= size()) {
    throw Exception("rating index out of range");
  }

  // validating value
  if (value < -EPSILON || value > (1.0+EPSILON)) {
    string msg = "transition out of range";
    throw Exception(msg);
  }

  // insert value into matrix
  mMatrix[row][col] = value;
  isDirty = true;
}

/**************************************************************************//**
 * @details The validation process includes to determine the index of
 *          default rating.
 * @throw Exception Transition matrix is invalid.
 */
void ccruncher::Transitions::validate() const
{
  // checking that all rows sum 1
  for(size_t i=0; i<size(); i++) {
    double sum = 0.0;

    for(size_t j=0; j<size(); j++) {
      sum += mMatrix[i][j];
    }

    if (fabs(sum-1.0) > EPSILON) {
      throw Exception(to_string(i+1) + "-th transition row does not add up to 1");
    }
  }

  // searching default rating
  mIndexDefault = -1;
  for(size_t i=0; i<size(); i++) {
    if (fabs(mMatrix[i][i]-1.0) < EPSILON) {
      if (mIndexDefault < 0) {
        mIndexDefault = i;
      }
      else {
        throw Exception("found 2 or more default ratings");
      }
    }
  }
  if (mIndexDefault < 0) {
    throw Exception("default rating not found");
  }

  // check that any rating can be defaulted
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

  isDirty = false;
}

/**************************************************************************//**
 * @details Regularize the transition matrix using QOM (Quasi Optimizacion of
 *          the root Matrix). Algorithm extracted from paper:
 *          - title: 'Regularization Algorithms for Transition Matrices'
 *          - authors: Alezander Kreinin, Marina Sidelnikova
 *          - editor: Algo Research Quarterly, Vol. 4, Nos. 1/2, March/June 2001.
 *          Computes the regularization error using the sub-inf matrix norm.
 * @see http://en.wikipedia.org/wiki/Matrix_norm
 * @throw Exception Matrix contains a row = 0.
 */
double ccruncher::Transitions::regularize()
{
  // computes the regularization error (sub-inf matrix norm)
  double regularizationError = 0.0;
  for(size_t i=0; i<size(); i++)
  {
    double sum = 0.0;
    for(size_t j=0; j<size(); j++) {
      sum += fabs(mMatrix[i][j]);
    }
    double error = fabs(sum-1.0);
    if (error > regularizationError) {
      regularizationError = error;
    }
  }

  // matrix regularization
  for(size_t i=0; i<size(); i++)
  {
    while(true)
    {
      // step 1. find the row projection on the hyperplane
      size_t num0s = 0;
      double lambda = 0.0;
      for(size_t j=0; j<size(); j++) {
        lambda += mMatrix[i][j];
        if (mMatrix[i][j] == 0.0) {
          num0s++;
        }
      }

      // checking that vector != 0
      if (num0s == size()) {
        throw Exception("invalid row (=0)");
      }

      lambda = (lambda-1.0)/(double)(size()-num0s);
      for(size_t j=0; j<size(); j++) {
        if (mMatrix[i][j] != 0.0) {
          mMatrix[i][j] -= lambda;
        }
      }

      // step 2 + step 3'. checking termination criteria
      bool stop = true;
      for(size_t j=0; j<size(); j++) {
        if (mMatrix[i][j] < 0.0) {
          mMatrix[i][j] = 0.0;
          stop = false;
        }
      }

      if (stop) {
        break;
      }
    }
  }

  return regularizationError;
}

/**************************************************************************//**
 * @details Given the transition matrix for period T1, compute the transition
 *          matrix for a new period, t.
 * @param[in] t Period (in months) of the new transition matrix.
 * @return Transition matrix for period t.
 * @throw Exception Invalid transition matrix or error scaling matrix.
 */
Transitions ccruncher::Transitions::scale(int t) const
{
  if (isDirty) {
    validate();
  }

  try
  {
    Transitions ret(*this);
    PowMatrix::pow(mMatrix, double(t)/double(getPeriod()), ret.mMatrix);
    ret.mPeriod = t;
    ret.regularize();
    ret.validate();
    return ret;
  }
  catch(Exception &e)
  {
    throw Exception(e, "error scaling transition matrix");
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
  for(size_t i=0; i<size(); i++) {
    ret[i][0] = aux[i][mIndexDefault];
  }

  // filling CDFR(.,t)
  for(size_t t=1; t<numrows; t++)
  {
    prod(aux, mMatrix, tmp);

    for(size_t i=0; i<size(); i++) {
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
 *          curves computing the default probabilities at each period.
 * @param[in] date Starting date of this transition matrix.
 * @param[in] numrows Number of months to compute.
 * @return List of CDF related to this transition matrix.
 * @throw Exception Error creating CDF object.
 */
vector<CDF> ccruncher::Transitions::getCDFs(const Date &date, int numrows) const
{
  if (isDirty) {
    validate();
  }

  // computing CDFR
  vector<vector<double>> values(size(), vector<double>(numrows,NAN));
  cdfr(numrows, values);

  // creating dprobs function object
  vector<CDF> dprobs(size());
  for(int i=0; i<numrows; i++) {
    double t = add(date, i*mPeriod, 'M') - date;
    for(size_t irating=0; irating < size(); irating++) {
      dprobs[irating].add(t, values[irating][i]);
    }
  }

  return dprobs;
}

/**************************************************************************//**
 * @param[in] M1 Left-hand matrix.
 * @param[in] M2 Right-hand matrix.
 * @param[out] M3 Result Product matrix (M1 x M2).
 */
void ccruncher::Transitions::prod(const vector<vector<double>> &M1,
                const vector<vector<double>> &M2, vector<vector<double>> &M3)
{
  assert(M1.size() > 0);
  assert(M1.size() == M2.size());
  assert(M1.size() == M3.size());

  size_t n = M1.size();

  for(size_t i=0; i<n; i++) {
    assert(M1[i].size() == n);
    assert(M2[i].size() == n);
    assert(M3[i].size() == n);
    for(size_t j=0; j<n; j++) {
      double val = 0.0;
      for(size_t k=0; k<n; k++) {
        val += M1[i][k]*M2[k][j];
      }
      M3[i][j] = val;
    }
  }
}

