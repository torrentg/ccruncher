
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2017 Gerard Torrent
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

#ifndef _Params_
#define _Params_

#include <string>
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Parameters of the simulation.
 *
 * @see http://ccruncher.net/ifileref.html#parameters
 */
class Params
{

  private:

    //! Simulation starting time
    Date time0;
    //! Simulation ending time
    Date timeT;
    //! Number of Monte Carlo iterations (0 = no limit)
    size_t maxIterations = 1000000;
    //! Maximum Monte Carlo execution time in seconds (0 = no limit)
    size_t maxSeconds = 0;
    //! Simulation copula type
    std::string copula = "gaussian";
    //! RNG seed
    unsigned long rngSeed = 0UL;
    //! Monte Carlo antithetic usage
    bool antithetic = true;
    //! Simulation block size
    unsigned short blockSize = 128;

  public:

    //! Returns starting time
    Date getTime0() const { return time0; }
    //! Set starting time
    void setTime0(Date date) { time0 = date; }
    //! Returns ending time
    Date getTimeT() const { return timeT; }
    //! Set ending time
    void setTimeT(Date date) { timeT = date; }
    //! Returns the number of MonteCarlo iterations
    size_t getMaxIterations() const { return maxIterations; }
    //! set the number of MonteCarlo iterations
    void setMaxIterations(size_t num) { maxIterations = num; }
    //! Returns the maximum execution time
    size_t getMaxSeconds() const { return maxSeconds; }
    //! Set the maximum execution time
    void setMaxSeconds(size_t num) { maxSeconds = num; }
    //! Returns copula type
    std::string getCopula() const { return copula; }
    //! Set copula type
    void setCopula(const std::string &str);
    //! Returns the t-student copula degrees of freedom
    double getNdf() const;
    //! Returns RNG seed
    unsigned long getRngSeed() const { return rngSeed; }
    //! Set RNG seed
    void setRngSeed(unsigned long num) { rngSeed = num; }
    //! Returns antithetic flag
    bool getAntithetic() const { return antithetic; }
    //! Set antithetic flag
    void setAntithetic(bool val) { antithetic = val; }
    //! Returns simulation block size
    unsigned short getBlockSize() const { return blockSize; }
    //! Set simulation block size
    void setBlockSize(unsigned short num) { blockSize = num; }

    //! Set a parameter
    void setParamValue(const std::string &name, const std::string &value);
    //! Validate object content
    bool isValid(bool throwException=false) const;

};

} // namespace

#endif

