
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#include "portfolio/Obligor.hpp"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * Checks if any of its assets is active in the given time range.
 * @param[in] from Starting simulation date.
 * @param[in] to Ending simulation date.
 */
bool ccruncher::Obligor::isActive(const Date &from, const Date &to) const
{
  for(const Asset &asset : assets) {
    if (asset.isActive(from,to)) {
      return true;
    }
  }
  return false;
}

