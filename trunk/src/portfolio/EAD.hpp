
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#ifndef _EAD_
#define _EAD_

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

class EAD
{

  public:

    // exposure types
    enum Type
    { 
      Fixed=1,
      Lognormal=2,
      Exponential=3,
      Uniform=4,
      Gamma=5,
      Normal=6
    };

  private:

    // internal struct
    struct Distr
    {
      const char *str;
      int len;
      size_t nargs;
      EAD::Type type;
      //TODO: add gsl_ran_* function ref?
    };

    // supported distributions
    static const Distr distrs[];

  private:

    // exposure type
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
    EAD();
    // constructor
    EAD(const char *) throw(Exception);
    // constructor
    EAD(const std::string &) throw(Exception);
    // constructor
    EAD(Type, double a, double b=NAN) throw(Exception);
    // returns type
    Type getType() const;
    // retuns value1
    double getValue1() const;
    // returns value2
    double getValue2() const;
    // returns exposure
    double getValue(const gsl_rng *rng=NULL) const;
    // check if is a Non-A-EAD value
    static bool isvalid(const EAD &);
    // apply current net value factor
    void mult(double);

};

//---------------------------------------------------------------------------

//===========================================================================
// default constructor
//===========================================================================
inline ccruncher::EAD::EAD() : type(Fixed), value1(NAN), value2(NAN)
{
}

//===========================================================================
// returns type
//===========================================================================
inline ccruncher::EAD::Type ccruncher::EAD::getType() const
{
  return type;
}

//===========================================================================
// getValue
//===========================================================================
inline double ccruncher::EAD::getValue(const gsl_rng *rng) const
{
  switch(type)
  {
    case Fixed:
      return value1;

    case Lognormal:
      assert(rng != NULL);
      return gsl_ran_lognormal(rng, value1, value2);

    case Exponential:
      assert(rng != NULL);
      return gsl_ran_exponential(rng, value1);

    case Uniform:
      assert(rng != NULL);
      return gsl_ran_flat(rng, value1, value2);

    case Gamma:
      assert(rng != NULL);
      return gsl_ran_gamma(rng, value1, value2);

    case Normal:
      assert(rng != NULL);
      return std::max(0.0, value1 + gsl_ran_gaussian(rng, value2));

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
