
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

#ifndef _ExpatParser_
#define _ExpatParser_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <expat.h>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class ExpatParser
{

  private:

    // parser expat object
    XML_Parser xmlparser;
    // user data
    ExpatUserData userdata;
    // current tag (internal use)
    static const char *current_tag;

  private:
  
    // startElement function catcher
    static void startElement(void *ud, const char *name, const char **atts);
    // endElement function catcher
    static void endElement(void *ud, const char *name);
    // characterData Handler function
    static void characterData(void *ud, const char *s, int len) throw(Exception);
    // reset internal variables
    void reset();

  public:

    // constructor
    ExpatParser();
    // destructor
    ~ExpatParser();
    // parse xml
    void parse(const string &xmlcontent, ExpatHandlers *eh) throw(Exception);
    // parse xml
    void parse(istream &xmlcontent, ExpatHandlers *eh) throw(Exception);
    // returns main object
    void * getObject();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------