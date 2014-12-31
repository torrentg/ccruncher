
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

#ifndef _Portfolio_
#define _Portfolio_

#include <map>
#include <string>
#include <vector>
#include "portfolio/Obligor.hpp"
#include "portfolio/LGD.hpp"
#include "params/Segmentation.hpp"
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief  List of obligors.
 *
 * @details This class provides support for parsing portfolio from the xml
 *          input file. Unused data is removed, this include:
 *            - unactive obligors (no asset in date range time0-time1)
 *            - asset data values out of date range time0-time1
 *            - disabled segmentations
 *            - unused segments
 *
 * @see http://ccruncher.net/ifileref.html#portfolio
 */
class Portfolio : public ExpatHandlers
{

  private:

    //! List of obligors
    std::vector<Obligor> mObligors;
    //! Map used to check id obligor oneness
    std::map<std::string,size_t> mIdObligors;
    //! Map used to check id asset oneness
    std::map<std::string,size_t> mIdAssets;
    //! xml parser stage (0=none, 1=portfolio, 2=obligor, 3=asset, 4=data)
    int mStage = 0;
    //! number of segmentations
    ushort mNumSegmentations = 0;
    //! Current obligor segments
    std::vector<ushort> obligorSegments;
    //! current asset lgd
    LGD assetLGD;

  private:

    //! Remove unused segmentation-segments
    std::vector<Segmentation> recodeSegments(const std::vector<Segmentation> &segmentations);
    //! Create a table that maps old segment indexes to new segment indexes
    std::vector<std::vector<ushort>> getRecodeTable(const std::vector<Segmentation> &segmentations);
    //! Count the number of obligor in each segmentation-segment
    std::vector<std::vector<size_t>> getHitsPerSegment(const std::vector<Segmentation> &segmentations);
    //! Create the segmentation recode map using segmentation hits info
    void recodeSegmentation(const std::vector<size_t> &hits, std::vector<ushort> &table);
    //! Recode each portfolio-obligor-asset segment applying table assignation
    void recodePortfolioSegments(const std::vector<std::vector<ushort>> &table);
    //! Create a new segmentations list applying recode rules
    std::vector<Segmentation> getSegmentationsRecoded(const std::vector<Segmentation> &segmentations, const std::vector<std::vector<ushort>> &table);
    
  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

    //! Parse portfolio tag
    void epstartPortfolio(ExpatUserData &eu, const char **attributes);
    //! Parse obligor tag
    void epstartObligor(ExpatUserData &eu, const char **attributes);
    //! Parse asset tag
    void epstartAsset(ExpatUserData &eu, const char **attributes);
    //! Parse belongs-to tag
    void epstartBelongsto(ExpatUserData &eu, const char **attributes);
    //! Parse values tag
    void epstartValues(ExpatUserData &eu, const char **attributes);
    //! Check parsed portfolio
    void ependPortfolio(ExpatUserData &eu);
    //! Check parsed obligor
    void ependObligor(ExpatUserData &eu);
    //! Check parsed asset
    void ependAsset(ExpatUserData &eu);

  public:

    //! Returns obligors list
    std::vector<Obligor> & getObligors() { return mObligors; }

};

} // namespace

#endif

