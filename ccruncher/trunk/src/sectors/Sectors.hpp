
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
// Sectors.hpp - Sectors header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2005/12/17 - Gerard Torrent [gerard@fobos.generacio.com]
//   . Sectors refactoring
//
// 2007/07/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added getIndex() method
//
//===========================================================================

#ifndef _Sectors_
#define _Sectors_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "sectors/Sector.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Sectors : public ExpatHandlers
{

  private:

    // list of sectors
    vector<Sector> vsectors;
    // auxiliary variable (used by parser)
    Sector auxsector;

    // add a sector to list
    void insertSector(const Sector &) throw(Exception);
    // validate list
    void validations() throw(Exception);


  public:

    // default constructor
    Sectors();
    // destructor
    ~Sectors();

    // return the number of sectors
    int size() const;
    // return the index of the sector
    int getIndex(const string &name);
    // [] operator
    Sector& operator [] (int i);
    // [] operator
    Sector& operator [] (const string &name) throw(Exception);
    // returns object content as xml
    string getXML(int) const throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
