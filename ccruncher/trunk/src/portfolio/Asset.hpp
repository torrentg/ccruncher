
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
// Asset.hpp - Asset header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . asset refactoring
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#ifndef _Asset_
#define _Asset_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <map>
#include <vector>
#include "xercesc/dom/DOM.hpp"
#include "interests/Interests.hpp"
#include "segmentations/Segmentation.hpp"
#include "segmentations/Segmentations.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/XMLUtils.hpp"
#include "utils/ExpatHandlers.hpp"
#include "DateValues.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Asset : public ExpatHandlers
{

  private:

    string id;
    string name;
    map<int,int> belongsto;
    vector<DateValues> data;
    Segmentations *segmentations;
    bool tilt;

    void parseDOMNode(const DOMNode &, Segmentations *) throw(Exception);
    void parseData(const DOMNode &) throw(Exception);
    double getVCashFlow(Date date1, Date date2, Interests *);
    double getVExposure(Date date1, Date date2, Interests *);
    double getVRecovery(Date date1, Date date2, Interests *);
    void insertDateValues(DateValues &) throw(Exception);
    void insertBelongsTo(int iconcept, int tsegment) throw(Exception);
    

  public:

    Asset(Segmentations *);
    Asset(const DOMNode &, Segmentations *) throw(Exception);
    ~Asset();

    string getId(void);
    string getName(void);

    void addBelongsTo(int iconcept, int isegment) throw(Exception);
    void getVertexes(Date *dates, int n, Interests *ints, DateValues *ret);
    vector<DateValues> *getData();
    bool belongsTo(int iconcept, int isegment);
    int getSegment(int iconcept);
    void reset(Segmentations *);
    
    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
