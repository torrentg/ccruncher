
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
#include <stack>
#include <vector>
#include <string>
#include <expat.h>
#include <cassert>
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

// forward declarations
class ExpatHandlers;
class Date;
class Interest;
class Ratings;
class Factors;
class Segmentations;

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
      //! Constructor
      ExpatUserDataToken(const char *n=nullptr, ExpatHandlers *h=nullptr) : handlers(h) {
        assert(h != nullptr && n!=nullptr && strlen(n)<20);
        strncpy(name, n, 20);
      }
    };

  private:

    //! Stack of handlers
    std::stack<ExpatUserDataToken, std::vector<ExpatUserDataToken> > mStack;
    //! Current element name
    const char *mCurrentTag;
    //! Buffer used to apply defines
    char *mBuffer;
    //! Buffer size
    size_t mBufferSize;
    //! Points to first char in current buffer
    char *mBufferPos1;
    //! Points to last char in current buffer
    char *mBufferPos2;

  private:

    //! Push str to buffer
    const char* bufferPush(const char *str, size_t n);
    //! Append str to buffer
    const char* bufferAppend(const char *str, size_t n);

  public:

    //! List of defines
    std::map<std::string,std::string> defines;
    //! Initial date
    ccruncher::Date *date1;
    //! Ending date
    ccruncher::Date *date2;
    //! Yield curve
    ccruncher::Interest *interest;
    //! List of ratings
    ccruncher::Ratings *ratings;
    //! List of factors
    ccruncher::Factors *factors;
    //! List of segmentations
    ccruncher::Segmentations *segmentations;

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
    ExpatHandlers* getCurrentHandlers() const { return mStack.top().handlers; }
    //! Returns current handlers element name
    const char* getCurrentName() const { return mStack.top().name; }
    //! Remove current handlers
    void removeCurrentHandlers() { mStack.pop(); }
    //! Set current handlers
    void setCurrentHandlers(const char *name, ExpatHandlers *eh);
    //! Set current tag name
    void setCurrentTag(const char *t) { mCurrentTag = t; }
    //! Returns current tag name
    const char *getCurrentTag() const { return mCurrentTag; }
    //! Apply defines to the given string
    const char* applyDefines(const char *str);

};

} // namespace

#endif

