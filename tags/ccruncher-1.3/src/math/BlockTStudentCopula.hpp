
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2008 Gerard Torrent
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
// BlockTStudentCopula.hpp - BlockTStudentCopula header - $Rev: 435 $
// --------------------------------------------------------------------------
//
// 2008/12/05 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _BlockTStudentCopula_
#define _BlockTStudentCopula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "math/Copula.hpp"
#include "math/BlockMatrixChol.hpp"
#include <MersenneTwister.h>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class BlockTStudentCopula : public Copula
{
  private:

    int n;
    int m;
    int ndf;
    BlockMatrixChol *chol;
    double **correls;
    double *aux1;
    double *aux2;
    double chisim;
    bool owner;

    MTRand mtrand;

    void init();
    void finalize();
    double transform(double val);
    void randNm();
    void randCs();


  public:

    int size();
    void next();
    double get(int);
    void setSeed(long);

    BlockTStudentCopula(double **C, int *n, int m, int ndf) throw(Exception);
    BlockTStudentCopula(const BlockTStudentCopula &) throw(Exception);
    ~BlockTStudentCopula();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
