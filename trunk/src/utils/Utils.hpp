
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
// Utils.hpp - Utils header - $Rev$
// --------------------------------------------------------------------------
//
// 2005/07/19 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/07/24 - Gerard Torrent [gerard@mail.generacio.com]
//   . added hash() method
//   . added timestamp() method
//
// 2005/08/06 - Gerard Torrent [gerard@mail.generacio.com]
//   . added method getCompilationOptions()
//
// 2005/08/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . added private constructor (non-instantiable class)
//   . added rankid static variable (to avoid rank reorder when exits a proces)
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#ifndef _Utils_
#define _Utils_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/Exception.hpp"
#include <fstream>

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Utils
{
  private:

    // non-instantiable class
    Utils() {};

    // pointer to /dev/null
    static ofstream *nullstream;
    // mpi rank id
    static int rankid;


  public:

    static bool isMaster();
    static void setSilentMode() throw(Exception);
    static unsigned long hash(const string &str);
    static string timestamp();
    static string getCompilationOptions();
    static long trand();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
