
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
// XMLUtils.hpp - XMLUtils header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _XMLUtils_
#define _XMLUtils_

//---------------------------------------------------------------------------

#include <string>
#include "xercesc/dom/DOM.hpp"
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include "Date.hpp"
#include "Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class XMLUtils
{

  public:

    static void initialize() throw(Exception);
    static void terminate() throw();
    static DOMBuilder * getParser() throw(Exception);
    static Wrapper4InputSource * getInputSource(const string &xmlcontent) throw(Exception);
    static DOMDocument * getDocument(DOMBuilder *, const string &) throw(Exception);
    static DOMDocument * getDocument(DOMBuilder *parser, Wrapper4InputSource *wis) throw(Exception);

    static string XMLCh2String(const XMLCh *);
    static XMLCh * String2XMLCh(const string &);

    static Exception XMLException2Exception(const XMLException &e);
    static Exception DOMException2Exception(const DOMException &e);

    static bool isVoidTextNode(const DOMNode &);
    static bool isCommentNode(const DOMNode &);
    static bool isNodeName(const DOMNode &, const string &);

    static string getStringAttribute(const DOMNamedNodeMap &, const string &attrname, const string &defvalue);
    static int getIntAttribute(const DOMNamedNodeMap &, const string &attrname, const int defvalue);
    static long getLongAttribute(const DOMNamedNodeMap &, const string &attrname, const long defvalue);
    static double getDoubleAttribute(const DOMNamedNodeMap &, const string &attrname, const double defvalue);
    static Date getDateAttribute(const DOMNamedNodeMap &, const string &attrname, const Date &defvalue);
    static bool getBooleanAttribute(const DOMNamedNodeMap &attrs, const string &attrname, const bool &defvalue);
};


//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
