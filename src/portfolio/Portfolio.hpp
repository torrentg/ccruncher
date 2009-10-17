
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

#ifndef _Portfolio_
#define _Portfolio_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include "utils/Exception.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"
#include "ratings/Ratings.hpp"
#include "sectors/Sectors.hpp"
#include "interests/Interest.hpp"
#include "segmentations/Segmentations.hpp"
#include "portfolio/Borrower.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Portfolio : public ExpatHandlers
{

  private:

    vector<Borrower *> vborrowers;

    const Ratings *ratings;
    const Sectors *sectors;
    Segmentations *segmentations;
    const Interest *interest;
    Date date1;
    Date date2;
    Borrower *auxborrower;

    void insertBorrower(Borrower &) throw(Exception);
    void validations() throw(Exception);
    void mtlp(unsigned int);


  public:

    Portfolio(const Ratings &, const Sectors &, Segmentations &, const Interest &, const Date &date1, const Date &date2);
    ~Portfolio();

    vector<Borrower *> &getBorrowers();
    int getNumActiveBorrowers(const Date &, const Date &) throw(Exception);
    void sortBorrowers(const Date &from, const Date &to, bool onlyactive) throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------