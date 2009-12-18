
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
#include <string>
#include <vector>
#include <map>
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

    // list of borrowers
    vector<Borrower *> vborrowers;
    // list of ratings (used by parser)
    const Ratings *ratings;
    // list of sectors (used by parser)
    const Sectors *sectors;
    // list of segmentations (used by parser)
    Segmentations *segmentations;
    // list of interest (used by parser)
    const Interest *interest;
    // initial simulation date
    Date date1;
    // final simulation date
    Date date2;
    // auxiliar borrower (used by parser)
    Borrower *auxborrower;
    // map used to check id borrower oneness
    map<string,bool> idborrowers;
    // map used to check id asset oneness
    map<string,bool> idassets;

  private:
  
    // inserts a borrower in the portfolio
    void insertBorrower(Borrower *) throw(Exception);
    // validate portfolio
    void validations() throw(Exception);
    // move the i-th borrower to the last position in the list
    void mtlp(unsigned int);

  public:

    // constructor
    Portfolio(const Ratings &, const Sectors &, Segmentations &, const Interest &, const Date &date1, const Date &date2);
    // destructor
    ~Portfolio();
    // returns the borrowers list
    vector<Borrower *> &getBorrowers();
    // returns the number of active borrowers
    int getNumActiveBorrowers(const Date &, const Date &) throw(Exception);
    // sort the borrower list (by sector and rating)
    void sortBorrowers(const Date &from, const Date &to, bool onlyactive) throw(Exception);
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
