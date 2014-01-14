
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#include <map>
#include <vector>
#include <string>
#include <expat.h>
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

// forward declaration
class ExpatHandlers;

/**************************************************************************//**
 * @brief   Data relative to an XML parsing.
 *
 * @details Expat parser library calls static handlers defined in
 *          ExpatParser. These methods retrieve current parsing info from
 *          this class. Data relative to an XML parsing is:
 *            - stack of ExpatHandlers
 *            - defines (data and methods)
 *            - current element name (can differ from ExpatHandlers element)
 *
 * @see     http://expat.sourceforge.net/
 * @see     http://www.xml.com/pub/a/1999/09/expat/index.html
 */
class ExpatUserData
{

  private:

    //! Internal struct
    struct ExpatUserDataToken
    {
      //! Element name
      char name[20];
      //! Element handlers
      ExpatHandlers *handlers;
    };

  private:

    //! Stack of handlers
    std::vector<ExpatUserDataToken> pila;
    //! Stack position (current element)
    int pila_pos;
    //! Current element name
    const char *current_tag;
    //! Buffer used to apply defines
    char *buffer;
    //! Buffer size
    size_t buffer_size;
    //! Points to first char in current buffer
    char *buffer_pos1;
    //! Points to last char in current buffer
    char *buffer_pos2;

  private:

    //! Push str to buffer
    const char* bufferPush(const char *str, size_t n);
    //! Append str to buffer
    const char* bufferAppend(const char *str, size_t n);

  public:

    //! List of defines
    std::map<std::string,std::string> defines;
    //! Apply defines to the given string
    const char* applyDefines(const char *str);

  public:

    //! Constructor
    ExpatUserData(size_t buffersize=4096);
    //! Copy constructor
    ExpatUserData(const ExpatUserData &);
    //! Destructor
    ~ExpatUserData();
    //! Assignment operator
    ExpatUserData & operator= (const ExpatUserData &);
    //! Returns current handlers
    ExpatHandlers* getCurrentHandlers() const { return pila[pila_pos].handlers; }
    //! Returns current handlers element name
    const char* getCurrentName() const { return pila[pila_pos].name; }
    //! Remove current handlers
    void removeCurrentHandlers() { pila_pos--; }
    //! Set current handlers
    void setCurrentHandlers(const char *name, ExpatHandlers *eh);
    //! Set current tag name
    void setCurrentTag(const char *t) { current_tag = t; }
    //! Returns current tag name
    const char *getCurrentTag() const { return current_tag; }

};

} // namespace

#endif

