
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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
#include <gsl/gsl_rng.h>
#include "interests/Interest.hpp"
#include "segmentations/Segmentations.hpp"
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
    // asset name
    string name;
    // asset creation date
    Date date;
    // last asset date
    Date lastdate;
    // cashflow values
    vector<DateValues> data;
    // pointer to segmentations list
    Segmentations *segmentations;
    // precomputed data
    vector<DateValues> pdata;
    // auxiliary variable (used by parser)
    bool have_data;
    // default recovery
    Recovery drecovery;

  private:
  
    // insert a cashflow value
    void insertDateValues(const DateValues &) throw(Exception);
    // returns recovery at given date
    Recovery getRecovery(Date d);
    // returns loss at given date
    double getLossX(Date d);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // constructor
    Asset(Segmentations *, Recovery recovery=Recovery::getNAN());
    // destructor
    ~Asset();
    // return asset id
    string getId(void) const;
    // returns asset name
    string getName(void) const;
    // add a segmentation-segment relation
    void addBelongsTo(int isegmentation, int isegment) throw(Exception);
    // precompute losses
    void precomputeLosses(const Date &d1, const Date &d2, const Interest &interest);
    // returns loss at the given default time
    double getLoss(const Date &at, const gsl_rng *rng=NULL) const;
    // returns a pointer to cashflow
    vector<DateValues> &getData();
    // check if belongs to segmentation-segment
    bool belongsTo(int isegmentation, int isegment) const;
    // given a segmentation returns the segment
    int getSegment(int isegmentation) const;
    // free memory allocated by DateValues
    void deleteData();
    // indicates if this asset has cashflows in date1-date2
    bool isActive(const Date &, const Date &) throw(Exception);
    // returns minimum event date (restricted to precomputed events)
    Date getMinDate() const;
    // returns maximum event date (restricted to precomputed events)
    Date getMaxDate() const;
    // returns default recovery
    Recovery getRecovery() const;

};

//---------------------------------------------------------------------------

//===========================================================================
// getSegment
//===========================================================================
inline int ccruncher::Asset::getSegment(int isegmentation) const
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int) vsegments.size());
  return vsegments[isegmentation];
}

//===========================================================================
// getLoss
//===========================================================================
inline double ccruncher::Asset::getLoss(const Date &at, const gsl_rng *rng) const
{
  int length = (int) pdata.size();

  if (length == 0 || at < pdata.front().date || pdata.back().date < at)
  {
    return 0.0;
  }
  else 
  {
    for(int i=0; i<length; i++) 
    {
      if (at <= pdata[i].date) 
      {
        return pdata[i].cashflow * (1.0-pdata[i].recovery.getValue(rng));
      }
    }
  }
  
  assert(false);
  return 0.0;
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
