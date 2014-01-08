
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

#ifndef _DateValues_
#define _DateValues_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cmath>
#include "utils/Date.hpp"
#include "portfolio/EAD.hpp"
#include "portfolio/LGD.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class DateValues
{

  public:

    // date where values take place
    Date date;
    // exposure at default
    EAD ead;
    // loss given default
    LGD lgd;

  public:

    // constructor
    explicit DateValues(Date d=Date(), const EAD &e=EAD(), const LGD &r=LGD());
    // less-than operator
    bool operator<(const DateValues &) const;
    // less-than operator
    bool operator<(const Date &) const;

};

//---------------------------------------------------------------------------

//===========================================================================
// constructor
//===========================================================================
inline ccruncher::DateValues::DateValues(Date d, const EAD &e, const LGD &r) :
    date(d), ead(e), lgd(r)
{
  // nothing to do
}

//===========================================================================
// less-than operador (needed by sort functions)
//===========================================================================
inline bool ccruncher::DateValues::operator<(const DateValues &right) const
{
  return (date < right.date);
}

//===========================================================================
// less-than operador (needed by sort functions)
//===========================================================================
inline bool ccruncher::DateValues::operator<(const Date &right) const
{
  return (date < right);
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
