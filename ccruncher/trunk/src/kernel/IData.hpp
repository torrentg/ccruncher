
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
// IData.hpp - IData header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/25 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added logger
//
//===========================================================================

#ifndef _IData_
#define _IData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "xercesc/dom/DOM.hpp"
#include "params/Params.hpp"
#include "interests/Interests.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "portfolio/Portfolio.hpp"
#include "transitions/TransitionMatrix.hpp"
#include "correlations/CorrelationMatrix.hpp"
#include "segmentations/Segmentations.hpp"
#include "aggregators/Aggregators.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class IData
{

  private:

    void init();
    void release();
    void parseDOMNode(const DOMNode&) throw(Exception);
    void parseRootNode(const DOMNode &) throw(Exception);
    void parseParams(const DOMNode &) throw(Exception);
    void parseInterests(const DOMNode &) throw(Exception);
    void parseRatings(const DOMNode &) throw(Exception);
    void parseTransitions(const DOMNode &) throw(Exception);
    void parseSectors(const DOMNode &) throw(Exception);
    void parseCorrelations(const DOMNode &) throw(Exception);
    void parseSegmentations(const DOMNode &) throw(Exception);
    void parseAggregators(const DOMNode &) throw(Exception);
    void parsePortfolio(const DOMNode &) throw(Exception);


  public:

    Params *params;
    Interests *interests;
    Ratings *ratings;
    TransitionMatrix *transitions;
    Sectors *sectors;
    CorrelationMatrix *correlations;
    Segmentations *segmentations;
    Aggregators *aggregators;
    Portfolio *portfolio;

    IData(const DOMNode &) throw(Exception);
    IData(const string &xmlfilename) throw(Exception);
    ~IData();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
