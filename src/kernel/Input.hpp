
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#ifndef _Input_
#define _Input_

#include <vector>
#include "params/Params.hpp"
#include "params/CDF.hpp"
#include "params/Segmentations.hpp"
#include "portfolio/Obligor.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Monte Carlo input data interface.
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
    virtual const Segmentations & getSegmentations() const = 0;
    //! Returns portfolio
    virtual std::vector<Obligor> & getPortfolio() = 0;

};

} // namespace

#endif

