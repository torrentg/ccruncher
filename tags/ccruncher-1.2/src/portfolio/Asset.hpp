
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
// Asset.hpp - Asset header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/03/16 - Gerard Torrent [gerard@mail.generacio.com]
//   . asset refactoring
//
// 2005/04/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from xerces to expat
//
// 2005/07/09 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed exposure/recovery by netting
//
// 2005/08/31 - Gerard Torrent [gerard@mail.generacio.com]
//   . tag concept renamed to segmentation
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@mail.generacio.com]
//   . Interests class refactoring
//   . Asset refactoring
//
// 2006/01/05 - Gerard Torrent [gerard@mail.generacio.com]
//   . netting replaced by recovery
//
// 2007/07/26 - Gerard Torrent [gerard@mail.generacio.com]
//   . added asset creation date
//   . getLosses function reviewed
//   . added precomputeLosses function
//
//===========================================================================

#ifndef _Asset_
#define _Asset_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <map>
#include <vector>
#include "interests/Interest.hpp"
#include "interests/Interests.hpp"
#include "segmentations/Segmentations.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/ExpatHandlers.hpp"
#include "portfolio/DateValues.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Asset : public ExpatHandlers
{

  private:

    // asset identifier
    string id;
    // asset name
    string name;
    // minimum event date (= creation date)
    Date mindate;
    // maximum event date
    Date maxdate;
    // segmentation-segment relations
    map<int,int> belongsto;
    // cashflow values
    vector<DateValues> data;
    // pointer to segmentations list
    Segmentations *segmentations;
    // precomputed losses at time nodes
    vector<double> plosses;
    // auxiliary variable (used by parser)
    bool have_data;

    // returns data index by right, -1 if don't exist
    int getRightIdx(Date d);
    // returns cashflow sum from given date
    double getCashflowSum(Date d, const Interest &);
    // precompute loss at d2
    double precomputeLoss(const Date d1, const Date d2, const Interest &spot);
    // insert a cashflow value
    void insertDateValues(const DateValues &) throw(Exception);
    // insert a segmentation-segment relation
    void insertBelongsTo(int isegmentation, int tsegment) throw(Exception);


  public:

    // constructor
    Asset(Segmentations &);
    // destructor
    ~Asset();

    // hash key (to speed up equals by id)
    unsigned long hkey;

    // return asset id
    string getId(void) const;
    // returns asset name
    string getName(void) const;
    // add a segmentation-segment relation
    void addBelongsTo(int isegmentation, int isegment) throw(Exception);
    // precompute losses at given dates
    void precomputeLosses(const vector<Date> &dates, const Interests &interests);
    // returns precomputed loss at requested time node index
    double getLoss(int k);
    // returns a pointer to cashflow
    vector<DateValues> &getData();
    // check if belongs to segmentation-segment
    bool belongsTo(int isegmentation, int isegment);
    // given a segmentation returns the segment
    int getSegment(int isegmentation);
    // reset object content
    void reset(Segmentations *);
    // free memory allocated by DateValues
    void deleteData();
    // returns minimum event date
    Date getMinDate();
    // returns maximum event date
    Date getMaxDate();

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------