
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

#pragma once

#include <string>
#include <vector>
#include "portfolio/Obligor.hpp"
#include "params/Params.hpp"
#include "params/CDF.hpp"
#include "params/Segmentation.hpp"
#include "params/Rating.hpp"
#include "params/Factor.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Monte Carlo input data interface. Additionally, provides static
 *        methods to validate input data.
 */
class Input
{

  public:

    //! Destructor
    virtual ~Input() {}

    //! Returns simulation params
    virtual const Params & getParams() const = 0;
    //! Returns default probabilities functions (length = nratings)
    virtual const std::vector<CDF> & getCDFs() const = 0;
    //! Returns correlation matrix (dim = nfactors x nfactors)
    virtual const std::vector<std::vector<double>> & getCorrelations() const = 0;
    //! Returns factor loadings (dim = nfactors)
    virtual const std::vector<double> & getFactorLoadings() const = 0;
    //! Returns segmentations
    virtual const std::vector<Segmentation> & getSegmentations() const = 0;
    //! Returns portfolio
    virtual std::vector<Obligor> & getPortfolio() = 0;

  public:

    //! Validate a list of CDFs
    static bool validateCDFs(const std::vector<CDF> &dprobs, bool throwException=false);
    //! Validate correlations matrix
    static bool validateCorrelations(const std::vector<std::vector<double>> &M, bool throwException=false);
    //! Validate a list of factors
    static bool validateFactors(const std::vector<Factor> &factors, bool throwException=false);
    //! Validate a list of factor loadings
    static bool validateFactorLoadings(const std::vector<double> &loadings, bool throwException=false);
    //! Validate a list of ratings
    static bool validateRatings(const std::vector<Rating> &ratings, bool throwException=false);
    //! Validate a list of segmentations
    static bool validateSegmentations(const std::vector<Segmentation> &segmentations, bool throwException=false);
    //! Validate a list of obligors
    static bool validatePortfolio(const std::vector<Obligor> &obligors, unsigned char numFactors,
         unsigned char numRatings, const std::vector<Segmentation> &segmentations,
         const Date &date1, const Date &date2, bool throwException=false);
    //! Validate an obligor
    static bool validateObligor(const Obligor &obligor, unsigned char numFactors,
         unsigned char numRatings, const std::vector<Segmentation> &segmentations,
         const Date &date1, const Date &date2, bool throwException=false);
    //! Validate an asset
    static bool validateAsset(const Asset &asset, const std::vector<Segmentation> &segmentations,
         const Date &date1, const Date &date2, bool throwException=false);

    //! Return the index of the factor
    static unsigned char indexOfFactor(const std::vector<Factor> &factors, const char *name);
    //! Return the index of the factor
    static unsigned char indexOfFactor(const std::vector<Factor> &factors, const std::string &name) { return indexOfFactor(factors, name.c_str()); }
    //! Return the index of the rating
    static unsigned char indexOfRating(const std::vector<Rating> &ratings, const char *name);
    //! Return the index of the rating
    static unsigned char indexOfRating(const std::vector<Rating> &ratings, const std::string &name) { return indexOfRating(ratings, name.c_str()); }
    //! Return index of default rating
    static unsigned char indexOfDefaultRating(const std::vector<CDF> &dprobs);
    //! Return the index of the given segmentation
    static unsigned short indexOfSegmentation(const std::vector<Segmentation> &segmentations, const char *sname);
    //! Return the index of the given segmentation
    static unsigned short indexOfSegmentation(const std::vector<Segmentation> &segmentations, const std::string &name) { return indexOfSegmentation(segmentations, name.c_str()); }
    //! Return the factor loadings
    static std::vector<double> getFactorLoadings(const std::vector<Factor> &factors);

};

} // namespace
