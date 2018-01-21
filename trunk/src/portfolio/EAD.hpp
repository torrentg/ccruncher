
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2018 Gerard Torrent
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

#pragma once

#include <cmath>
#include <string>
#include <cassert>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

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
    enum class Type
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
    Type mType;
    //! Distribution parameter
    double mValue1;
    //! Distribution parameter
    double mValue2;

  private:
  
    //! Initialize content
    void init(Type, double, double);
    //! Check distribution parameters
    static bool valid(Type, double, double);

  public:
  
    //! Constructor (fixed ead)
    EAD(double x=0.0);
    //! Constructor
    EAD(Type, double a, double b=NAN);
    //! Constructor
    EAD(const char *);
    //! Constructor
    EAD(const std::string &str) : EAD(str.c_str()) {}
    //! Comparison operator
    bool operator==(const EAD &o) const;
    //! Comparison operator
    bool operator!=(const EAD &o) const;
    //! Exposure type
    Type getType() const { return mType; }
    //! Returns distribution parameter
    double getValue1() const { return mValue1; }
    //! Returns distribution parameter
    double getValue2() const { return mValue2; }
    //! Returns exposure
    double getValue(const gsl_rng *rng=nullptr) const;
    //! Returns the mean of the exposure
    double getExpected() const;
    //! Apply current net value factor
    void mult(double);

};

/**************************************************************************//**
 * @param[in] rng Random number generator.
 * @return Exposure value (if fixed) or simulated value (if distribution).
 */
inline double ccruncher::EAD::getValue(const gsl_rng *rng) const
{
  switch(mType)
  {
    case Type::Fixed:
      return mValue1;

    case Type::Lognormal:
      assert(rng != nullptr);
      return gsl_ran_lognormal(rng, mValue1, mValue2);

    case Type::Exponential:
      assert(rng != nullptr);
      return gsl_ran_exponential(rng, mValue1);

    case Type::Uniform:
      assert(rng != nullptr);
      return gsl_ran_flat(rng, mValue1, mValue2);

    case Type::Gamma:
      assert(rng != nullptr);
      return gsl_ran_gamma(rng, mValue1, mValue2);

    case Type::Normal:
      assert(rng != nullptr);
      return std::max(0.0, mValue1 + gsl_ran_gaussian(rng, mValue2));

    default:
      assert(false);
      return NAN;
  }
}

} // namespace
