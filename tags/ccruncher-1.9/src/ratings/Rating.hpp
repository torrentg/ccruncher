
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

#ifndef _Rating_
#define _Rating_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/Exception.hpp"
#include "utils/ExpatHandlers.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Rating : public ExpatHandlers
{

  public:

    // rating name
    string name;
    // rating description
    string desc;

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // rating constructor
    Rating();
    // serialize object content as xml
    string getXML(int) const throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------