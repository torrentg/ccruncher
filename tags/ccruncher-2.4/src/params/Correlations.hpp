
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
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Matrix of correlations between factors.
 *
 * @details This object represents the factor correlation matrix and
 *          provides the following functionalities:
 *            - Acces to matrix elements (read-only)
 *            - Xml parsing (extending ExpatHandlers)
 *            - Cholesky matrix computation
 *
 * @see http://ccruncher.net/ifileref.html#correlations
 */
class Correlations : public ExpatHandlers
{

  private:

    //! List of factors
    Factors factors;
    //! Correlation matrix
    std::vector<std::vector<double> > matrix;

  private:

    //! Insert a new matrix value
    void insertCorrelation(const std::string &, const std::string &, double) throw(Exception);
    //! Validate matrix values
    void validate() throw(Exception);

  protected:

    //! Directives to process an xml start tag element
    void epstart(ExpatUserData &, const char *, const char **);
    //! Directives to process an xml end tag element
    void epend(ExpatUserData &, const char *);

  public:

    //! Default constructor
    Correlations() {}
    //! Constructor
    Correlations(const Factors &) throw(Exception);
    //! Set factors
    void setFactors(const Factors &) throw(Exception);
    //! Return factors
    const Factors& getFactors() const;
    //! Matrix dimension
    int size() const;
    //! Matrix element access
    const std::vector<double>& operator[] (int row) const;
    //! Return Cholesky matrix
    gsl_matrix* getCholesky() const throw(Exception);

};

} // namespace

#endif

