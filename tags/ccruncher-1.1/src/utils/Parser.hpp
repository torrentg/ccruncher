
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
// Parser.hpp - Parser header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/03/31 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added support for char * type
//
// 2005/07/21 - Gerard Torrent [gerard@fobos.generacio.com]
//   . format methods segregated to Format class
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added private constructor (non-instantiable class)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#ifndef _Parser_
#define _Parser_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Parser
{

  private:

    // non-instantiable class
    Parser() {};


  public:

    static int intValue(const string &) throw(Exception);
    static long longValue(const string &) throw(Exception);
    static double doubleValue(const string &) throw(Exception);
    static Date dateValue(const string &) throw(Exception);
    static bool boolValue(const string &) throw(Exception);

    static int intValue(const char *) throw(Exception);
    static long longValue(const char *) throw(Exception);
    static double doubleValue(const char *) throw(Exception);
    static Date dateValue(const char *) throw(Exception);
    static bool boolValue(const char *) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------