
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Borrower : public ExpatHandlers
{

  private:

    map<int,int> belongsto;
    vector<Asset> vassets;

    const Ratings *ratings;
    const Sectors *sectors;
    Segmentations *segmentations;
    const Interest *interest;
    Date date1;
    Date date2;
    Asset auxasset;

    void insertAsset(Asset &) throw(Exception);
    void insertBelongsTo(int isegmentation, int tsegment) throw(Exception);


  public:

    int irating;
    int isector;
    string id;
    string name;

    Borrower(const Ratings &, const Sectors &, Segmentations &, const Interest &, const Date &d1, const Date &d2);
    ~Borrower();

    vector<Asset> & getAssets();
    bool isActive(const Date &, const Date &) throw(Exception);

    void addBelongsTo(int isegmentation, int isegment) throw(Exception);
    bool belongsTo(int isegmentation, int isegment);
    int getSegment(int isegmentation);

    static bool less(const Borrower *left, const Borrower *right);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

// comparation operator
bool operator <  (const Borrower&, const Borrower&);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
