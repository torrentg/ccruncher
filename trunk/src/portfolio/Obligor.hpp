
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

#ifndef _Obligor_
#define _Obligor_

#include <vector>
#include "portfolio/Asset.hpp"
#include "portfolio/LGD.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief  Obligor with a rating and a list of assets.
 *
 * @details Obligors assigns its belongs-to segments to his assets.
 *
 * @see http://ccruncher.net/ifileref.html#portfolio
 */
class Obligor
{

  public:

    //! Obligor's factor index
    unsigned char ifactor;
    //! Obligor's rating index
    unsigned char irating;
    //! Number of active assets
    std::vector<Asset> assets;
    //! Obligor's lgd
    LGD lgd;

  public:

    //! Constructor
    Obligor() : ifactor(0), irating(0) {}
    //! Indicates if this obligor has values in date1-date2
    bool isActive(const Date &, const Date &) const;

};

} // namespace

#endif

