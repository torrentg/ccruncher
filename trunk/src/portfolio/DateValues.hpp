
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

#ifndef _DateValues_
#define _DateValues_

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
    explicit DateValues(Date d=Date(), const EAD &e=EAD(), const LGD &l=LGD());
    //! Less-than operator
    bool operator<(const DateValues &) const;
    //! Less-than operator
    bool operator<(const Date &) const;

};

/**************************************************************************//**
 * @details Operator required by sort functions.
 * @param[in] d DateValue date.
 * @param[in] e DateValue exposure.
 * @param[in] l DateValue loss given default.
 */
inline ccruncher::DateValues::DateValues(Date d, const EAD &e, const LGD &l) :
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

} // namespace


#endif

