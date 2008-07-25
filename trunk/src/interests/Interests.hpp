
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
// Interests.hpp - Interests header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/03/19 - Gerard Torrent [gerard@mail.generacio.com]
//   . added dummy constructor
//
// 2005/04/02 - Gerard Torrent [gerard@mail.generacio.com]
//   . migrated from xerces to expat
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@mail.generacio.com]
//   . class refactoring
//
//===========================================================================

#ifndef _Interests_
#define _Interests_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "interests/Interest.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Interests : public ExpatHandlers
{

  private:

    // list of interest curves
    vector<Interest> vinterests;
    // index for spot curve
    int ispot;
    // uaxiliary variable (used by parser)
    Interest auxinterest;

    // insert an interest curve to list
    void insertInterest(const Interest &) throw(Exception);
    // validate list
    void validate() throw(Exception);


  public:

    // default constructor
    Interests();
    // destructor
    ~Interests();

    // returns the number of interests
    int size() const;
    // [] operator
    Interest& operator [] (int i);
    // [] operator
    Interest& operator [] (const string &name) throw(Exception);
    // return xml string with object content
    string getXML(int ilevel) const throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
