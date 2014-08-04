
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

#include <cassert>
#include "params/Rating.hpp"

using namespace std;

/**************************************************************************//**
 * @param[in] name Rating name.
 * @param[in] desc Rating description.
 */
ccruncher::Rating::Rating(const string &name, const string &desc) :
    mName(name), mDesc(desc)
{
  // nothing to do
}

/**************************************************************************//**
 * @param[in] name Rating name.
 */
void ccruncher::Rating::setName(const string &name)
{
  mName = name;
}

/**************************************************************************//**
 * @param[in] desc Rating description.
 */
void ccruncher::Rating::setDescription(const string &desc)
{
  mDesc = desc;
}

