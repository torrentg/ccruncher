
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

#ifndef _Recovery_
#define _Recovery_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

enum RecoveryType
{ 
  Fixed,
  Beta
};

//---------------------------------------------------------------------------

class Recovery
{

  public:

    // recovery type
    RecoveryType type;
    // type=Fixed->recovery , type=Beta->alpha
    double value1;
    // type=Beta->beta
    double value2;

  public:
  
    // default constructor
    Recovery();
    // constructor
    Recovery(const char *) throw(Exception);
    // constructor
    Recovery(const string &) throw(Exception);
    // constructor
    Recovery(RecoveryType, double, double);
    // constructor
    Recovery(double fixedvalue);
    // constructor
    Recovery(double alpha, double beta);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
