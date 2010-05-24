
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#ifndef _Borrower_
#define _Borrower_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <map>
#include <vector>
#include "portfolio/Asset.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/ExpatHandlers.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "interests/Interest.hpp"
#include "segmentations/Segmentations.hpp"
#include <cassert>

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Borrower : public ExpatHandlers
{

  private:

    // segmentation-segment relations
    vector<int> vsegments;
    // borrower assets list
    vector<Asset *> vassets;
    // pointer to ratings object (used by parser)
    const Ratings *ratings;
    // pointer to sectors object (used by parser)
    const Sectors *sectors;
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

    // borrower rating
    int irating;
    // borrower sector
    int isector;
    // borrower identifier
    string id;
    // borrower name
    string name;

  public:
  
    // constructor
    Borrower(const Ratings &, const Sectors &, Segmentations &, const Interest &, const Date &d1, const Date &d2);
    // destructor
    ~Borrower();
    // return the asset list
    vector<Asset *> & getAssets();
    // indicates if this borrower has cashflows in date1-date2
    bool isActive(const Date &, const Date &) throw(Exception);
    // add a segmentation-segment relation
    void addBelongsTo(int isegmentation, int isegment) throw(Exception);
    // check if belongs to segmentation-segment
    bool belongsTo(int isegmentation, int isegment);
    // given a segmentation returns the segment
    int getSegment(int isegmentation);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
