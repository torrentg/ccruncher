
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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
#include <map>
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Parameters of the simulation.
 *
 * @see http://ccruncher.net/ifileref.html#parameters
 */
class Params : public std::map<std::string,std::string>, public ExpatHandlers
{

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;
  
  private:

    //! Returns parameter value
    std::string getParamValue(const std::string &key) const;
    //! Returns parameter value
    std::string getParamValue(const std::string &key, const std::string &defaultValue) const;

  public:
  
    //! Inherits std::map constructors
    using std::map<std::string,std::string>::map;
    //! Returns starting time
    Date getTime0() const;
    //! Returns ending time
    Date getTimeT() const;
    //! Returns the number of MonteCarlo iterations
    size_t getMaxIterations() const;
    //! Returns the maximum execution time
    size_t getMaxSeconds() const;
    //! Returns copula
    std::string getCopula() const;
    //! Returns the t-student copula degrees of freedom
    double getNdf() const;
    //! Returns RNG seed
    unsigned long getRngSeed() const;
    //! Returns antithetic flag
    bool getAntithetic() const;
    //! Returns simulation block size
    unsigned short getBlockSize() const;
    //! Validate object content
    bool isValid(bool throwException=false) const;

};

} // namespace

#endif

