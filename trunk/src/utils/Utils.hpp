
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

#ifndef _Utils_
#define _Utils_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Utils
{

  private:
  
    // non-instantiable class
    Utils() {}

  public:

    // returns current timestamp in format dd/mm/yyyy hh:mm:ss
    static string timestamp();
    // returns compilation options
    static string getCompilationOptions();
    // returns random value based on current time
    static unsigned long trand();
    // return the number of cores
    static int getNumCores();
    // copyright info
    static string copyright();
    // version info
    static string version();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
