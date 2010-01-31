
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
    // last computed loss
    double loss;
    // asset identifier
    string id;
    // asset name
    string name;
    // minimum event date (= creation date)
    Date mindate;
    // maximum event date
    Date maxdate;
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

  private:
  
    // returns recovery at given date
    double getRecovery(Date d);
    // returns cashflow sum from given date
    double getCashflowSum(Date d);
    // insert a cashflow value
    void insertDateValues(const DateValues &) throw(Exception);

  public:

    // constructor
    Asset(Segmentations *);
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
    // returns precomputed loss at requested time node index
    double getLoss(const Date &at, bool force=true);
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
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

//===========================================================================
// getSegment
//===========================================================================
inline int ccruncher::Asset::getSegment(int isegmentation)
{
  assert(isegmentation >= 0);
  assert(isegmentation < (int) vsegments.size());
  return vsegments[isegmentation];
}

//===========================================================================
// getLoss
// force=true --> loss is computed and stored in variable loss, return loss
// force=false -> returns loss variable value
//===========================================================================
inline double ccruncher::Asset::getLoss(const Date &at, bool force)
{
  if (!force) return loss;
  else loss = 0.0;

  int length = (int) ptimes.size();

  if (at < mindate || maxdate < at || length == 0)
  {
    loss = 0.0;
  }
  else if (ptimes[length-1] < at)
  {
    loss = 0.0;
  }
  else 
  {
    for(int i=0; i<length; i++) 
    {
      if (at <= ptimes[i]) 
      {
        loss = plosses[i];
        break;
      }
    }
  }
  return loss;
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
