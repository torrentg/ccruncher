
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#ifndef _Sectors_
#define _Sectors_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Sectors : public ExpatHandlers
{

  private:

    // internal struct
    struct Sector
    {
        // sector name
        string name;
        // sector description
        string desc;
        // constructor
        Sector(const string &n="", const string &d="") : name(n), desc(d) {}
    };

  private:

    // list of sectors
    vector<Sector> vsectors;

  private:
  
    // add a sector to list
    void insertSector(const Sector &) throw(Exception);
    // validate list
    void validations() throw(Exception);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);
  
  public:

    // default constructor
    Sectors();
    // destructor
    ~Sectors();
    // return the number of sectors
    int size() const;
    // return the index of the sector
    int getIndex(const char *name) const;
    int getIndex(const string &name) const;
    // return sector name
    const string& getName(int i) const;
    // return sector description
    const string& getDescription(int i) const;
    // returns object content as xml
    string getXML(int) const throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
