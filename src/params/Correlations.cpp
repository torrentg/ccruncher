
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
#include <gsl/gsl_linalg.h>
#include "params/Correlations.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] n Number of factors. Matrix dimension is nxn.
 */
ccruncher::Correlations::Correlations(size_t n)
{
  mMatrix.assign(n, vector<double>(n, NAN));
  for(size_t i=0; i<n; i++) {
    mMatrix[i][i] = 1.0;
  }
}

/**************************************************************************//**
 * @param[in] M Matrix values (M square matrix)
 * @exception Non-valid correlation matrix.
 */
ccruncher::Correlations::Correlations(const std::vector<std::vector<double>> &M)
{
  mMatrix = M;
  if (!isValid()) {
    throw Exception("invalid correlation matrix");
  }
}

/**************************************************************************//**
 * @details The correlation matrix is a square matrix nxn where n is
 *          number of factors.
 * @return The number of factors.
 */
size_t ccruncher::Correlations::size() const
{
  return mMatrix.size();
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] eu Xml parsing data.
 * @param[in] tag Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Correlations::epstart(ExpatUserData &eu, const char *tag, const char **attributes)
{
  if (isEqual(tag,"correlations")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("unexpected attributes in tag 'correlations'");
    }
  }
  else if (isEqual(tag,"correlation"))
  {
    string factor1 = getStringAttribute(attributes, "factor1");
    string factor2 = getStringAttribute(attributes, "factor2");
    double value = getDoubleAttribute(attributes, "value");

    // converting factors to indexes
    size_t row = eu.factors->indexOf(factor1);
    size_t col = eu.factors->indexOf(factor2);

    // checking for diagonal value
    if (row == col && value != 1.0) {
        throw Exception("diagonal value distrinct than 1");
    }

    // checking non-diagonal value range
    if (value < -1.0 || 1.0 < value) {
      string msg = "correlation value[" + factor1 + "," + factor2 + "] out of range [-1,+1]";
      throw Exception(msg);
    }

    // checking that value is not previously defined
    if (!std::isnan(mMatrix[row][col]) || !std::isnan(mMatrix[col][row])) {
      string msg = "correlation[" + factor1 + "," + factor2 + "] redefined";
      throw Exception(msg);
    }

    // inserting value into matrix
    mMatrix[row][col] = value;
    mMatrix[col][row] = value;
  }
  else {
    throw Exception("unexpected tag '" + string(tag) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] tag Element name.
 * @exception Invalid correlation matrix.
 */
void ccruncher::Correlations::epend(ExpatUserData &, const char *tag)
{
  if (isEqual(tag,"correlations")) {
    if (!isValid()) {
      throw Exception("invalid correlation matrix");
    }
  }
}

/**************************************************************************//**
 * @details Check that all matrix elements are set.
 * @return true=valid correlation matrix, false=invalid correlation matrix.
 */
bool ccruncher::Correlations::isValid()
{
  size_t n = size();

  // checking that has elements
  if (mMatrix.empty()) return false;

  // checking that is square
  for(size_t i=0; i<n; i++) {
    if (mMatrix[i].size() != n) {
      return false;
    }
  }

  // checking that values are in-range
  for(size_t i=0; i<n; i++) {
    for(size_t j=0; j<n; j++) {
      double value = mMatrix[i][j];
      if (std::isnan(value) || value < -1.0 || 1.0 < value) {
        return false;
      }
    }
  }

  // checking that is symmetric
  for(size_t i=0; i<n; i++) {
    for(size_t j=0; j<n; j++) {
      if (mMatrix[i][j] != mMatrix[j][i]) {
        return false;
      }
    }
  }

  // checking that is definite positive
  try {
    gsl_matrix *aux =  getCholesky();
    gsl_matrix_free(aux);
  }
  catch(Exception &e) {
    return false;
  }

  return true;
}

/**************************************************************************//**
 * @param[in] row Row index (0-based).
 * @return Row values.
 */
const vector<double>& ccruncher::Correlations::operator[] (size_t row) const
{
  assert(row < mMatrix.size());
  return mMatrix[row];
}

/**************************************************************************//**
 * @param[in] row Row index (0-based).
 * @return Row values.
 */
vector<double>& ccruncher::Correlations::operator[] (size_t row)
{
  assert(row < mMatrix.size());
  return mMatrix[row];
}

/**************************************************************************//**
 * @details Computes the Cholesky decomposition, L,  of the correlation
 *          matrix M. That is, M = L·L', where L is a lower triangular
 *          matrix.
 * @return Cholesky matrix.
 * @throw Exception Correlation matrix is not definite-posivite.
 */
gsl_matrix * ccruncher::Correlations::getCholesky() const
{
  assert(size() > 0);

  size_t n = size();
  gsl_matrix *chol = gsl_matrix_alloc(n, n);

  for(size_t i=0; i<n; i++)
  {
    gsl_matrix_set(chol, i, i, 1.0);

    for(size_t j=i+1; j<n; j++)
    {
      gsl_matrix_set(chol, i, j, mMatrix[i][j]);
      gsl_matrix_set(chol, j, i, mMatrix[i][j]);
    }
  }

  gsl_error_handler_t *eh = gsl_set_error_handler_off();
  int rc = gsl_linalg_cholesky_decomp(chol);
  gsl_set_error_handler(eh);
  if (rc != GSL_SUCCESS) {
    gsl_matrix_free(chol);
    throw Exception("correlation matrix non definite-positive");
  }

  return chol;
}

