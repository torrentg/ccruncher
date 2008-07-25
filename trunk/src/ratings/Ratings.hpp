
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
// Ratings.hpp - Ratings header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from xerces to expat
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@mail.generacio.com]
//   . class refactoring
//
// 2007/07/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added getIndex() method
//
//===========================================================================

#ifndef _Ratings_
#define _Ratings_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "ratings/Rating.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Ratings : public ExpatHandlers
{

  private:

    // ratings list
    vector<Rating> vratings;
    // auxiliary variable (used by parser)
    Rating auxrating;

    // insert a rating in the list
    void insertRating(const Rating &) throw(Exception);
    // validate object content
    void validations() throw(Exception);


  public:

    // constructor
    Ratings();
    // destructor
    ~Ratings();

    // return the number of ratings
    int size() const;
    // return the index of the rating
    int getIndex(const string &name) const;
    // [] operator
    Rating& operator [] (int i);
    // [] operator
    Rating& operator [] (const string &name) throw(Exception);
    // serialize object content as xml
    string getXML(int) const throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
