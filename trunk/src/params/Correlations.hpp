
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

#ifndef _Correlations_
#define _Correlations_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "params/Sectors.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Correlations : public ExpatHandlers
{

  private:

    // list of sectors
    Sectors sectors;
    // matrix values
    vector<vector<double> > matrix;

  private:

    // insert a new matrix value
    void insertCorrelation(const string &r1, const string &r2, double val) throw(Exception);
    // validate object content
    void validate() throw(Exception);

  protected:
  
    // ExpatHandler method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandler method
    void epend(ExpatUserData &, const char *);
  
  public:

    // constructor
    Correlations();
    // constructor
    Correlations(const Sectors &) throw(Exception);
    // initialize object
    void setSectors(const Sectors &) throw(Exception);
    // matrix size (= number of sector)
    int size() const;
    // returns a pointer to matrix values
    const vector<vector<double> >& getMatrix() const;
    // matrix element access
    const vector<double>& operator[] (int row) const;
    // serializes object content as xml
    string getXML(int) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
