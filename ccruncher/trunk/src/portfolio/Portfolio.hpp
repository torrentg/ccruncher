
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
// Portfolio.hpp - Portfolio header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _Portfolio_
#define _Portfolio_

//---------------------------------------------------------------------------

#include <algorithm>
#include "xercesc/dom/DOM.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "interests/Interests.hpp"
#include "segmentations/Segmentations.hpp"
#include "Client.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Portfolio
{

  private:

    vector<Client *> vclients;

    void parseDOMNode(Ratings *, Sectors *, Segmentations *, Interests *, const DOMNode&) throw(Exception);
    void insertClient(Client *) throw(Exception);
    void validations() throw(Exception);
    void mtlp(int);

  public:

    Portfolio(Ratings *, Sectors *, Segmentations *, Interests *, const DOMNode &) throw(Exception);
    ~Portfolio();

    vector<Client *> *getClients();
    int getNumActiveClients(Date, Date) throw(Exception);
    void sortClients(Date from, Date to) throw(Exception);
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
