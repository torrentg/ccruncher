
//***************************************************************************
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
// Interests.hpp - Interests header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#ifndef _Interests_
#define _Interests_

//---------------------------------------------------------------------------

#include <algorithm>
#include "xercesc/dom/DOM.hpp"
#include "utils/Exception.hpp"
#include "Interest.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Interests
{

  private:

    vector<Interest> vinterests;
    int ispot;

    void parseDOMNode(const DOMNode&) throw(Exception);
    void insertInterest(Interest &) throw(Exception);
    void validate() throw(Exception);


  public:

    Interests(const DOMNode &) throw(Exception);
    ~Interests();

    vector<Interest> * getInterests();
    Interest * getInterest(string name) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
