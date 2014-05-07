
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

#ifndef _Factors_
#define _Factors_

#include <string>
#include <vector>
#include "params/Factor.hpp"
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief List of factors/sectors.
 *
 * @see http://ccruncher.net/ifileref.html#factors
 */
class Factors : public ExpatHandlers, public std::vector<Factor>
{

  protected:
  
    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;
  
  public:

    //! Inherits std::vector constructors
    using std::vector<Factor>::vector;
    //! Adds a factor to this list
    void add(const Factor &);
    //! Return the index of the factor
    size_t indexOf(const char *name) const;
    //! Return the index of the factor
    size_t indexOf(const std::string &name) const;
    //! Return factor loadings
    std::vector<double> getLoadings() const;

};

} // namespace

#endif

