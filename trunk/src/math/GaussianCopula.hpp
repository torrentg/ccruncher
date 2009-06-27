
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
// GaussianCopula.hpp - GaussianCopula header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/07/24 - Gerard Torrent [gerard@mail.generacio.com]
//   . class CopulaNormal renamed to GaussianCopula
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2009/06/24 - Gerard Torrent [gerard@mail.generacio.com]
//   . replaced random number generator
//
//===========================================================================

#ifndef _GaussianCopula_
#define _GaussianCopula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "math/Random.hpp"
#include "math/Copula.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class GaussianCopula : public Copula
{
  private:

    int n;
    double **sigmas;
    double *aux1;
    double *aux2;
    bool owner;

    Random random;

    void init();
    void finalize();
    void correls2sigmas(double **mcorrels) throw(Exception);
    double transform(double val);
    void randNm();

  public:

    int size();
    void next();
    double get(int);
    void setSeed(long);

    GaussianCopula(int, double **) throw(Exception);
    GaussianCopula(const GaussianCopula &) throw(Exception);
    ~GaussianCopula();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
