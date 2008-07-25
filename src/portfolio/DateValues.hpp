
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
// DateValues.hpp - DateValues header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/16 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added recovery value
//
// 2005/03/18 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added constructor based on DOMNode
//
// 2005/04/02 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/07/09 - Gerard Torrent [gerard@fobos.generacio.com]
//   . changed exposure/recovery by netting
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#ifndef _DateValues_
#define _DateValues_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class DateValues
{

  public:

    Date date;
    double cashflow;
    double netting;

    DateValues();
    DateValues(Date date, double cashflow, double netting);

};

//---------------------------------------------------------------------------

// comparation operator
bool operator <  (const DateValues&, const DateValues&);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
