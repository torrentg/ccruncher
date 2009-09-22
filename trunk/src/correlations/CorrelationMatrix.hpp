
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

#ifndef _CorrelationMatrix_
#define _CorrelationMatrix_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "sectors/Sectors.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class CorrelationMatrix : public ExpatHandlers
{

  private:

    // nxn = matrix size (n = number of sectors)
    int n;
    // epsilon used to compare doubles
    double epsilon;
    // list of sectors
    Sectors sectors;
    // matrix of values
    double **matrix;

    // insert a new matrix value
    void insertSigma(const string &r1, const string &r2, double val) throw(Exception);
    // validate object content
    void validate(void) throw(Exception);


  public:

    // constructor
    CorrelationMatrix();
    // constructor
    CorrelationMatrix(Sectors &) throw(Exception);
    // copy constructor
    CorrelationMatrix(CorrelationMatrix &) throw(Exception);
    // destructor
    ~CorrelationMatrix();
    // initialize object
    void setSectors(Sectors &) throw(Exception);
    // matrix size (= number of sector)
    int size() const;
    // returns a pointer to matrix values
    double ** getMatrix() const;
    // serializes object content as xml
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
