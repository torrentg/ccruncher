
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

#ifndef _BlockTStudentCopula_
#define _BlockTStudentCopula_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "math/Random.hpp"
#include "math/Copula.hpp"
#include "math/BlockMatrixChol.hpp"
#include "utils/LookupTable.hpp"
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
    double ndf;
    BlockMatrixChol chol;
    double *aux1;
    double *aux2;

    Random random;
    LookupTable lut;

    void reset();
    void finalize();
    double transform(double val);
    void randNm();
    void initLUT() throw(Exception);


  public:

    int size();
    void next();
    double get(int);
    void setSeed(long);
    double getConditionNumber();

    BlockTStudentCopula(double **C, int *n, int m, double ndf) throw(Exception);
    BlockTStudentCopula(const BlockTStudentCopula &) throw(Exception);
    ~BlockTStudentCopula();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------