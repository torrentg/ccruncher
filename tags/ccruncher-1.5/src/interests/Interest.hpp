
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

#ifndef _Interest_
#define _Interest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"
#include "interests/Rate.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Interest : public ExpatHandlers
{

  private:

    // date where interest curve is defined
    Date date0;
    // rate values
    vector<Rate> vrates;
    // auxiliary variable (used by parser)
    Rate auxrate;

  private:

    // insert a rate to list
    void insertRate(Rate &) throw(Exception);
    // given a time, returns the rate (interpolated)
    double getValue(const double) const;
    // transforms from date to index
    double date2idx(const Date &date1) const;
    // returns upsilon function value
    double getUpsilon(const double r, const double t) const;

  public:

    // default constructor
    Interest();
    // constructor
    Interest(const Date &);
    // destructor
    ~Interest();
    // numbers of rates
    int size() const;
    // returns initial date
    Date getDate0() const;
    // returns upsilon value
    double getUpsilon(const Date &date1, const Date &date2) const;
    // serialize object content as xml
    string getXML(int) const throw(Exception);
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
