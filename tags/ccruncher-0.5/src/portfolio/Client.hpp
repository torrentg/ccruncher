
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// Client.hpp - Client header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . asset refactoring
//
// 2005/04/03 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/07/26 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added hash key (to accelerate comparations)
//
//===========================================================================

#ifndef _Client_
#define _Client_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <map>
#include <algorithm>
#include "portfolio/Asset.hpp"
#include "portfolio/DateValues.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/ExpatHandlers.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "interests/Interests.hpp"
#include "interests/Interest.hpp"
#include "segmentations/Segmentation.hpp"
#include "segmentations/Segmentations.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Client : public ExpatHandlers
{

  private:

    map<int,int> belongsto;
    vector<Asset> vassets;

    Ratings *ratings;
    Sectors *sectors;
    Segmentations *segmentations;
    Interests *interests;
    Asset auxasset;

    void insertAsset(Asset &) throw(Exception);
    void insertBelongsTo(int iconcept, int tsegment) throw(Exception);

  public:

    int irating;
    int isector;
    string id;
    string name;
    unsigned long hkey;

    Client(Ratings *, Sectors *, Segmentations *, Interests *);
    ~Client();

    vector<Asset> * getAssets();
    bool isActive(Date, Date) throw(Exception);

    void addBelongsTo(int iconcept, int isegment) throw(Exception);
    bool belongsTo(int iconcept, int isegment);
    int getSegment(int iconcept);

    static bool less(const Client *left, const Client *right);
    void reset(Ratings *, Sectors *, Segmentations *, Interests *);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

// comparation operator
bool operator <  (const Client&, const Client&);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
