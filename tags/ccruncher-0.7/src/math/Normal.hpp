
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
// Normal.hpp - Normal header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/08/08 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added private constructor (non-instantiable class)
//
//===========================================================================

#ifndef _Normal_
#define _Normal_

//---------------------------------------------------------------------------

#include "utils/config.h"

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class Normal
{

  private:

    // non-instantiable class
    Normal() {};


  public:

    static double pdf(double x);
    static double pdf(double x, double mu, double sigma);
    static double cdf(double x);
    static double cdf(double x, double mu, double sigma);
    static double cdfinv(double p);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
