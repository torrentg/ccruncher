
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

#ifndef _Factors_
#define _Factors_

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

class Factors : public ExpatHandlers
{

  private:

    // internal struct
    struct Factor
    {
        // factor name
        string name;
        // factor description
        string desc;
        // constructor
        Factor(const string &n="", const string &d="") : name(n), desc(d) {}
    };

  private:

    // list of factors
    vector<Factor> vfactors;

  private:
  
    // add a factor to list
    void insertFactor(const Factor &) throw(Exception);
    // validate list
    void validations() throw(Exception);

  protected:
  
    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);
  
  public:

    // default constructor
    Factors();
    // return the number of factors
    int size() const;
    // return the index of the factor
    int getIndex(const char *name) const;
    int getIndex(const string &name) const;
    // return factor name
    const string& getName(int i) const;
    // return factor description
    const string& getDescription(int i) const;
    // returns object content as xml
    string getXML(int) const throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
