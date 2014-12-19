
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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
#include <string>
#include <cassert>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

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
    enum class Type
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

    //! Default constructor
    LGD();
    //! Constructor
    LGD(const char *);
    //! Constructor
    LGD(const std::string &);
    //! Constructor
    LGD(Type, double a, double b=NAN);
    //! Constructor
    LGD(double);
    //! Comparison operator
    bool operator==(const LGD &o) const;
    //! Comparison operator
    bool operator!=(const LGD &o) const;
    //! Returns type
    Type getType() const;
    //! Returns distribution parameter
    double getValue1() const;
    //! Returns distribution parameter
    double getValue2() const;
    //! Returns lgd
    double getValue(const gsl_rng *rng=nullptr) const;

  public:

    //! Check if it is a Non-A-LGD value
    static bool isValid(const LGD &);

};

/**************************************************************************//**
 * @details Create a LGD with invalid values.
 */
inline ccruncher::LGD::LGD() : mType(Type::Fixed), mValue1(NAN), mValue2(NAN)
{
  // nothing to do
}

/**************************************************************************//**
 * @details Create a LGD of type fixed.
 */
inline ccruncher::LGD::LGD(double x) : mType(Type::Fixed), mValue1(x), mValue2(NAN)
{
  // nothing to do
}

/**************************************************************************//**
 * @param[in] rng Random number generator.
 * @return LGD value (if fixed) or simulated value (if distribution).
 */
inline double ccruncher::LGD::getValue(const gsl_rng *rng) const
{
  switch(mType)
  {
    case Type::Fixed:
      return mValue1;

    case Type::Beta:
      assert(rng != nullptr);
      return gsl_ran_beta(rng, mValue1, mValue2);

    case Type::Uniform:
      assert(rng != nullptr);
      return gsl_ran_flat(rng, mValue1, mValue2);

    default:
      assert(false);
      return NAN;
  }
}

}

#endif

