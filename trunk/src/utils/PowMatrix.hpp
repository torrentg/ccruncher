
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2015 Gerard Torrent
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

#ifndef _PowMatrix_
#define _PowMatrix_

#include <vector>
#include "utils/Exception.hpp"

namespace ccruncher {


/**************************************************************************//**
 * @brief   Functions to raise a matrix to power.
 *
 * @details Matrix must satisfy the following conditions:
 *          - square
 *          - invertible
 *          - real coeficients
 *          - real eigenvalues
 */
class PowMatrix
{

  public:

    //! Improved std::pow() function
    static double fpow(double base, double exponent);
    //! Matrix pow function
    static void pow(double **a, double x, size_t n, double **ret);
    //! Matrix pow function
    static void pow(const std::vector<std::vector<double>> &a, double x, std::vector<std::vector<double>> &ret);

};

} // namespace

#endif

