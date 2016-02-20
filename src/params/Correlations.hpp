
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2016 Gerard Torrent
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

#ifndef _Correlations_
#define _Correlations_

#include <gsl/gsl_matrix.h>
#include "params/Factors.hpp"
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Matrix of correlations between factors.
 *
 * @details This object represents the factor correlation matrix and
 *          provides the following functionalities:
 *            - Xml parsing (extending ExpatHandlers)
 *            - Cholesky matrix computation
 *
 * @see http://ccruncher.net/ifileref.html#correlations
 */
class Correlations : public std::vector<std::vector<double>>, public ExpatHandlers
{

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:

    //! Constructor
    Correlations(size_t dim=0);
    //! Inherits std::vector constructors
    using std::vector<std::vector<double>>::vector;

  public:

    //! Validate matrix content
    static bool isValid(const std::vector<std::vector<double>> &M, bool throwException=false);
    //! Return Cholesky matrix
    static gsl_matrix* getCholesky(const std::vector<std::vector<double>> &M);

};

} // namespace

#endif

