
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

#ifndef _Portfolio_
#define _Portfolio_

#include <map>
#include <string>
#include <vector>
#include "portfolio/Obligor.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief  List of obligors.
 *
 * @see http://ccruncher.net/ifileref.html#portfolio
 */
class Portfolio : public ExpatHandlers
{

  private:

    //! List of obligors
    std::vector<Obligor *> vobligors;
    //! Map used to check id obligor oneness
    std::map<std::string,bool> idobligors;
    //! Map used to check id asset oneness
    std::map<std::string,bool> idassets;

  private:
  
      //! Copy constructor (currently forbidden)
    Portfolio(const Portfolio &);
    //! Assignment operator (currently forbidden)
    Portfolio& operator=(const Portfolio &);
    //! Checks an obligor
    void checkObligor(Obligor *) throw(Exception);

  protected:
  
    //! Directives to process an xml start tag element
    void epstart(ExpatUserData &, const char *, const char **);
    //! Directives to process an xml end tag element
    void epend(ExpatUserData &, const char *);

  public:

    //! Default constructor
    Portfolio() {}
    //! Destructor
    ~Portfolio();
    //! Returns the obligors list
    std::vector<Obligor *> &getObligors();

};

} // namespace

#endif

