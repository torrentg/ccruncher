
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
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
#include "utils/Format.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @param[in] factors_ List of defined factors.
 * @throw Exception List of factors is empty.
 */
ccruncher::Correlations::Correlations(const Factors &factors_)
{
  setFactors(factors_);
}

/**************************************************************************//**
 * @param[in] factors_ List of defined factors.
 * @throw Exception List of factors is empty.
 */
void ccruncher::Correlations::setFactors(const Factors &factors_)
{
  if (factors_.size() == 0) {
    throw Exception("factors not found");
  }
  else {
    factors = factors_;
    matrix.assign(size(), vector<double>(size(), NAN));
    for(int i=0; i<size(); i++) {
      matrix[i][i] = 1.0;
    }
  }
}

/**************************************************************************/
const Factors& ccruncher::Correlations::getFactors() const
{
  return factors;
}

/**************************************************************************//**
 * @details The correlation matrix is a square matrix nxn where n is
 *          number of factors.
 * @return The number of factors.
 */
int ccruncher::Correlations::size() const
{
  return factors.size();
}

/**************************************************************************//**
 * @details Decodes factor identifiers, check value range and inserts
 *          value in the matrix.
 * @param[in] factor1 Factor identifier.
 * @param[in] factor2 Factor identifier.
 * @param[in] value Correlation between factor1 and factor2.
 * @throw Exception Factor not found, repeated element, or invalid value.
 */
void ccruncher::Correlations::insertCorrelation(const std::string &factor1,
    const std::string &factor2, double value)
{
  int row = factors.indexOf(factor1);
  int col = factors.indexOf(factor2);

  // checking index factor
  if (row < 0 || col < 0)
  {
    string msg = "unknow factor [" + factor1 + "," + factor2 + "]";
    throw Exception(msg);
  }

  // checking for digonal value
  if (row == col)
  {
    if (value != 1.0) {
      throw Exception("diagonal value distrinct than 1");
    }
    else {
      return;
    }
  }

  // checking non-diagonal value range
  if (value < -1.0 || 1.0 < value)
  {
    string msg = "correlation value[" + factor1 + "," + factor2 + "] out of range [-1,+1]";
    throw Exception(msg);
  }

  // checking that value don't exist
  if (!std::isnan(matrix[row][col]) || !std::isnan(matrix[col][row]))
  {
    string msg = "correlation[" + factor1 + "," + factor2 + "] redefined";
    throw Exception(msg);
  }

  // inserting value into matrix
  matrix[row][col] = value;
  matrix[col][row] = value;
}

/**************************************************************************//**
 * @see ExpatHandlers::epstart
 * @param[in] name Element name.
 * @param[in] attributes Element attributes.
 * @throw Exception Error processing xml data.
 */
void ccruncher::Correlations::epstart(ExpatUserData &, const char *name, const char **attributes)
{
  if (isEqual(name,"correlations")) {
    if (getNumAttributes(attributes) != 0) {
      throw Exception("unexpected attributes in tag 'correlations'");
    }
  }
  else if (isEqual(name,"correlation")) {
    string factor1 = getStringAttribute(attributes, "factor1");
    string factor2 = getStringAttribute(attributes, "factor2");
    double value = getDoubleAttribute(attributes, "value");
    insertCorrelation(factor1, factor2, value);
  }
  else {
    throw Exception("unexpected tag '" + string(name) + "'");
  }
}

/**************************************************************************//**
 * @see ExpatHandlers::epend
 * @param[in] name Element name.
 */
void ccruncher::Correlations::epend(ExpatUserData &, const char *name)
{
  if (isEqual(name,"correlations")) {
    validate();
  }
}

/**************************************************************************//**
 * @details Check that all matrix elements are set.
 * @throw Exception Correlation element not defined.
 */
void ccruncher::Correlations::validate()
{
  // checking that all matrix elements exists
  for(int i=0; i<size(); i++)
  {
    for(int j=0; j<size(); j++)
    {
      if (std::isnan(matrix[i][j]))
      {
        throw Exception("correlation[" + factors[i].getName() +
            "," + factors[j].getName() + "] not defined");
      }
    }
  }
}

/**************************************************************************//**
 * @param[in] row Row index (0-based).
 * @return Row values.
 */
const vector<double>& ccruncher::Correlations::operator[] (int row) const
{
  assert(row >= 0 && row < (int)matrix.size());
  return matrix[row];
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

  int k = size();
  gsl_matrix *chol = gsl_matrix_alloc(k, k);

  for(int i=0; i<k; i++)
  {
    gsl_matrix_set(chol, i, i, 1.0);

    for(int j=i+1; j<k; j++)
    {
      gsl_matrix_set(chol, i, j, matrix[i][j]);
      gsl_matrix_set(chol, j, i, matrix[i][j]);
    }
  }

  gsl_error_handler_t *eh = gsl_set_error_handler_off();
  int rc = gsl_linalg_cholesky_decomp(chol);
  gsl_set_error_handler(eh);
  if (rc != GSL_SUCCESS) {
    throw Exception("correlation matrix non definite-positive");
  }

  return chol;
}

