
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include "portfolio/Asset.hpp"
#include "portfolio/LGD.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/ExpatHandlers.hpp"
#include "params/Ratings.hpp"
#include "params/Factors.hpp"
#include "params/Interest.hpp"
#include "params/Segmentations.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Obligor : public ExpatHandlers
{

  private:

    // segmentation-segment relations
    std::vector<int> vsegments;
    // obligor assets list
    std::vector<Asset *> vassets;
    // pointer to ratings object (used by parser)
    const Ratings *ratings;
    // pointer to factors object (used by parser)
    const Factors *factors;
    // pointer to segmentations object (used by parser)
    Segmentations *segmentations;
    // pointer to interest object (used by parser)
    const Interest *interest;
    // initial simulation date
    Date date1;
    // ending simulation date
    Date date2;

  private: 
  
    // insert a asset
    void prepareLastAsset() throw(Exception);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // obligor rating
    int irating;
    // obligor factor
    int ifactor;
    // obligor identifier
    std::string id;
    // default lgd
    LGD lgd;

  public:
  
    // copy constructor
    Obligor(const Obligor &);
    // constructor
    Obligor(const Ratings &, const Factors &, Segmentations &, const Interest &, const Date &d1, const Date &d2);
    // destructor
    ~Obligor();
    // assignment operator
    Obligor& operator=(const Obligor &);
    // return the asset list
    std::vector<Asset *> & getAssets();
    // indicates if this obligor has values in date1-date2
    bool isActive(const Date &, const Date &) throw(Exception);
    // add a segmentation-segment relation
    void addBelongsTo(int isegmentation, int isegment) throw(Exception);
    // check if belongs to segmentation-segment
    bool belongsTo(int isegmentation, int isegment);
    // given a segmentation returns the segment
    int getSegment(int isegmentation);
    // says if this obligor has an asset that use obligor's lgd
    bool hasLGD() const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------