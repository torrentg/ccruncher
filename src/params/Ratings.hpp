
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

#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief List of ratings.
 *
 * @see http://ccruncher.net/ifileref.html#ratings
 */
class Ratings : public ExpatHandlers
{

  private:

    //! Internal struct
    struct Rating
    {
        //! Rating name
        std::string name;
        //! Rating description
        std::string desc;
        //! Constructor
        Rating(const std::string &n="", const std::string &d="") : name(n), desc(d) {}
    };

  private:

    //! Ratings list
    std::vector<Rating> vratings;

  private:
  
    //! insert a rating in the list
    void insertRating(const Rating &) throw(Exception);

  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:

    //! Default constructor
    Ratings() {}
    //! Number of ratings
    int size() const;
    //! Return the index of the rating
    int getIndex(const char *name) const;
    //! Return the index of the rating
    int getIndex(const std::string &name) const;
    //! Return i-th rating name
    const std::string& getName(int i) const;
    //! Return i-th rating description
    const std::string& getDescription(int i) const;

};

} // namespace

#endif

