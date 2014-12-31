
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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
#include <string>
#include <cassert>
#include <gsl/gsl_linalg.h>
#include "params/Correlations.hpp"

#define EPSILON 1e-14

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @details Creates an identity matrix of dimension dim x dim
 */
ccruncher::Correlations::Correlations(size_t dim)
{
  if (dim > 0) {
    this->assign(dim, vector<double>(dim, 0.0));
    for(size_t i=0; i<dim; i++) {
      (*this)[i][i] = 1.0;
    }
  }
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
    this->clear();
    assert(eu.factors != nullptr);
    size_t numFactors = eu.factors->size();
    assign(numFactors, vector<double>(numFactors, NAN));
    for(size_t i=0; i<numFactors; i++) {
      (*this)[i][i] = 1.0;
    }
  }
  else if (isEqual(tag,"correlation"))
  {
    string factor1 = getStringAttribute(attributes, "factor1");
    string factor2 = getStringAttribute(attributes, "factor2");
    double value = getDoubleAttribute(attributes, "value");

    // converting factors to indexes
    size_t row = eu.factors->indexOf(factor1);
    assert(row < this->size());
    size_t col = eu.factors->indexOf(factor2);
    assert(col < this->at(row).size());

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
    if (!std::isnan((*this)[row][col]) || !std::isnan((*this)[col][row])) {
      string msg = "correlation[" + factor1 + "," + factor2 + "] redefined";
      throw Exception(msg);
    }

    // inserting value into matrix
    (*this)[row][col] = value;
    (*this)[col][row] = value;
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
    isValid(*this, true);
  }
}

/**************************************************************************//**
 * @details Check that all matrix elements are set.
 * @param[in] M Correlatoin matrix values.
 * @param[in] throwException Throw an exception if validation fails.
 * @return true=valid correlation matrix, false=invalid correlation matrix.
 */
bool ccruncher::Correlations::isValid(const std::vector<std::vector<double>> &M, bool throwException)
{
  try
  {
    // checking that has elements
    if (M.empty()) {
      throw Exception("correlation matrix is empty");
    }

    // checking that is square
    for(size_t i=0; i<M.size(); i++) {
      if (M[i].size() != M.size()) {
        throw Exception("non-square correlation matrix");
      }
    }

    // checking that values are in-range
    for(size_t i=0; i<M.size(); i++) {
      if (std::abs(M[i][i]-1.0) > EPSILON) {
        throw Exception("correlation matrix with a diagonal element distinct than 1");
      }
      for(size_t j=0; j<M.size(); j++) {
        double value = M[i][j];
        if (std::isnan(value) || value < -1.0 || 1.0 < value) {
          throw Exception("value out of range [-1,+1]");
        }
      }
    }

    // checking that is symmetric
    for(size_t i=0; i<M.size(); i++) {
      for(size_t j=0; j<M.size(); j++) {
        if (std::abs(M[i][j]-M[j][i]) > EPSILON) {
          throw Exception("non-symmetric correlation matrix");
        }
      }
    }

    return true;
  }
  catch(Exception &)
  {
    if (throwException) throw;
    else return false;
  }
}

/**************************************************************************//**
 * @details Computes the Cholesky decomposition, L,  of the correlation
 *          matrix M. That is, M = L·L', where L is a lower triangular
 *          matrix.
 * @return Cholesky matrix.
 * @throw Exception Correlation matrix is not definite-posivite.
 */
gsl_matrix * ccruncher::Correlations::getCholesky(const std::vector<std::vector<double>> &M)
{
  assert(M.size() > 0);

  size_t n = M.size();
  gsl_matrix *chol = gsl_matrix_alloc(n, n);

  for(size_t i=0; i<n; i++) {
    gsl_matrix_set(chol, i, i, 1.0);
    for(size_t j=i+1; j<n; j++) {
      gsl_matrix_set(chol, i, j, M[i][j]);
      gsl_matrix_set(chol, j, i, M[i][j]);
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

