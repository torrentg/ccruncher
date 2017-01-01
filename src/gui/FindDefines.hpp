
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#ifndef _FindDefines_
#define _FindDefines_

#include <map>
#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

namespace ccruncher_gui {

/**************************************************************************//**
 * @brief Retrieve the defines from a CCruncher input file.
 */
class FindDefines : public ccruncher::ExpatHandlers
{

  private:

    //! List of defines
    std::map<std::string,std::string> defines;

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(const char *) override;
    //! Directives to process xml data element
    virtual void epdata(const char *, int) override;

  public:

    //! Constructor
    FindDefines(const std::string &);
    //! Return defines
    const std::map<std::string,std::string> &getDefines() const;

};

} // namespace

#endif

