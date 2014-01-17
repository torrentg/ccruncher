
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

#include <cmath>
#include <cassert>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief  Loss given default.
 *
 * @details Loss given default is a percentage and can be a fixed value or
 *          a random variable with values in range [0,1].
 *
 * @see http://ccruncher.net/ifileref.html#portfolio
 */
class LGD
{

  public:

    //! Loss given default types
    enum Type
    { 
      Fixed=1,   //!< Fixed value in [0,1]
      Uniform=2, //!< Uniform distribution
      Beta=3     //!< Beta distribution
    };

  private:

    //! Internal struct
    struct Distr
    {
      //! Distribution name
      const char *str;
      //! Name length
      int len;
      //! Exposure type
      LGD::Type type;
      //TODO: add gsl_ran_* function ref?
    };

    //! List of supported distributions
    static const Distr distrs[];

  private:

    //! Lgd type
    Type type;
    //! Distribution parameter
    double value1;
    //! Distribution parameter
    double value2;

  private:
  
    //! Initialize content
    void init(Type, double, double) throw(Exception);
    //! Check distribution parameters
    static bool valid(Type, double, double);

  public:

    //! Default constructor
    LGD();
    //! Constructor
    LGD(const char *) throw(Exception);
    //! Constructor
    LGD(const std::string &) throw(Exception);
    //! Constructor
    LGD(Type, double a, double b=NAN) throw(Exception);
    //! Returns type
    Type getType() const;
    //! Returns distribution parameter
    double getValue1() const;
    //! Returns distribution parameter
    double getValue2() const;
    //! Returns lgd
    double getValue(const gsl_rng *rng=NULL) const;
    //! Check if it is a Non-A-LGD value
    static bool isvalid(const LGD &);

};

/**************************************************************************//**
 * @details Create a LGD with invalid values.
 */
inline ccruncher::LGD::LGD() : type(Fixed), value1(NAN), value2(NAN)
{
  // nothing to do
}

/**************************************************************************//**
 * @param[in] rng Random number generator.
 * @return LGD value (if fixed) or simulated value (if distribution).
 */
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

}

#endif

