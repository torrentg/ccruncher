
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

#include <string>
#include <vector>
#include <map>
#include "portfolio/Obligor.hpp"
#include "params/Segmentations.hpp"
#include "params/Factors.hpp"
#include "params/Ratings.hpp"
#include "params/Interest.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

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
    //! List of ratings (used by parser)
    const Ratings *ratings;
    //! List of factors (used by parser)
    const Factors *factors;
    //! List of segmentations (used by parser)
    Segmentations *segmentations;
    //! List of interest (used by parser)
    const Interest *interest;
    //! Initial simulation date
    Date date1;
    //! Final simulation date
    Date date2;
    //! Auxiliar obligor (used by parser)
    Obligor *auxobligor;
    //! Map used to check id obligor oneness
    std::map<std::string,bool> idobligors;
    //! Map used to check id asset oneness
    std::map<std::string,bool> idassets;

  private:
  
      //! Copy constructor (currently forbidden)
    Portfolio(const Portfolio &);
    //! Assignment operator (currently forbidden)
    Portfolio& operator=(const Portfolio &);
    //! Inserts an obligor in the list
    void insertObligor(Obligor *) throw(Exception);

  protected:
  
    //! Directives to process an xml start tag element
    void epstart(ExpatUserData &, const char *, const char **);
    //! Directives to process an xml end tag element
    void epend(ExpatUserData &, const char *);

  public:

    //! Default constructor
    Portfolio();
    //! Constructor
    Portfolio(const Ratings &, const Factors &, Segmentations &, const Interest &, const Date &date1, const Date &date2);
    //! initialize portfolio object
    void init(const Ratings &, const Factors &, Segmentations &, const Interest &, const Date &date1, const Date &date2);
    //! Destructor
    ~Portfolio();
    //! Returns the obligors list
    std::vector<Obligor *> &getObligors();

};

} // namespace

#endif

