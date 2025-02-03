
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#include <vector>
#include <string>
#include "kernel/Input.hpp"
#include "params/CDF.hpp"
#include "params/Params.hpp"
#include "params/Interest.hpp"
#include "params/Rating.hpp"
#include "params/Factor.hpp"
#include "params/Transitions.hpp"
#include "params/Segmentation.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Base class used by XmlInputData and eventually other classes.
 */
class InputData : public Input
{

  protected:

    //! Simulation title
    std::string title;
    //! Simulation description
    std::string description;
    //! Simulation parameters
    Params params;
    //! Simulation yield curve
    Interest interest;
    //! Simulation ratings
    std::vector<Rating> ratings;
    //! Simulation transition matrix
    Transitions transitions;
    //! Default probabilities functions
    std::vector<CDF> dprobs;
    //! Simulation factors
    std::vector<Factor> factors;
    //! Correlations between factors
    std::vector<std::vector<double>> correlations;
    //! Simulation segmentations
    std::vector<Segmentation> segmentations;
    //! Simulation portfolio
    std::vector<Obligor> obligors;
    //! Factor loadings
    std::vector<double> floadings;
    //! Default probabilities functions
    std::vector<CDF> cdfs;

  protected:

    //! Correlation matrix initialization
    void initCorrelations(size_t dim);
    //! Compute factor loadings
    void fillFLoadings();
    //! Compute CDFs
    void fillCDFs();
    //! Remove unused segmentation-segments
    void removeUnusedSegments();
    //! Create a table that maps old segment indexes to new segment indexes
    std::vector<std::vector<unsigned short>> getRecodeTable() const;
    //! Count the number of obligor in each segmentation-segment
    std::vector<std::vector<size_t>> getHitsPerSegment() const;
    //! Create the segmentation recode map using segmentation hits info
    std::vector<unsigned short> getRecodeTableRow(const std::vector<size_t> &hits) const;
    //! Recode each portfolio-obligor-asset segment applying table assignation
    void recodePortfolioSegments(const std::vector<std::vector<unsigned short>> &table);
    //! Update segmentations list applying recoding rules
    void recodeSegmentations(const std::vector<std::vector<unsigned short>> &table);

  public:

    //! Returns simulation params
    virtual const Params & getParams() const override { return params; }
    //! Returns default probabilities functions
    virtual const std::vector<CDF> & getCDFs() const override { return cdfs; }
    //! Returns correlation matrix
    virtual const std::vector<std::vector<double>> & getCorrelations() const override { return correlations; }
    //! Returns factor loadings (dim = nfactors)
    virtual const std::vector<double> & getFactorLoadings() const override { return floadings; }
    //! Returns segmentations
    virtual const std::vector<Segmentation> & getSegmentations() const override { return segmentations; }
    //! Returns portfolio
    virtual std::vector<Obligor> & getPortfolio() override { return obligors; }

    //! Returns simulation title
    const std::string & getTitle() const { return title; }
    //! Returns simulation description
    const std::string & getDescription() const { return description; }
    //! Returns yield curve
    const Interest & getInterest() const { return interest; }
    //! Returns ratings
    const std::vector<Rating> & getRatings() const { return ratings; }
    //! Returns transition matrix
    const Transitions & getTransitions() const { return transitions; }
    //! Returns default probabilities
    const std::vector<CDF> & getDefaultProbabilities() const { return dprobs; }
    //! Returns factors
    const std::vector<Factor> & getFactors() const { return factors; }

};

} // namespace
