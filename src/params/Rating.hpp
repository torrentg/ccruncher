
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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

#pragma once

#include <string>

namespace ccruncher {

/**************************************************************************//**
 * @brief Object representing a rating.
 *
 * @see http://ccruncher.net/ifileref.html#ratings
 */
class Rating
{

  public:

    //! Rating name
    std::string name;
    //! Rating description
    std::string description;

  public:

    //! Constructor
    Rating(const std::string &nam="", const std::string &desc="") : name(nam), description(desc) {}

};

} // namespace
