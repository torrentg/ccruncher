
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

#ifndef _ExpatHandlers_
#define _ExpatHandlers_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <cstring>
#include "utils/ExpatUserData.hpp"
#include "utils/Exception.hpp"
#include "utils/Date.hpp"
#include <cassert>

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

// forward declaration
class ExpatUserData;

//---------------------------------------------------------------------------

class ExpatHandlers
{

  protected:

    // removes current handlers from stack
    void epback(ExpatUserData &eud);
    // push handlers to stack
    void eppush(ExpatUserData &eud, ExpatHandlers *eh, const char *name, const char **atts);
    // stops the parser
    void epstop(ExpatUserData &eud);
    // string comparison
    bool isEqual(const char *, const char *) const;
    // returns the number of attributes
    int getNumAttributes(const char **atts) const;
    // returns the value of the given attribute
    const char * getAttributeValue(const char **atts, const char *attname) const throw(Exception);
    const char * getAttributeValue(const char **atts, const char *attname, const char *defval) const;
    // returns attribute value as string
    std::string getStringAttribute(const char **atts, const char *attname) const throw(Exception);
    std::string getStringAttribute(const char **atts, const char *attname, const std::string &defval) const;
    // returns attribute value as int
    int getIntAttribute(const char **atts, const char *attname) const throw(Exception);
    int getIntAttribute(const char **atts, const char *attname, int defval) const;
    // returns attribute value as long
    long getLongAttribute(const char **atts, const char *attname) const throw(Exception);
    long getLongAttribute(const char **atts, const char *attname, long defval) const;
    // returns attribute value as double
    double getDoubleAttribute(const char **atts, const char *attname) const throw(Exception);
    double getDoubleAttribute(const char **atts, const char *attname, double defval) const;
    // returns attribute value as date
    Date getDateAttribute(const char **atts, const char *attname) const throw(Exception);
    Date getDateAttribute(const char **atts, const char *attname, const Date &defval) const;
    // returns attribute value as boolean
    bool getBooleanAttribute(const char **atts, const char *attname) const throw(Exception);
    bool getBooleanAttribute(const char **atts, const char *attname, bool defval) const;

  protected:
  
    // handler for open tag
    virtual void epstart(ExpatUserData &eud, const char *name, const char **atts) = 0;
    // handler for closed tag
    virtual void epend(ExpatUserData &/*eud*/, const char */*name*/) {}
    // handler for data
    virtual void epdata(ExpatUserData &eud, const char *name, const char *cdata, int len);

  public:

    // destructor
    virtual ~ExpatHandlers();
    
  public:
  
    // friend class
    friend class ExpatParser;

};

//---------------------------------------------------------------------------

//===========================================================================
// isEqual
//===========================================================================
inline bool ccruncher::ExpatHandlers::isEqual(const char *pchr, const char *str) const
{
  assert(str != NULL);

  if (pchr != NULL)
  {
    return (std::strcmp(str, pchr) == 0);
  }
  else
  {
    return false;
  }
}

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
