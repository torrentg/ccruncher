
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

#ifndef _Correlations_
#define _Correlations_

#include <string>
#include <gsl/gsl_matrix.h>
#include "params/Factors.hpp"
#include "utils/ExpatHandlers.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Matrix of correlations between factors.
 *
 * @details This object represents the factor correlation matrix and
 *          provides the following functionalities:
 *            - Acces to matrix elements
 *            - Xml parsing (extending ExpatHandlers)
 *            - Cholesky matrix computation
 *
 * @see http://ccruncher.net/ifileref.html#correlations
 */
class Correlations : public ExpatHandlers
{

  private:

    //! Correlation matrix
    std::vector<std::vector<double>> mMatrix;

  protected:

    //! Directives to process an xml start tag element
    virtual void epstart(ExpatUserData &, const char *, const char **) override;
    //! Directives to process an xml end tag element
    virtual void epend(ExpatUserData &, const char *) override;

  public:

    //! Constructor
    Correlations(size_t n=0);
    //! Constructor
    Correlations(const std::vector<std::vector<double>> &);
    //! Matrix dimension
    size_t size() const;
    //! Matrix element access
    const std::vector<double>& operator[] (size_t row) const;
    //! Matrix element access
    std::vector<double>& operator[] (size_t row);
    //! Validate matrix content
    bool isValid();
    //! Return Cholesky matrix
    gsl_matrix* getCholesky() const;

};

} // namespace

#endif

