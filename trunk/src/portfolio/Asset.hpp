
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

#ifndef _Asset_
#define _Asset_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <map>
#include <vector>
#include "interests/Interest.hpp"
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
    // precomputed times at event dates
    vector<Date> ptimes;
    // precomputed losses at event dates
    vector<double> plosses;
    // auxiliary variable (used by parser)
    bool have_data;

    // returns recovery at given date
    double getRecovery(Date d, const Interest &, Date c);
    // returns cashflow sum from given date
    double getCashflowSum(Date d, const Interest &, Date c);
    // insert a cashflow value
    void insertDateValues(const DateValues &) throw(Exception);
    // insert a segmentation-segment relation
    void insertBelongsTo(int isegmentation, int tsegment) throw(Exception);


  public:

    // constructor
    Asset(Segmentations *);
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
    // precompute losses
    void precomputeLosses(const Date &d1, const Date &d2, const Interest &interest);
    // returns precomputed loss at requested time node index
    double getLoss(const Date &at);
    // returns a pointer to cashflow
    vector<DateValues> &getData();
    // check if belongs to segmentation-segment
    bool belongsTo(int isegmentation, int isegment);
    // given a segmentation returns the segment
    int getSegment(int isegmentation);
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
