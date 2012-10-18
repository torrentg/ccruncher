
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include <cmath>
#include "portfolio/DateValues.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::DateValues::DateValues()
{
  date = NAD;
  exposure = Exposure(Exposure::Fixed,NAN);
  recovery = Recovery(Recovery::Fixed,NAN);
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::DateValues::DateValues(Date date_, const Exposure &exposure_, const Recovery &recovery_)
{
  date = date_;
  exposure = exposure_;
  recovery = recovery_;
}

