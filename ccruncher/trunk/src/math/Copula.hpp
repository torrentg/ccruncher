
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
// Copula.hpp - Copula header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2006/01/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added setSeed abstract method
//
//===========================================================================

#ifndef _Copula_
#define _Copula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Copula
{

  public:

    virtual int size() = 0;
    virtual void next() = 0;
    virtual double get(int) = 0;
    virtual void setSeed(long) = 0;

    virtual ~Copula() {};
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
