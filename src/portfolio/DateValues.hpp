
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

#include "portfolio/EAD.hpp"
#include "portfolio/LGD.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Triplet (Date, EAD, LGD).
 *
 * @details Exposure At Default ($) and Loss Given Default (%) values
 *          estimated at fixed data.
 *
 * @see http://ccruncher.net/ifileref.html#portfolio (element values)
 */
class DateValues
{

  public:

    //! Date where values take place
    Date date;
    //! Exposure at default
    EAD ead;
    //! Loss given default
    LGD lgd;

  public:

    //! Constructor
    explicit DateValues(const Date &d=Date(), const EAD &e=EAD(), const LGD &l=LGD());
    //! Less-than operator
    bool operator<(const DateValues &) const;
    //! Less-than operator
    bool operator<(const Date &) const;
    //! Comparison operator
    bool operator==(const DateValues &o) const;
    //! Comparison operator
    bool operator!=(const DateValues &o) const;

};

/**************************************************************************//**
 * @details Operator required by sort functions.
 * @param[in] d DateValue date.
 * @param[in] e DateValue exposure.
 * @param[in] l DateValue loss given default.
 */
inline ccruncher::DateValues::DateValues(const Date &d, const EAD &e, const LGD &l) :
    date(d), ead(e), lgd(l)
{
  // nothing to do
}

/**************************************************************************//**
 * @details Operator required by sort functions.
 * @param[in] right Objecto to compare.
 * @return true if date < object's date.
 */
inline bool ccruncher::DateValues::operator<(const DateValues &right) const
{
  return (date < right.date);
}

/**************************************************************************//**
 * @details Operator required by sort functions.
 * @param[in] right Object to compare.
 * @return true if date < object's date.
 */
inline bool ccruncher::DateValues::operator<(const Date &right) const
{
  return (date < right);
}

/**************************************************************************//**
 * @param[in] o Instance to compare.
 * @return true=are equal, false=otherwise.
 */
inline bool ccruncher::DateValues::operator==(const DateValues &o) const
{
  if (date != o.date) return false;
  else if (ead != o.ead) return false;
  else if (lgd != o.lgd) return false;
  else return true;
}

/**************************************************************************//**
 * @param[in] o EAD instance to compare.
 * @return true=are distinct, false=otherwise.
 */
inline bool ccruncher::DateValues::operator!=(const DateValues &o) const
{
  return !(*this == o);
}

} // namespace
