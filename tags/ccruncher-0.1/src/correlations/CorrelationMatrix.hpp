
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
// CorrelationMatrix.hpp - CorrelationMatrix header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _CorrelationMatrix_
#define _CorrelationMatrix_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "xercesc/dom/DOM.hpp"
#include "utils/Exception.hpp"
#include "sectors/Sectors.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace xercesc;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class CorrelationMatrix
{

  private:

    int n;
    double epsilon;
    Sectors *sectors;
    double **matrix;

    void init(Sectors *) throw(Exception);
    void parseDOMNode(const DOMNode&) throw(Exception);
    void parseSigma(const DOMNode&) throw(Exception);
    void insertSigma(const string &r1, const string &r2, double val) throw(Exception);
    void validate(void) throw(Exception);

  public:

    CorrelationMatrix(Sectors *, const DOMNode &) throw(Exception);
    ~CorrelationMatrix();

    int size();
    double ** getMatrix();
    string getXML(int) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
