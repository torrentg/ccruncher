
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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
#include "utils/ExpatUserData.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

class ExpatUserData; // defined in file ExpatUserData.hpp

//---------------------------------------------------------------------------

class ExpatHandlers
{

  private:

    // returns the value of the given attribute
    char * getAttributeValue(const char **atts, const string &attname) const;

  protected:

    // removes current handlers from stack
    void epback(ExpatUserData &eud);
    // push handlers to stack
    void eppush(ExpatUserData &eud, ExpatHandlers *eh, const char *name, const char **atts);
    // stops the parser
    void epstop(ExpatUserData &eud);
    // string comparison
    bool isEqual(const char *, const string &);
    // returns the number of attributes
    int getNumAttributes(const char **atts);
    // returns attribute value as string
    string getStringAttribute(const char **atts, const string &attname, const string &defval);
    // returns attribute value as int
    int getIntAttribute(const char **atts, const string &attname, const int &defval);
    // returns attribute value as long
    long getLongAttribute(const char **atts, const string &attname, const long &defval);
    // returns attribute value as double
    double getDoubleAttribute(const char **atts, const string &attname, const double &defval);
    // returns attribute value as date
    Date getDateAttribute(const char **atts, const string &attname, const Date &defval);
    // returns attribute value as boolean
    bool getBooleanAttribute(const char **atts, const string &attname, const bool &defval);

  public:

    // destructor
    virtual ~ExpatHandlers();
    // handler for open tag
    virtual void epstart(ExpatUserData &eud, const char *name, const char **atts) = 0;
    // handler for closed tag
    virtual void epend(ExpatUserData &eud, const char *name) = 0;
    // handler for data
    virtual void epdata(ExpatUserData &eud, const char *name, const char *cdata, int len);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
