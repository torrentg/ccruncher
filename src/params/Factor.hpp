
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#ifndef _Factor_
#define _Factor_

#include <string>
#include <cmath>

namespace ccruncher {

/**************************************************************************//**
 * @brief Object representing a factor/sector.
 *
 * @see http://ccruncher.net/ifileref.html#factors
 */
class Factor
{

  public:

    //! Factor name
    std::string name;
    //! Factor description
    std::string description;
    //! Factor loading
    double loading;

  public:

    //! Constructor (to use in initializer list constructors)
    Factor(const char *nam) : name(nam), description(""), loading(NAN) {}
    //! Constructor
    Factor(const std::string &nam="", double load=NAN, const std::string &desc="")
      : name(nam), description(desc), loading(load) {}

};

} // namespace

#endif

