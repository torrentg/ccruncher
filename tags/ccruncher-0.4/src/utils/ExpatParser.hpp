
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
// ExpatParser.hpp - ExpatParser header
// --------------------------------------------------------------------------
//
// 2005/03/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/06/10 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added characterData method
//
//===========================================================================

#ifndef _ExpatParser_
#define _ExpatParser_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <expat.h>
#include <iostream>
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

    /** parser expat object */
    XML_Parser xmlparser;

    /** user data */
    ExpatUserData userdata;

    /** startElement function catcher */
    static void XMLCALL startElement(void *ud, const char *name, const char **atts);

    /** endElement function catcher */
    static void XMLCALL endElement(void *ud, const char *name);

    /** characterData Handler function */
    static void XMLCALL characterData(void *ud, const char *s, int len) throw(Exception);

  public:

    /** constructor (xml=string) */
    ExpatParser();

    /** destructor */
    ~ExpatParser();

    /** parse xml */
    void parse(string xmlcontent, ExpatHandlers *eh) throw(Exception);

    /** parse xml */
    void parse(istream &xmlcontent, ExpatHandlers *eh) throw(Exception);

    /** returns main object */
    void * getObject();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
