
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

class FindDefines : public ccruncher::ExpatHandlers
{

  private:

    // defines found
    std::map<std::string,std::string> defines;

  protected:

    // ExpatHandlers method
    void epstart(ccruncher::ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ccruncher::ExpatUserData &, const char *);
    // ExpatHandlers method
    void epdata(ccruncher::ExpatUserData &, const char *, const char *, int);

  public:

    // constructor
    FindDefines(const std::string &);
    // return defines
    const std::map<std::string,std::string> &getDefines() const;

};

}

#endif

