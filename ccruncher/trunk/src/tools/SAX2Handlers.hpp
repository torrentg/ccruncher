
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
// SAX2Handlers.hpp - SAX2Handlers header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _SAX2Handlers_
#define _SAX2Handlers_

//---------------------------------------------------------------------------

#include <vector>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;

//---------------------------------------------------------------------------

#define TAG_NONE                 0
#define TAG_SEGMENTAGGREGATOR    1
#define TAG_ITEM                 2
#define TAG_TRANCH               3
#define TAG_CASHFLOW             4
#define TAG_EXPOSURE             5
#define TAG_RATING               6

#define NUMTOKENS               10
#define TOKEN_SEGMENTAGGREGATOR  0
#define TOKEN_VALUES             1
#define TOKEN_NTRANCHES          2
#define TOKEN_ITEM               3
#define TOKEN_TRANCH             4
#define TOKEN_VALUE              5
#define TOKEN_CASHFLOW           6
#define TOKEN_EXPOSURE           7
#define TOKEN_TYPE               8
#define TOKEN_NAME               9

//---------------------------------------------------------------------------

class SAX2Handlers : public DefaultHandler
{

  private:

    int m;
    unsigned int ipos;
    int jpos;
    vector<double> *tranch;
    int currenttag;
    bool fSawErrors;
    string getAttribute(string aname, const Attributes& attrs);
    XMLCh *tokens[NUMTOKENS];

  public:

    SAX2Handlers();
    ~SAX2Handlers();

    int getNumTranches() const;
    vector<double> *getTranches();
    bool getSawErrors() const;

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ContentHandler interface
    // -----------------------------------------------------------------------
    void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs);
    void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
    void characters(const XMLCh* const chars, const unsigned int length);
    void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
    void resetDocument();

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& exc);
    void error(const SAXParseException& exc);
    void fatalError(const SAXParseException& exc);
    void resetErrors();

};

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
