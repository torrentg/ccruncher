
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#ifndef _LGD_
#define _LGD_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "utils/Exception.hpp"
#include <cassert>

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class LGD
{

  public:

    // loss given default types
    enum Type
    { 
      Fixed=1,
      Uniform=2,
      Beta=3
    };

  private:

    // internal struct
    struct Distr
    {
      const char *str;
      int len;
      LGD::Type type;
      //TODO: add gsl_ran_* function ref?
    };

    // supported distributions
    static const Distr distrs[];

  private:

    // lgd type
    Type type;
    // depends on type
    double value1;
    // depends on type
    double value2;

  private:
  
    // set values
    void init(Type, double, double) throw(Exception);
    // check params
    static bool valid(Type, double, double);

  public:

    // default constructor
    LGD();
    // constructor
    LGD(const char *) throw(Exception);
    // constructor
    LGD(const std::string &) throw(Exception);
    // constructor
    LGD(Type, double a, double b=NAN) throw(Exception);
    // returns type
    Type getType() const;
    // retuns value1
    double getValue1() const;
    // returns value2
    double getValue2() const;
    // returns lgd
    double getValue(const gsl_rng *rng=NULL) const;
    // check if is a Non-A-LGD value
    static bool isvalid(const LGD &);

};

//---------------------------------------------------------------------------

//===========================================================================
// default constructor
//===========================================================================
inline ccruncher::LGD::LGD() : type(Fixed), value1(NAN), value2(NAN)
{
}

//===========================================================================
// returns type
//===========================================================================
inline ccruncher::LGD::Type ccruncher::LGD::getType() const
{
  return type;
}

//===========================================================================
// getValue
//===========================================================================
inline double ccruncher::LGD::getValue(const gsl_rng *rng) const
{
  switch(type)
  {
    case Fixed:
      return value1;

    case Beta:
      assert(rng != NULL);
      return gsl_ran_beta(rng, value1, value2);

    case Uniform:
      assert(rng != NULL);
      return gsl_ran_flat(rng, value1, value2);

    default:
      assert(false);
      return NAN;
  }
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
