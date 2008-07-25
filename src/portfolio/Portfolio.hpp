
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
// Portfolio.hpp - Portfolio header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/03 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/09/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added onlyactive argument to sortClients() method
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Portfolio refactoring
//
//===========================================================================

#ifndef _Portfolio_
#define _Portfolio_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <algorithm>
#include "utils/Exception.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "interests/Interests.hpp"
#include "segmentations/Segmentations.hpp"
#include "portfolio/Client.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Portfolio : public ExpatHandlers
{

  private:

    vector<Client *> vclients;

    Ratings *ratings;
    Sectors *sectors;
    Segmentations *segmentations;
    Interests *interests;
    Client *auxclient;

    void insertClient(Client &) throw(Exception);
    void validations() throw(Exception);
    void mtlp(unsigned int);
    void reset(Ratings &, Sectors &, Segmentations &, Interests &);

  public:

    Portfolio(Ratings &, Sectors &, Segmentations &, Interests &);
    ~Portfolio();

    vector<Client *> &getClients();
    int getNumActiveClients(const Date &, const Date &) throw(Exception);
    void sortClients(const Date &from, const Date &to, bool onlyactive) throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
