
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

#ifndef _Random_
#define _Random_

//---------------------------------------------------------------------------

#include <gsl/gsl_rng.h>
#include "utils/config.h"

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class Random
{

  private:

    // RNG object
    gsl_rng *rng;

  private:
  
    // RNG initialization
    void init();

  public:

    // default constructor
    Random();
    // constructor
    Random(long seed);
    // destructor
    ~Random();
    // set RNG seed
    void setSeed(long);
    // returns N(0,1)
    double nextGaussian();
    // returns N(mu,sigma)
    double nextGaussian(double mu, double sigma);
    // returns T(nu)
    double nextChisq(double nu);
    // returns Unif(a,b)
    double nextUnif(double a, double b);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------

