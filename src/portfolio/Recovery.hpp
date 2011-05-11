
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2011 Gerard Torrent
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

#ifndef _Recovery_
#define _Recovery_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Recovery
{

  public:

    // recovery types
    enum RecoveryType
    { 
      Fixed=1,
      Uniform=2,
      Beta=3
    };

  private:

    // recovery type
    RecoveryType type;
    // depends on type
    double value1;
    // depends on type
    double value2;

  private:
  
    // set values
    void init(RecoveryType, double, double) throw(Exception);
    // check params
    static void checkParams(RecoveryType, double, double) throw(Exception);

  public:
  
    // default constructor
    Recovery();
    // constructor
    Recovery(const char *) throw(Exception);
    // constructor
    Recovery(const string &) throw(Exception);
    // constructor
    Recovery(RecoveryType, double a, double b=NAN) throw(Exception);
    // returns type
    RecoveryType getType() const;
    // retuns value1
    double getValue1() const;
    // returns value2
    double getValue2() const;
    // returns recovery (includes Beta)
    double getValue(const gsl_rng *rng=NULL) const;
    // check if is a Non-A-Recovery value
    static bool valid(const Recovery &);
    // to string
	string toString() const;
	
};

//---------------------------------------------------------------------------

//===========================================================================
// default constructor
//===========================================================================
inline ccruncher::Recovery::Recovery()
{
  type = Fixed;
  value1 = NAN;
  value2 = NAN;
}

//===========================================================================
// returns type
//===========================================================================
inline ccruncher::Recovery::RecoveryType ccruncher::Recovery::getType() const
{
  return type;
}

//===========================================================================
// getValue
//===========================================================================
inline double ccruncher::Recovery::getValue(const gsl_rng *rng) const
{
  if (type == Fixed) return value1;
  else if (rng == NULL) return NAN;
  else 
  {
    switch(type)
    {
      case Beta:
        return gsl_ran_beta(rng, value1, value2);
      case Uniform:
        return gsl_ran_flat(rng, value1, value2);
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
