
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
// Sectors.hpp - Sectors header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
//===========================================================================

#ifndef _Sectors_
#define _Sectors_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "xercesc/dom/DOM.hpp"
#include "Sector.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Sectors : public ExpatHandlers
{

  private:

    vector<Sector> vsectors;
    Sector auxsector;

    void parseDOMNode(const DOMNode&) throw(Exception);
    void insertSector(Sector &) throw(Exception);
    void validations() throw(Exception);


  public:

    Sectors();
    Sectors(const DOMNode &) throw(Exception);
    ~Sectors();

    vector<Sector> * getSectors();
    int getIndex(const string &);
    string getName(int index) throw(Exception);
    string getXML(int) throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
