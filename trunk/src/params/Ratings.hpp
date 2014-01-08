
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#ifndef _Ratings_
#define _Ratings_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Ratings : public ExpatHandlers
{

  private:

    // internal struct
    struct Rating
    {
        // rating name
        std::string name;
        // rating description
        std::string desc;
        // constructor
        Rating(const std::string &n="", const std::string &d="") : name(n), desc(d) {}
    };

  private:

    // ratings list
    std::vector<Rating> vratings;

  private:
  
    // insert a rating in the list
    void insertRating(const Rating &) throw(Exception);
    // validate object content
    void validations() throw(Exception);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // constructor
    Ratings();
    // return the number of ratings
    int size() const;
    // return the index of the rating
    int getIndex(const char *name) const;
    int getIndex(const std::string &name) const;
    // return rating name
    const std::string& getName(int i) const;
    // return rating description
    const std::string& getDescription(int i) const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
