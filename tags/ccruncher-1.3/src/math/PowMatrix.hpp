
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
// PowMatrix.hpp - PowMatrix header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/08/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . added private constructor (non-instantiable class)
//   . changed scope of pow(x,y) [ccruncher::PowMatrix:: -> ccruncher::]
//
// 2005/08/09 - Gerard Torrent [gerard@mail.generacio.com]
//   . pow method renamed: pow(x,y) -> fpow(x,y)
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#ifndef _PowMatrix_
#define _PowMatrix_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <tnt_array2d.h>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace TNT;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class PowMatrix
{

  private:

    // non-instantiable class
    PowMatrix() {};

    // internal function
    static Array2D<double> inverse(Array2D<double> &x) throw(Exception);


  public:

    // matrix pow function
    static void pow(double **a, double x, int n, double **ret) throw(Exception);

};

//---------------------------------------------------------------------------

// improved pow() function [belongs to ccruncher namespace]
double fpow(double, double) throw(Exception);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
