
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
// Interest.hpp - Interest header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _Interest_
#define _Interest_

//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include "xercesc/dom/DOM.hpp"
#include "utils/Date.hpp"
#include "Rate.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Interest
{

  private:

    static const double EPSILON=1e-7;

    string name;
    Date fecha;
    vector<Rate> vrates;

    void parseDOMNode(const DOMNode&) throw(Exception);  
    void insertRate(Rate &) throw(Exception);

    double getValue(const double);
    double date2idx(Date &date1);
    Date idx2date(int t);  
    double actualCoef(const double r, const double t);
    double updateCoef(const double r, const double t);

  public:

    Interest();
    Interest(const string &);
    Interest(const DOMNode &) throw(Exception);
    ~Interest();

    string getName() const;
    Date getFecha() const;

    double getActualCoef(Date &date1, Date &date2) throw(Exception);
    double getUpdateCoef(Date &date1, Date &date2) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
