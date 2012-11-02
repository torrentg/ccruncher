
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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
#include <cmath>
#include <vector>
#include <algorithm>
#include "params/Interest.hpp"
#include "params/Segmentations.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "utils/ExpatHandlers.hpp"
#include "portfolio/DateValues.hpp"
#include "portfolio/Recovery.hpp"
#include <cassert>

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Asset : public ExpatHandlers
{

  private:

    // segmentation-segment relations
    vector<int> vsegments;
    // asset identifier
    string id;
    // asset creation date
    Date date;
    // exposure-recovery values
    vector<DateValues> data;
    // default recovery
    Recovery drecovery;
    // pointer to segmentations list (used by parser)
    Segmentations *segmentations;
    // auxiliary variable (used by parser)
    bool have_data;

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // constructor
    Asset(Segmentations *);
    // return asset id
    string getId() const;
    // add a segmentation-segment relation
    void addBelongsTo(int isegmentation, int isegment) throw(Exception);
    // precpare data
    void prepare(const Date &d1, const Date &d2, const Interest &interest);
    // check if belongs to segmentation-segment
    bool belongsTo(int isegmentation, int isegment) const;
    // given a segmentation returns the segment
    int getSegment(int isegmentation) const;
    // indicates if this asset has info in date1-date2
    bool isActive(const Date &, const Date &) throw(Exception);
    // returns minimum event date (restricted to precomputed events)
    Date getMinDate() const;
    // returns maximum event date (restricted to precomputed events)
    Date getMaxDate() const;
    // used to test SimulationThread::simule lower_bound
    const DateValues& getValues(const Date t) const;
    // says if use obligor recovery
    bool hasObligorRecovery() const;
    // returns reference to data
    const vector<DateValues>& getData() const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
