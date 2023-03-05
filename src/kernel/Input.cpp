
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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
#include "kernel/Input.hpp"
#include "utils/Exception.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1e-12

/**************************************************************************//**
 * @param[in] dprobs List of CDFs to check.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Validation error.
 */
bool ccruncher::Input::validateCDFs(const vector<CDF> &dprobs, bool throwException)
{
  try
  {
    if (dprobs.size() < 2 || dprobs.size() > numeric_limits<unsigned char>::max()) {
      throw Exception("invalid number of ratings");
    }

    unsigned char idefault = indexOfDefaultRating(dprobs);

    // finding empty cdf's or with range bad-defined
    for(size_t i=0; i<dprobs.size(); i++)
    {
      if (i != idefault && dprobs[i].evalue(0.0) > EPSILON) {
        throw Exception("cdf with dprob(0) > 0");
      }
      if (dprobs[i].getPoints().back().second < EPSILON) {
        throw Exception("cdf with dprob(x) = 0 for all x");
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
 * @details Check that all matrix elements are set.
 * @param[in] M Correlation matrix values.
 * @param[in] throwException Throw an exception if validation fails.
 * @return true=valid correlation matrix, false=invalid correlation matrix.
 */
bool ccruncher::Input::validateCorrelations(const vector<vector<double>> &M, bool throwException)
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
 * @param[in] factors List of factors.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Invalid number of factors, or duplicated factor name,
 *        or factor loadings out of range.
 */
bool ccruncher::Input::validateFactors(const vector<Factor> &factors, bool throwException)
{
  try
  {
    if (factors.empty()) {
      throw Exception("'factors' have no elements");
    }
    if (factors.size() > numeric_limits<unsigned char>::max()) {
      int maxval = numeric_limits<unsigned char>::max();
      throw Exception("number of factors bigger than " + to_string(maxval));
    }

    // checking for duplicated elements
    for(size_t i=0; i<factors.size(); i++) {
      for(size_t j=i+1; j<factors.size(); j++) {
        if (factors[i].name == factors[j].name) {
          throw Exception("factor name '" + factors[i].name + "' repeated");
        }
      }
    }

    // checking factor loading ranges
    vector<double> loadings = getFactorLoadings(factors);
    validateFactorLoadings(loadings, true);

    return true;
  }
  catch(Exception &)
  {
    if (throwException) throw;
    else return false;
  }
}

/**************************************************************************//**
 * @param[in] loadings List of factors loadings.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Invalid number of factors, or factor loadings out of range.
 */
bool ccruncher::Input::validateFactorLoadings(const vector<double> &loadings, bool throwException)
{
  try
  {
    if (loadings.empty() || loadings.size() > numeric_limits<unsigned char>::max()) {
      throw Exception("loadings has invalid length");
    }

    // checking factor loading ranges
    for(size_t i=0; i<loadings.size(); i++) {
      if (std::isnan(loadings[i]) || loadings[i] < 0.0 || 1.0 < loadings[i]) {
        string msg = "factor loading out of range [0,1]";
        throw Exception(msg);
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
 * @param[in] ratings List of ratings.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Invalid number of ratings or rating name repeated.
 */
bool ccruncher::Input::validateRatings(const vector<Rating> &ratings, bool throwException)
{
  try
  {
    if (ratings.size() < 2) {
      throw Exception("required a minimum of 2 ratings");
    }
    if (ratings.size() > numeric_limits<unsigned char>::max()) {
      int maxval = numeric_limits<unsigned char>::max();
      throw Exception("number of ratings bigger than " + to_string(maxval));
    }

    // checking for duplicated elements
    for(size_t i=0; i<ratings.size(); i++) {
      for(size_t j=i+1; j<ratings.size(); j++) {
        if (ratings[i].name == ratings[j].name) {
          throw Exception("rating name '" + ratings[i].name + "' repeated");
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
 * @details Performs the following checks:
 *          - non-empty
 *          - size less than 65535
 *          - non duplicated segmentation names
 *          - 0 or 1 basic (only 1 segment) segmentation.
 * @param[in] segmentations List of segmentations.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Validation error.
 */
bool ccruncher::Input::validateSegmentations(const vector<Segmentation> &segmentations, bool throwException)
{
  try
  {
    if (segmentations.empty()) {
      throw Exception("segmentations not found");
    }
    if (segmentations.size() > numeric_limits<unsigned short>::max()) {
      throw Exception("number of segmentations bigger than " + to_string(numeric_limits<unsigned short>::max()));
    }

    // checking for enabled segmentations
    int numEnabled = 0;
    for(size_t i=0; i<segmentations.size(); i++) {
      if (segmentations[i].isEnabled()) {
        numEnabled++;
      }
    }
    if (numEnabled == 0) {
      throw Exception("enabled segmentations not found");
    }

    // check for duplicated elements
    for(size_t i=0; i<segmentations.size(); i++) {
      for(size_t j=i+1; j<segmentations.size(); j++) {
        if (segmentations[i].getName() == segmentations[j].getName()) {
          throw Exception("segmentation name '" + segmentations[i].getName() + "' repeated");
        }
      }
    }

    // check that only exist 0 or 1 'portfolio' segmentation
    /*
    string str;
    int nbasic = 0;
    for(const Segmentation &segmentation : segmentations) {
      if (segmentation.size() == 1) {
        str += (nbasic>0?", ":"") + segmentation.getName();
        nbasic++;
      }
    }
    if (nbasic > 1) {
      throw Exception("found duplicated segmentations [" + str + "]");
    }
    */

    return true;
  }
  catch(Exception &)
  {
    if (throwException) throw;
    else return false;
  }
}

/**************************************************************************//**
 * @param[in] obligors List of obligors.
 * @param[in] numFactors Number of factors.
 * @param[in] numRatings Number of ratings.
 * @param[in] segmentations List of segmentations.
 * @param[in] date1 Starting simulation date.
 * @param[in] date2 Ending simulation date.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Validation error.
 */
bool ccruncher::Input::validatePortfolio(const vector<Obligor> &obligors, unsigned char numFactors,
  unsigned char numRatings, const vector<Segmentation> &segmentations, const Date &date1,
  const Date &date2, bool throwException)
{
  try
  {
    if (obligors.empty()) {
      throw Exception("empty portfolio");
    }
    for(auto &obligor : obligors) {
      validateObligor(obligor, numFactors, numRatings, segmentations, date1, date2, true);
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
 * @param[in] obligor Obligor to validate.
 * @param[in] numFactors Number of factors.
 * @param[in] numRatings Number of ratings.
 * @param[in] segmentations List of segmentations.
 * @param[in] date1 Starting simulation date.
 * @param[in] date2 Ending simulation date.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Validation error.
 */
bool ccruncher::Input::validateObligor(const Obligor &obligor, unsigned char numFactors,
         unsigned char numRatings, const vector<Segmentation> &segmentations,
         const Date &date1, const Date &date2, bool throwException)
{
  try
  {
    if (obligor.ifactor >= numFactors) {
      throw Exception("obligor with invalid factor");
    }
    if (obligor.irating >= numRatings) {
      throw Exception("obligor with invalid rating");
    }
    if (!LGD::isValid(obligor.lgd)) {
      throw Exception("obligor with invalid lgd");
    }
    if (obligor.assets.empty()) {
      throw Exception("obligor has no assets");
    }
    if (obligor.assets.size() > numeric_limits<unsigned short>::max()) {
      throw Exception("obligor has too much assets");
    }
    for(auto &asset : obligor.assets) {
      validateAsset(asset, segmentations, date1, date2, true);
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
 * @param[in] asset Asset to validate.
 * @param[in] segmentations List of segmentations (sorted by enabled).
 * @param[in] date1 Starting simulation date.
 * @param[in] date2 Ending simulation date.
 * @param[in] throwException Throw an exception if validation fails.
 * @throw Exception Validation error.
 */
bool ccruncher::Input::validateAsset(const Asset &asset, const vector<Segmentation> &segmentations,
         const Date &date1, const Date &date2, bool throwException)
{
  try
  {
      // counting the number of enabled segmentations
    size_t numEnabledSegmentations = 0;
    for(size_t i=0; i<segmentations.size(); i++) {
      if (!segmentations[i].isEnabled()) {
        break;
      }
      else {
        numEnabledSegmentations++;
      }
    }

    if (asset.values.empty() || (asset.values.size()==1 && asset.values[0].ead==EAD(0.0))) {
      throw Exception("asset without values");
    }

    bool iszero = (asset.values[0].ead==EAD(0.0));
    for(size_t i=1; i<asset.values.size(); i++) {
      if (asset.values[i-1].date >= asset.values[i].date) {
        throw Exception("asset contains invalid datevalues");
      }
      if (asset.values[i].ead!=EAD(0.0)) {
        iszero = false;
      }
    }

    if (iszero) {
      throw Exception("asset without risk");
    }

    if (asset.segments.size() != numEnabledSegmentations) {
      throw Exception("asset with an invalid number of segments");
    }

    for(size_t k=0; k<numEnabledSegmentations; k++) {
      if (asset.segments[k] >= segmentations[k].size()) {
        throw Exception("asset with invalid segment");
      }
    }

    if (!asset.isActive(date1, date2)) {
      throw Exception("asset not active in time range");
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
 * @param[in] factors Factors list.
 * @param[in] name Factor name.
 * @return Index (0-based) of the given factor.
 * @throw Exception Factor not found.
 */
unsigned char ccruncher::Input::indexOfFactor(const std::vector<Factor> &factors, const char *name)
{
  assert(name != nullptr);
  assert(factors.size() <= numeric_limits<unsigned char>::max());
  for(size_t i=0; i<factors.size(); i++) {
    if (factors[i].name.compare(name) == 0) {
      return static_cast<unsigned char>(i);
    }
  }
  throw Exception("factor '" + string(name) + "' not found");
}

/**************************************************************************//**
 * @param[in] ratings Ratings list.
 * @param[in] name Rating name.
 * @return Index (0-based) of the given rating.
 * @throw Exception Rating not found.
 */
unsigned char ccruncher::Input::indexOfRating(const std::vector<Rating> &ratings, const char *name)
{
  assert(name != nullptr);
  assert(ratings.size() <= numeric_limits<unsigned char>::max());
  for(size_t i=0; i<ratings.size(); i++) {
    if (ratings[i].name.compare(name) == 0) {
      return static_cast<unsigned char>(i);
    }
  }
  throw Exception("rating '" + string(name) + "' not found");
}

/**************************************************************************//**
 * @param[in] segmentations Segmentations list.
 * @param[in] name Segmentation name.
 * @return Index (0-based) of the segmentation.
 * @throw Exception Segmentation not found.
 */
unsigned short ccruncher::Input::indexOfSegmentation(const std::vector<Segmentation> &segmentations, const char *name)
{
  assert(segmentations.size() <= numeric_limits<unsigned short>::max());
  for(size_t i=0; i<segmentations.size(); i++) {
    if (segmentations[i].getName().compare(name) == 0) {
      return static_cast<unsigned short>(i);
    }
  }
  throw Exception("segmentation '" + string(name) + "' not found");
}

/**************************************************************************//**
 * @param[in] dprobs List of CDFs.
 * @return Index (0-based) of the 'default' rating.
 * @throw Exception Default rating not found.
 */
unsigned char ccruncher::Input::indexOfDefaultRating(const vector<CDF> &dprobs)
{
  if (dprobs.size() < 2) {
    throw Exception("required a minimum of 2 ratings");
  }
  if (dprobs.size() > numeric_limits<unsigned char>::max()) {
    int maxval = numeric_limits<unsigned char>::max();
    throw Exception("number of ratings bigger than " + to_string(maxval));
  }
  for(size_t i=0; i<dprobs.size(); i++) {
    if (dprobs[i].evalue(0.0) > 1.0-EPSILON) {
      return static_cast<unsigned char>(i);
    }
  }
  throw Exception("default rating not found");
}

/**************************************************************************//**
 * @param[in] factors List of factors.
 * @return Factor loadings list.
 */
vector<double> ccruncher::Input::getFactorLoadings(const std::vector<Factor> &factors)
{
  assert(factors.size() <= numeric_limits<unsigned char>::max());
  vector<double> w(factors.size());
  for(size_t i=0; i<factors.size(); ++i) {
    w[i] = factors[i].loading;
  }
  return w;
}

