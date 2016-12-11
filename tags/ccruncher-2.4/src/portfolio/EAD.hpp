
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

#ifndef _EAD_
#define _EAD_

#include <cmath>
#include <cassert>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief  Exposure at default.
 *
 * @details Exposure at default can be a fixed value or a random variable
 *          with positive values.
 *
 * @see http://ccruncher.net/ifileref.html#portfolio
 */
class EAD
{

  public:

    //! Exposure types
    enum Type
    { 
      Fixed=1,       //!< Fixed value ($)
      Lognormal=2,   //!< Lognormal distribution
      Exponential=3, //!< Exponential distribution
      Uniform=4,     //!< Uniform distribution
      Gamma=5,       //!< Gamma distribution
      Normal=6       //!< Truncated Gaussian distribution
    };

  private:

    //! Internal struct
    struct Distr
    {
      //! Distribution name
      const char *str;
      //! Name length
      int len;
      //! Number of arguments
      size_t nargs;
      //! Exposure type
      EAD::Type type;
      //TODO: add gsl_ran_* function ref?
    };

    //! List of supported distributions
    static const Distr distrs[];

  private:

    //! Exposure type
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
    EAD();
    //! Constructor
    EAD(const char *) throw(Exception);
    //! Constructor
    EAD(const std::string &) throw(Exception);
    //! Constructor
    EAD(Type, double a, double b=NAN) throw(Exception);
    //! Exposure type
    Type getType() const;
    //! Returns distribution parameter
    double getValue1() const;
    //! Returns distribution parameter
    double getValue2() const;
    //! Returns exposure
    double getValue(const gsl_rng *rng=NULL) const;
    //! Returns the mean of the exposure
    double getExpected() const;
    //! Apply current net value factor
    void mult(double);

    //! Check if it is a Non-A-EAD value
    static bool isvalid(const EAD &);

};

/**************************************************************************//**
 * @details Create a EAD with invalid values.
 */
inline ccruncher::EAD::EAD() : type(Fixed), value1(NAN), value2(NAN)
{
  // nothing to do
}

/**************************************************************************//**
 * @param[in] rng Random number generator.
 * @return Exposure value (if fixed) or simulated value (if distribution).
 */
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

} // namespace

#endif
