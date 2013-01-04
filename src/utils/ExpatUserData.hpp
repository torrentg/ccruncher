
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

#ifndef _ExpatUserData_
#define _ExpatUserData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/ExpatHandlers.hpp"
#include <map>
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
    int pila_pos;
    // current tag (used by ExpatParser)
    const char *current_tag;
    // buffer used to apply defines
    char *buffer;
    // buffer size
    size_t buffer_size;
    // points to first char in current buffer
    char *buffer_pos1;
    // points to last char in current buffer
    char *buffer_pos2;

  private:

    const char* bufferPush(const char *str, size_t n);
    const char* bufferAppend(const char *str, size_t n);

  public:

    // user replaces
    map<string,string> defines;
    // apply defines to the given string
    const char* applyDefines(const char *str);

  public:

    // void constructor
    ExpatUserData(size_t buffersize);
    // contructor
    ExpatUserData(XML_Parser xmlparser_, size_t buffersize);
    // destructor
    ~ExpatUserData();
    // assignment operator
    ExpatUserData & operator= (const ExpatUserData &);
    // returns parser
    XML_Parser getParser() const { return xmlparser; }
    // set parser
    void setParser(XML_Parser p) { xmlparser = p; }
    // returns current handlers
    ExpatHandlers* getCurrentHandlers() const { return pila[pila_pos].handlers; }
    // returns current name
    const char* getCurrentName() const { return pila[pila_pos].name; }
    // removeCurrentHandlers
    void removeCurrentHandlers() { pila_pos--; }
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
