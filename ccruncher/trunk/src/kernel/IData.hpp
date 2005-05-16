
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
// 2005/04/03 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added survival section
//
//===========================================================================

#ifndef _IData_
#define _IData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "params/Params.hpp"
#include "interests/Interests.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "portfolio/Portfolio.hpp"
#include "transitions/TransitionMatrix.hpp"
#include "survival/Survival.hpp"
#include "correlations/CorrelationMatrix.hpp"
#include "segmentations/Segmentations.hpp"
#include "aggregators/Aggregators.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class IData : public ExpatHandlers
{

  private:

    void init();
    void release();
    void validate() throw(Exception);


  public:

    Params *params;
    Interests *interests;
    Ratings *ratings;
    TransitionMatrix *transitions;
    Survival *survival;
    Sectors *sectors;
    CorrelationMatrix *correlations;
    Segmentations *segmentations;
    Aggregators *aggregators;
    Portfolio *portfolio;

    IData();
    IData(const string &xmlfilename) throw(Exception);
    ~IData();

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
