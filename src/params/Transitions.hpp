
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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

#include <string>
#include <vector>
#include "utils/Date.hpp"
#include "params/CDF.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Matrix of transitions between ratings.
 *
 * @details This class provides methods to define and scale this
 *          matrix to another time period.
 *
 * @see http://ccruncher.net/ifileref.html#transitions
 */
class Transitions
{

  private:

    //! Period (in months) that this transition matrix covers
    int mPeriod;
    //! Matrix values
    std::vector<std::vector<double>> mMatrix;
    //! Index of default rating
    mutable int mIndexDefault;
    //! Need to recompute spline flag
    mutable bool isDirty;

  private:

    //! Matrix product (M3 = M1·M2)
    static void prod(const std::vector<std::vector<double>> &M1,
                     const std::vector<std::vector<double>> &M2,
                     std::vector<std::vector<double>> &M3);

    //! Computes Cumulated Default Forward Rate
    void cdfr(size_t numrows, std::vector<std::vector<double>> &ret) const;
    //! Regularize the transition matrix
    double regularize();

  public:

    //! Default constructor
    Transitions(unsigned char numRatings=1, int period=12);
    //! Constructor
    Transitions(const std::vector<std::vector<double>> &matrix, int period);

    //! Matrix dimension (=number of ratings)
    unsigned char size() const;
    //! Returns period (in months) that covers this matrix
    int getPeriod() const;
    //! Set transition matrix values
    void setValues(const std::vector<std::vector<double>> &matrix);
    //! Insert a transition value into the matrix
    void setValue(unsigned char row, unsigned char col, double value);
    //! Returns default rating index
    unsigned char getIndexDefault() const;
    //! Returns equivalent transition matrix that covers t months
    Transitions scale(int t) const;
    //! Computes default probabilities functions related to this transition matrix
    std::vector<CDF> getCDFs(const Date &date, int numrows) const;
    //! Matrix element access
    const std::vector<double>& operator[] (unsigned char row) const;
    //! Validate transition matrix
    void validate() const;

};

} // namespace
