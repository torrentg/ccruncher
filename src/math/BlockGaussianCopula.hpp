
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

#ifndef _BlockGaussianCopula_
#define _BlockGaussianCopula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "math/Random.hpp"
#include "math/Copula.hpp"
#include "math/BlockMatrixChol.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class BlockGaussianCopula : public Copula
{
  private:

    int n;
    int m;
    BlockMatrixChol *chol;
    double **correls;
    double *aux1;
    double *aux2;
    bool owner;

    Random random;

    void init();
    void finalize();
    double transform(double val);
    void randNm();

  public:

    int size();
    void next();
    double get(int);
    void setSeed(long);
    double getConditionNumber();

    BlockGaussianCopula(double **C, int *n, int m) throw(Exception);
    BlockGaussianCopula(const BlockGaussianCopula &) throw(Exception);
    ~BlockGaussianCopula();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------