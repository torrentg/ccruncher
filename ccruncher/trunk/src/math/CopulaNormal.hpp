
//***************************************************************************
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
// CopulaNormal.hpp - CopulaNormal header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//***************************************************************************

#ifndef _CopulaNormal_
#define _CopulaNormal_

//---------------------------------------------------------------------------

#include "Copula.hpp"
#include "MersenneTwister.h"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class CopulaNormal : public Copula
{
  private:
  
    int n;
    double **sigmas;
    double *aux1;
    double *aux2;
    bool owner;

    MTRand mtrand;

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

    CopulaNormal(int, double **) throw(Exception);
    CopulaNormal(const CopulaNormal &) throw(Exception);
    ~CopulaNormal();
    
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
