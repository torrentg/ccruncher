
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

#ifndef _TMFCopula_
#define _TMFCopula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "math/FastTStudentCdf.hpp"
#include "math/GMFCopula.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class TMFCopula : public GMFCopula
{

  private:

    // degrees of freedom
    double ndf;
    // t-student cdf
    FastTStudentCdf tcdf;

  public:

    // constructor
    TMFCopula(const vector<vector<double> > &M, const vector<unsigned int> &dims, double ndf) throw(Exception);
    // copy constructor
    TMFCopula(const TMFCopula &, bool alloc=true) throw(Exception);
    // destructor
    ~TMFCopula();
    // clone
    Copula* clone(bool alloc=true);
    // simulates a copula realization
    void next();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
