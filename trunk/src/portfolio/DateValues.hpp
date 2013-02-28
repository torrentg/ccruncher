
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
#include "portfolio/Exposure.hpp"
#include "portfolio/Recovery.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class DateValues
{

  public:

    // date where values take place
    Date date;
    // exposure value
    Exposure exposure;
    // recovery value
    Recovery recovery;

  public:

    // constructor
    DateValues(Date d=NAD, const Exposure &e=Exposure(Exposure::Fixed,NAN), const Recovery &r=Recovery(Recovery::Fixed,NAN));
    // less-than operator
    bool operator<(const DateValues &) const;

};

//---------------------------------------------------------------------------

//===========================================================================
// constructor
//===========================================================================
inline ccruncher::DateValues::DateValues(Date d, const Exposure &e, const Recovery &r) :
    date(d), exposure(e), recovery(r)
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

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
