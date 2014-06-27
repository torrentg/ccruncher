
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

#ifndef _Obligor_
#define _Obligor_

#include <vector>
#include "portfolio/Asset.hpp"
#include "portfolio/LGD.hpp"
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief  Obligor with a rating and a list of assets.
 *
 * @details Obligors are adscribed to one segment in every segmentation that
 *          has obligors as components. Internally, obligors implements this
 *          assigning his assets to the adscribed obligors segmentations.
 *
 * @see http://ccruncher.net/ifileref.html#portfolio
 */
class Obligor : public ExpatHandlers
{

  private:

    //! Segmentation-segment relations
    std::vector<int> mSegments;
    //! Obligor assets list
    std::vector<Asset *> mAssets;

  private: 

    //! Insert an asset
    void insertAsset(ExpatUserData &eu);

  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:

    //! Obligor rating
    size_t irating;
    //! Obligor factor
    size_t ifactor;
    //! Obligor identifier
    std::string id;
    //! Default lgd
    LGD lgd;

  public:

    //! Copy constructor
    Obligor(const Obligor &);
    //! Constructor
    Obligor(size_t nsegmentations=0);
    //! Destructor
    virtual ~Obligor() override;
    //! Assignment operator
    Obligor& operator=(const Obligor &);
    //! Return the asset list
    std::vector<Asset *> & getAssets();
    //! Indicates if this obligor has values in date1-date2
    bool isActive(const Date &, const Date &);
    //! Add a segmentation-segment relation
    void addBelongsTo(int isegmentation, int isegment);
    //! Check if belongs to segmentation-segment
    bool belongsTo(int isegmentation, int isegment);
    //! Given a segmentation returns the segment
    int getSegment(int isegmentation);
    //! Says if this obligor has an asset that use obligor's lgd
    bool hasLGD() const;

};

} // namespace

#endif

