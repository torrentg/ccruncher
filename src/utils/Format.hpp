
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

#ifndef _Format_
#define _Format_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Format
{

  private:

    // non-instantiable class
    Format() {};

  public:

    // converts int to string
    static string toString(const int val);
    // converts long to string
    static string toString(const long val);
    // converts double to string
    static string toString(const double val);
    // converts date to string
    static string toString(const ccruncher::Date &val);
    // converts boolean to string
    static string toString(const bool &val);
    // converts unsigned int to string
    static string toString(const unsigned int &val);
    // converts size_t to string
    static string toString(const size_t &val);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
