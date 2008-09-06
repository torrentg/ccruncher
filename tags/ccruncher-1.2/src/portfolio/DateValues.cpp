
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
//
// DateValues.cpp - DateValues code - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/03/16 - Gerard Torrent [gerard@mail.generacio.com]
//   . added recovery value
//
// 2005/04/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from xerces to expat
//
// 2005/07/09 - Gerard Torrent [gerard@mail.generacio.com]
//   . changed exposure/recovery by netting
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/05 - Gerard Torrent [gerard@mail.generacio.com]
//   . netting replaced by recovery
//
//===========================================================================

#include <cmath>
#include <cfloat>
#include "portfolio/DateValues.hpp"

//===========================================================================
// constructor
//===========================================================================
ccruncher::DateValues::DateValues()
{
  date = Date(1,1,1);
  cashflow = NAN;
  recovery = NAN;
}

//===========================================================================
// constructor
//===========================================================================
ccruncher::DateValues::DateValues(Date _date, double _cashflow, double _recovery)
{
  date = _date;
  cashflow = _cashflow;
  recovery = _recovery;
}

//===========================================================================
// comparation operador (needed by sort functions)
//===========================================================================
bool ccruncher::operator <  (const DateValues &x, const DateValues &y)
{
  return (x.date < y.date);
}
