
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

#ifndef _Exposure_
#define _Exposure_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Exposure
{

  public:

    // exposure types
    enum ExposureType
    { 
      Fixed=1,
      Lognormal=2,
      Exponential=3,
      Uniform=4,
      Gamma=5,
      Normal=6
    };

  private:

    // exposure type
    ExposureType type;
    // depends on type
    double value1;
    // depends on type
    double value2;

  private:
  
    // set values
    void init(ExposureType, double, double) throw(Exception);
    // check params
    static void checkParams(ExposureType, double, double) throw(Exception);

  public:
  
    // default constructor
    Exposure();
    // constructor
    Exposure(const char *) throw(Exception);
    // constructor
    Exposure(const std::string &) throw(Exception);
    // constructor
    Exposure(ExposureType, double a, double b=NAN) throw(Exception);
    // returns type
    ExposureType getType() const;
    // retuns value1
    double getValue1() const;
    // returns value2
    double getValue2() const;
    // returns exposure
    double getValue(const gsl_rng *rng=NULL) const;
    // check if is a Non-A-Exposure value
    static bool valid(const Exposure &);
    // to string
    std::string toString() const;
    // apply current net value factor
    void mult(double);

};

//---------------------------------------------------------------------------

//===========================================================================
// default constructor
//===========================================================================
inline ccruncher::Exposure::Exposure() : type(Fixed), value1(NAN), value2(NAN)
{
}

//===========================================================================
// returns type
//===========================================================================
inline ccruncher::Exposure::ExposureType ccruncher::Exposure::getType() const
{
  return type;
}

//===========================================================================
// getValue
//===========================================================================
inline double ccruncher::Exposure::getValue(const gsl_rng *rng) const
{
  if (type == Fixed) return value1;
  else if (rng == NULL) return NAN;
  else 
  {
    switch(type)
    {
      case Lognormal:
        return gsl_ran_lognormal(rng, value1, value2);
      case Exponential:
        return gsl_ran_exponential(rng, value1);
      case Uniform:
        return gsl_ran_flat(rng, value1, value2);
      case Gamma:
        return gsl_ran_gamma(rng, value1, value2);
      case Normal:
        return std::max(0.0, value1 + gsl_ran_gaussian(rng, value2));
      default:
        return NAN;
    }
  }
}

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
