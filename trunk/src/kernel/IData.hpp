
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

#ifndef _IData_
#define _IData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <iostream>
#include <string>
#include <map>
#include "params/Params.hpp"
#include "params/Interest.hpp"
#include "params/Ratings.hpp"
#include "params/Sectors.hpp"
#include "params/Transitions.hpp"
#include "params/Survivals.hpp"
#include "params/Correlations.hpp"
#include "params/Segmentations.hpp"
#include "portfolio/Portfolio.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

#define STDIN_FILENAME "<stdin>"

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class IData : public ExpatHandlers
{

  private:

    // configuration filename
    string filename;
    // simulation title
    string title;
    // simulation description
    string description;
    // simulation parameters
    Params params;
    // simulation yield interest
    Interest interest;
    // simulation ratings
    Ratings ratings;
    // simulation transition matrix
    Transitions transitions;
    // simulation survival functions
    Survivals survivals;
    // simulation sectors
    Sectors sectors;
    // simulation correlations
    Correlations correlations;
    // simulation segmentations
    Segmentations segmentations;
    // simulation portfolio
    Portfolio *portfolio;
    // indicates if root tag exists
    bool hasmaintag;
    // defines flag (0=none, 1=current, 2=done)
    int hasdefinestag;

  private:
  
    // intialize class
    void init();
    // dealloc memory
    void release();
    // validate simulation data
    void validate() throw(Exception);
    // parse data
    void parse(istream &is, const map<string,string> &m) throw(Exception);
    // parse portfolio
    void parsePortfolio(ExpatUserData &, const char *, const char **) throw(Exception);
    // check define
    void checkDefine(const string &key, const string &value) const throw(Exception);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);
    // ExpatHandlers method
    void epdata(ExpatUserData &, const char *, const char *, int);

  public:

    // default constructor
    IData();
    // constructor
    IData(const string &xmlfilename, const map<string,string> &m=map<string,string>()) throw(Exception);
    // destructor
    ~IData();
    // returns simulation title
    string & getTitle();
    // returns simulation description
    string & getDescription();
    // returns simulation params
    Params & getParams();
    // returns simulation yield interest
    Interest & getInterest();
    // returns simulation ratings
    Ratings & getRatings();
    // returns simulation transition matrix
    Transitions & getTransitions();
    // returns simulation survivals functions
    Survivals & getSurvivals();
    // returns simulation sectors
    Sectors & getSectors();
    // returns simulation correlation matrix
    Correlations & getCorrelations();
    // returns simulation correlations
    Segmentations & getSegmentations();
    // returns simulation portfolio
    Portfolio & getPortfolio();
    // indicates if survivals tag is defined
    bool hasSurvivals() const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
