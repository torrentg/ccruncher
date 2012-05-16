
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

#ifndef _Defaults_
#define _Defaults_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include <map>
#include "params/Sectors.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

struct hdata
{
  int nobligors;
  int ndefaulted;
  hdata() : nobligors(-1), ndefaulted(-1) {}
  hdata(int o_, int d_) : nobligors(o_), ndefaulted(d_) {}
};

class Defaults : public ExpatHandlers
{

  private:

    // period
    int period;
    // list of sectors
    Sectors sectors;
    // auxiliar map
    map<string, int> indices;
    // observed values
    vector<vector<hdata> > data;

  private:

    // insert a new matrix value
    void insertValue(const string &t, const string &sector, int nobligors, int ndefaulted) throw(Exception);
    // validate object content
    void validate() throw(Exception);

  protected:

    // ExpatHandler method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandler method
    void epend(ExpatUserData &, const char *);

  public:

    // constructor
    Defaults();
    // constructor
    Defaults(Sectors &, int) throw(Exception);
    // destructor
    ~Defaults();
    // initialize object
    void setSectors(const Sectors &) throw(Exception);
    // initialize object
    void setPeriod(int) throw(Exception);
    // returns data
    const vector<vector<hdata> >& getData() const;
    // serializes object content as xml
    string getXML(int) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
