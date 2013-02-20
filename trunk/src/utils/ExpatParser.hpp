
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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
#include <zlib.h>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class ExpatParser
{

  private:

    // parser expat object
    XML_Parser xmlparser;
    // user data
    ExpatUserData userdata;

  private:

    // parse xml
    void parse(gzFile file, char *buf, size_t buffer_size, bool *stop) throw(Exception);
    // startElement function catcher
    static void startElement(void *ud, const char *name, const char **atts);
    // endElement function catcher
    static void endElement(void *ud, const char *name);
    // characterData Handler function
    static void characterData(void *ud, const char *s, int len);
    // reset internal variables
    void reset();

  public:

    // constructor
    ExpatParser();
    // destructor
    ~ExpatParser();
    // parse xml
    void parse(const std::string &xmlcontent, ExpatHandlers *eh, bool *stop=NULL) throw(Exception);
    // parse xml
    void parse(gzFile file, ExpatHandlers *eh, bool *stop=NULL) throw(Exception);
    // returns main object
    void * getObject();
    // returns defines
    const std::map<std::string,std::string>& getDefines() const;
    // set defines
    void setDefines(const std::map<std::string,std::string>&);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
