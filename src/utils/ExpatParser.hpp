
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

#ifndef _ExpatParser_
#define _ExpatParser_

#include <expat.h>
#include <zlib.h>
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief   Parse an XML file using the Expat parser.
 *
 * @details This class parses an XML file using the Expat library providing
 *          the following features:
 *            - input file can be gziped
 *            - support defines
 *            - stack of handlers
 *            - file checksum (adler32)
 *            - stop parser by a variable.
 *
 * @see     http://expat.sourceforge.net/
 * @see     http://www.xml.com/pub/a/1999/09/expat/index.html
 */
class ExpatParser
{

  private:

    //! Expat parser object
    XML_Parser mXmlParser;
    //! Expat user data
    ExpatUserData mUserData;
    //! File checksum value
    unsigned long mChecksum;

  private:

    //! startElement handler
    static void startElement(void *eud, const char *tag, const char **atts);
    //! endElement handler
    static void endElement(void *eud, const char *tag);
    //! characterData Handler
    static void characterData(void *eud, const char *cdata, int len);

    //! Parse an xml file
    void parse(gzFile file, char *buf, size_t buffer_size, bool *stop);
    //! Reset internal variables
    void reset();

  public:

    //! Constructor
    ExpatParser();
    //! Destructor
    ~ExpatParser();
    //! Parse a string containing an xml
    void parse(const std::string &xmlcontent, ExpatHandlers *eh, bool *stop=nullptr);
    //! Parse an xml file
    void parse(gzFile file, ExpatHandlers *eh, bool *stop=nullptr);
    //! Returns defines
    const std::map<std::string,std::string>& getDefines() const;
    //! Set defines
    void setDefines(const std::map<std::string,std::string>&);
    //! returns checksum value
    unsigned long getChecksum() const;
    //! returns user data
    ExpatUserData& getUserData() { return mUserData; }

};

} // namespace

#endif

