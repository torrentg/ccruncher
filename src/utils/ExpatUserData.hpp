
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

#ifndef _ExpatUserData_
#define _ExpatUserData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/ExpatHandlers.hpp"
#include <vector>
#include <string>
#include <expat.h>

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

// forward declaration
class ExpatHandlers;

//---------------------------------------------------------------------------

class ExpatUserData
{

  private:

    // internal struct
    struct ExpatUserDataToken
    {
        // token name related to handler
        char name[20];
        // pointer to class handlers container
        ExpatHandlers *handlers;
    };

  private:

    // expat xml parser
    XML_Parser xmlparser;
    // stack of handlers
    vector<ExpatUserDataToken> pila;
    // current handler
    int pos;
    // current tag (used by ExpatParser)
    const char *current_tag;

  public:

    // void constructor
    ExpatUserData();
    // contructor
    ExpatUserData(XML_Parser xmlparser_);
    // destructor
    ~ExpatUserData();
    // returns parser
    XML_Parser getParser();
    // returns current handlers
    ExpatHandlers* getCurrentHandlers();
    // returns current name
    const char* getCurrentName() const;
    // removeCurrentHandlers
    void removeCurrentHandlers();
    // setCurrentHandlers
    void setCurrentHandlers(const char *name, ExpatHandlers *eh);
    // set current tag name
    void setCurrentTag(const char *t) { current_tag = t; }
    // returns current tag name
    const char *getCurrentTag() const { return current_tag; }

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
