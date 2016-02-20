
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#ifndef _ExpatHandlers_
#define _ExpatHandlers_

#include <string>
#include <cstring>
#include "utils/ExpatUserData.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

// forward declaration
class ExpatUserData;

/**************************************************************************//**
 * @brief   Base class providing methods to interact with the XML parser.
 *
 * @details Expat is a stream-oriented parser. You register callback (or
 *          handler) functions with the parser and then start feeding it the
 *          document. As the parser recognizes parts of the document, it will
 *          call the appropriate handler for that part (if you've registered
 *          one). The document is fed to the parser in pieces, so you can
 *          start parsing before you have the whole document. This also allows
 *          you to parse really huge documents that won't fit into memory.
 *          This class provides support to define these handlers and methods
 *          to interact with the XML parser.
 *
 * @see     http://expat.sourceforge.net/
 * @see     http://www.xml.com/pub/a/1999/09/expat/index.html
 */
class ExpatHandlers
{

  protected:

    //! Removes current handlers from stack
    void epback(ExpatUserData &eud);
    //! Push new handlers to stack
    void eppush(ExpatUserData &eud, ExpatHandlers *eh, const char *name, const char **atts);
    //! Stops the parser
    void epstop(ExpatUserData &eud);
    //! String comparison
    bool isEqual(const char *, const char *) const;
    //! Returns the number of attributes
    int getNumAttributes(const char **atts) const;
    //! Returns the value of the given attribute
    const char * getAttributeValue(const char **atts, const char *attname) const;
    //! Returns the value of the given attribute
    const char * getAttributeValue(const char **atts, const char *attname, const char *defval) const;
    //! Returns attribute value as string
    std::string getStringAttribute(const char **atts, const char *attname) const;
    //! Returns attribute value as string
    std::string getStringAttribute(const char **atts, const char *attname, const std::string &defval) const;
    //! Returns attribute value as int
    int getIntAttribute(const char **atts, const char *attname) const;
    //! Returns attribute value as int
    int getIntAttribute(const char **atts, const char *attname, int defval) const;
    //! Returns attribute value as long
    long getLongAttribute(const char **atts, const char *attname) const;
    //! Returns attribute value as long
    long getLongAttribute(const char **atts, const char *attname, long defval) const;
    //! Returns attribute value as double
    double getDoubleAttribute(const char **atts, const char *attname) const;
    //! Returns attribute value as double
    double getDoubleAttribute(const char **atts, const char *attname, double defval) const;
    //! Returns attribute value as date
    Date getDateAttribute(const char **atts, const char *attname) const;
    //! Returns attribute value as date
    Date getDateAttribute(const char **atts, const char *attname, const Date &defval) const;
    //! Returns attribute value as boolean
    bool getBooleanAttribute(const char **atts, const char *attname) const;
    //! Returns attribute value as boolean
    bool getBooleanAttribute(const char **atts, const char *attname, bool defval) const;

  protected:
  
    //! Handler for open tag
    virtual void epstart(ExpatUserData &eud, const char *tag, const char **atts) = 0;
    //! Handler for closed tag
    virtual void epend(ExpatUserData &/*eud*/, const char */*name*/) {}
    //! Handler for data
    virtual void epdata(ExpatUserData &eud, const char *tag, const char *cdata, int len);
    //! Constructor
    ExpatHandlers() {}
    //! Destructor
    virtual ~ExpatHandlers() {}
    
  public:
  
    //! Friend class
    friend class ExpatParser;

};

/**************************************************************************//**
 * @details Compares 2 strings taking into accounts NULL values.
 *          This method is widely used to check parameter names.
 * @see http://www.cplusplus.com/reference/cstring/strcmp/
 * @return true=both strings are equal, false=otherwise.
 */
inline bool ccruncher::ExpatHandlers::isEqual(const char *pchr, const char *str) const
{
  if (pchr != nullptr && str != nullptr)
  {
    return (std::strcmp(str, pchr) == 0);
  }
  else
  {
    return false;
  }
}

} // namespace

#endif

