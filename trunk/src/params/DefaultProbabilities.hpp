
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#ifndef _DefaultProbabilities_
#define _DefaultProbabilities_

#include <vector>
#include "params/CDF.hpp"
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Default probabilities functions.
 *
 * @details These functions provides the probability of default (PD) at
 *          a given time (t) for a fixed rating (r): PD(t;r).
 *          This class provides methods to retrieve them from the xml
 *          input file and check if the content is valid.
 *
 * @see http://ccruncher.net/ifileref.html#dprobs
 */
class DefaultProbabilities : public std::vector<CDF>, public ExpatHandlers
{

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:

    //! Inherits std::vector constructors
    using std::vector<CDF>::vector;

  public:

    //! Return index of default rating
    static unsigned char getIndexDefault(const std::vector<CDF> &dprobs);
    //! Validate object content
    static bool isValid(const std::vector<CDF> &dprobs, bool throwException=false);

};

} // namespace

#endif

