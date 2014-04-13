
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

#ifndef _Params_
#define _Params_

#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Parameters of the simulation.
 *
 * @see http://ccruncher.net/ifileref.html#parameters
 */
class Params : public ExpatHandlers
{

  private:

    //! Parse a parameter
    void parseParameter(ExpatUserData &, const char **) throw(Exception);
    //! Validate object content
    void validate() const throw(Exception);

  public:

    //! time.0 param value
    Date time0;
    //! time.T param value
    Date timeT;
    //! maxiterations param value
    int maxiterations;
    //! maxseconds param value
    int maxseconds;
    //! copula.type param value
    std::string copula_type;
    //! rng.seed param value
    unsigned long rng_seed;
    //! antithetic param value
    bool antithetic;
    //! lhs param value
    unsigned short lhs_size;
    //! blocksize param value
    unsigned short blocksize;

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;
  
  public:
  
    //! Constructor
    Params();
    //! Returns copula type as string (gaussian or t)
    std::string getCopulaType() const throw(Exception);
    //! Returns copula param (only t-copula)
    double getCopulaParam() const throw(Exception);

};

} // namespace

#endif

