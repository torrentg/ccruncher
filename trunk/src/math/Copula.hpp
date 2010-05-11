
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#ifndef _Copula_
#define _Copula_

//---------------------------------------------------------------------------

#include "utils/config.h"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Copula
{

  public:

    // copula size
    virtual int size() = 0;
    // simulates a copula realization
    virtual void next() = 0;
    // returns i-th component
    virtual double get(int) = 0;
    // random number generator seed
    virtual void setSeed(long) = 0;
    // object replication
    virtual Copula* clone() = 0;
    // destructor
    virtual ~Copula() {};

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
