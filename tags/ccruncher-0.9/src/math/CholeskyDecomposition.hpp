
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
// CholeskyDecomposition.hpp - CholeskyDecomposition header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release (see 'Numerical Recipes in C')
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added private constructor (non-instantiable class)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#ifndef _CholeskyDecomposition_
#define _CholeskyDecomposition_

//---------------------------------------------------------------------------

#include "utils/config.h"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class CholeskyDecomposition
{

  private:

    // non-instantiable class
    CholeskyDecomposition() {};


  public:

    static bool choldc(double **a, double *p, int n);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
