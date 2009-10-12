
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

#ifndef _IData_
#define _IData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "params/Params.hpp"
#include "interests/Interest.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "portfolio/Portfolio.hpp"
#include "transitions/TransitionMatrix.hpp"
#include "survival/Survival.hpp"
#include "correlations/CorrelationMatrix.hpp"
#include "segmentations/Segmentations.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "utils/Timer.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class IData : public ExpatHandlers
{

  private:

    string title;
    string description;
    Params params;
    Interest interest;
    Ratings ratings;
    TransitionMatrix transitions;
    Survival survival;
    Sectors sectors;
    CorrelationMatrix correlations;
    Segmentations segmentations;
    Portfolio *portfolio;

    // portfolio can be huge (memory, time, ...)
    bool parse_portfolio;
    bool hasmaintag;
    Timer timer;

    void init();
    void release();
    void validate() throw(Exception);

  public:

    IData();
    IData(const string &xmlfilename, bool _parse_portfolio = true) throw(Exception);
    ~IData();

    /** get methods */
    string & getTitle();
    string & getDescription();
    const Params & getParams() const;
    const Interest & getInterest() const;
    const Ratings & getRatings() const;
    const TransitionMatrix & getTransitionMatrix() const;
    const Survival & getSurvival() const;
    const Sectors & getSectors() const;
    const CorrelationMatrix & getCorrelationMatrix() const;
    Segmentations & getSegmentations();
    Portfolio & getPortfolio() const;
    /** has methods */
    bool hasSurvival() const;
    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);
    void epdata(ExpatUserData &eu, const char *name_, const char *data, int len);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
