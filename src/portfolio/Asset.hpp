
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
#include <algorithm>
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
    // destructor
    ~Asset();
    // return asset id
    string getId(void) const;
    // returns asset name
    string getName(void) const;
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
    // returns exposure-recovery at given date
    const DateValues& getValues(const Date) const;
    // says if use obligor recovery
    bool hasObligorRecovery() const;

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
// getData
// returns:
//    > (NAD,0,1) if at <= asset creation date
//    > (NAD,0,1) if asset has 0 date-values
//    > (NAD,0,1) if at > last date-values
//    > otherwise, returns the smallest date-values that is not less than at
//===========================================================================
inline const DateValues& ccruncher::Asset::getValues(const Date at) const
{
  static const DateValues dvnf(NAD, Exposure(Exposure::Fixed,0.0), Recovery(Recovery::Fixed,1.0));
  
  //if (at < date || data.size() == 0 || at < data[0].date || data.back().date < at)
  if (at <= date || data.size() == 0 || data.back().date < at)
  {
    return dvnf;
  }
  else if (data.back().date == at)
  {
    return data.back();
  }
  else
  {
    return *(upper_bound(data.begin(), data.end(), DateValues(at-1)));
  }
  
  /*
  for(unsigned int i=0; i<data.size(); i++) 
  {
    if (at <= data[i].date) 
    {
      return data[i];
    }
  }
  
  assert(false);
  return dvnf;
  */
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
