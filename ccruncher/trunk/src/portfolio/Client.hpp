
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
//===========================================================================

#ifndef _Client_
#define _Client_

//---------------------------------------------------------------------------

#include <map>
#include <algorithm>
#include "xercesc/dom/DOM.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "interests/Interests.hpp"
#include "interests/Interest.hpp"
#include "finances/Asset.hpp"
#include "finances/DateValues.hpp"
#include "segmentations/Segmentation.hpp"
#include "segmentations/Segmentations.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Client
{

  private:

    map<int,int> belongsto;
    vector<Asset*> vassets;

    void parseDOMNode(Ratings *, Sectors *, Segmentations *, Interests *, const DOMNode&) throw(Exception);
    void insertAsset(Asset *) throw(Exception);
    void insertBelongsTo(int iconcept, int tsegment) throw(Exception);
    
  public:

    int irating;
    int isector;
    string id;
    string name;

    Client(Ratings *, Sectors *, Segmentations *, Interests *, const DOMNode &) throw(Exception);
    ~Client();

    vector<Asset*> * getAssets();
    bool isActive(Date, Date) throw(Exception);

    void addBelongsTo(int iconcept, int isegment) throw(Exception);
    bool belongsTo(int iconcept, int isegment);
    int getSegment(int iconcept);

    static bool less(const Client *left, const Client *right);
};

//---------------------------------------------------------------------------

// comparation operator
bool operator <  (const Client&, const Client&);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
