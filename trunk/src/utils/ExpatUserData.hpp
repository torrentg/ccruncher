
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

#ifndef _ExpatUserData_
#define _ExpatUserData_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/ExpatHandlers.hpp"
#include <stack>
#include <string>
#include <expat.h>

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

class ExpatHandlers; // defined in file ExpatHandlers.hpp

//---------------------------------------------------------------------------

// forward declaration
class ExpatHandlers;

//---------------------------------------------------------------------------

class ExpatUserData
{

  private:

    // internal class
    class ExpatUserDataToken
    {
      public:

        // token name related to handler
        string name;
        // pointer to class handlers container
        ExpatHandlers *handlers;

        // constructor
        ExpatUserDataToken(string &n, ExpatHandlers *h)
        {
          name = n;
          handlers = h;
        }
    };

  private:

    // expat xml parser
    XML_Parser xmlparser;
    // stack of handlers
    stack<ExpatUserDataToken> pila;

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
    string & getCurrentName();
    // removeCurrentHandlers
    void removeCurrentHandlers();
    // setCurrentHandlers
    void setCurrentHandlers(string name, ExpatHandlers *eh);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
