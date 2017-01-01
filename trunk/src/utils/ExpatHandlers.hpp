
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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
#include "utils/MacrosBuffer.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

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

    //! Macros buffer
    MacrosBuffer macros;

  protected:

    //! Stops the parser
    void epstop();
    //! String comparison
    bool isEqual(const char *, const char *) const;
    //! Returns the number of attributes
    int getNumAttributes(const char **atts) const;
    //! Returns the value of the given attribute
    const char * getAttributeValue(const char **atts, const char *attname);
    //! Returns the value of the given attribute
    const char * getAttributeValue(const char **atts, const char *attname, const char *defval);
    //! Returns attribute value as string
    std::string getStringAttribute(const char **atts, const char *attname);
    //! Returns attribute value as string
    std::string getStringAttribute(const char **atts, const char *attname, const std::string &defval);
    //! Returns attribute value as int
    int getIntAttribute(const char **atts, const char *attname);
    //! Returns attribute value as int
    int getIntAttribute(const char **atts, const char *attname, int defval);
    //! Returns attribute value as long
    long getLongAttribute(const char **atts, const char *attname);
    //! Returns attribute value as long
    long getLongAttribute(const char **atts, const char *attname, long defval);
    //! Returns attribute value as double
    double getDoubleAttribute(const char **atts, const char *attname);
    //! Returns attribute value as double
    double getDoubleAttribute(const char **atts, const char *attname, double defval);
    //! Returns attribute value as date
    Date getDateAttribute(const char **atts, const char *attname);
    //! Returns attribute value as date
    Date getDateAttribute(const char **atts, const char *attname, const Date &defval);
    //! Returns attribute value as boolean
    bool getBooleanAttribute(const char **atts, const char *attname);
    //! Returns attribute value as boolean
    bool getBooleanAttribute(const char **atts, const char *attname, bool defval);

  protected:
  
    //! Handler for open tag
    virtual void epstart(const char *tag, const char **atts) = 0;
    //! Handler for closed tag
    virtual void epend(const char *) {}
    //! Handler for data
    virtual void epdata(const char *cdata, int len);
    //! Constructor
    ExpatHandlers() {}
    //! Destructor
    virtual ~ExpatHandlers() {}
    
  public:
  
    //! Friend class
    friend class ExpatParser;

};

} // namespace

#endif

