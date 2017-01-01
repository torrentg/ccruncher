
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

#ifndef _Asset_
#define _Asset_

#include <vector>
#include "portfolio/DateValues.hpp"
#include "params/Interest.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Financial asset with credit risk.
 *
 * @details An asset is a list of DateValues (date-ead-lgd) sorted by date
 *          and a list of segmentation-segment indexes. It is your
 *          responsability to mantain these list with valid values.
 *          We recommend to inform the creation date as the first DateValue
 *          with ead=0 and lgd=0.
 *
 * @see http://ccruncher.net/ifileref.html#portfolio
 */
class Asset
{

  public:

    //! Asset datevalues
    std::vector<DateValues> values;
    //! Segmentation-segment relations
    std::vector<unsigned short> segments;

  public:

    //! Constructor
    Asset(unsigned short nsegmentations=0) : segments(nsegmentations, 0) {}
    //! Constructor
    Asset(const std::vector<unsigned short> &segments_) : segments(segments_) {}
    //! Prepare data
    void prepare(const Date &d1, const Date &d2, const Interest &interest);
    //! Indicates if this asset has info in date1-date2
    bool isActive(const Date &from, const Date &to) const;

};

} // namespace

#endif

