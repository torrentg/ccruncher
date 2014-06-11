
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

#include <cassert>
#include "params/Factor.hpp"

using namespace std;

/**************************************************************************//**
 * @param[in] name Factor name.
 * @param[in] desc Factor description.
 * @param[in] loading Factor loading.
 */
ccruncher::Factor::Factor(const string &name, double loading, const string &desc) :
    mName(name), mDesc(desc), mLoading(loading)
{
  // nothing to do
}

/**************************************************************************//**
 * @param[in] name Factor name.
 */
void ccruncher::Factor::setName(const string &name)
{
  mName = name;
}

/**************************************************************************//**
 * @param[in] desc Factor description.
 */
void ccruncher::Factor::setDescription(const string &desc)
{
  mDesc = desc;
}

/**************************************************************************//**
 * @param[in] loading Factor loading.
 */
void ccruncher::Factor::setLoading(double loading)
{
  mLoading = loading;
}

/**************************************************************************//**
 * @return Factor name.
 */
const std::string& ccruncher::Factor::getName() const
{
  return mName;
}

/**************************************************************************//**
 * @return Factor description.
 */
const std::string& ccruncher::Factor::getDescription() const
{
  return mDesc;
}

/**************************************************************************//**
 * @return Factor loading.
 */
double ccruncher::Factor::getLoading() const
{
  return mLoading;
}

