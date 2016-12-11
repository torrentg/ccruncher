
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

#ifndef _Transitions_
#define _Transitions_

#include <string>
#include <vector>
#include "params/Ratings.hpp"
#include "params/DefaultProbabilities.hpp"
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief Matrix of transitions between ratings.
 *
 * @details This class provides methods to read the transition matrix
 *          from the xml input file. Also offers a method to scale this
 *          matrix to another time period.
 *
 * @see http://ccruncher.net/ifileref.html#transitions
 */
class Transitions : public ExpatHandlers
{

  private:

    //! Period (in months) that this transition matrix covers
    int period;
    //! Matrix values
    std::vector<std::vector<double> > matrix;
    //! List of ratings
    Ratings ratings;
    //! Index of default rating
    int indexdefault;
    //! Regularization error
    double rerror;

  private:

    //! Matrix product (M12 = M1·M2)
    static void prod(const std::vector<std::vector<double> > &M1,
                     const std::vector<std::vector<double> > &M2,
                     std::vector<std::vector<double> > &M12);

    //! Insert a transition value into the matrix
    void insertTransition(const std::string &r1, const std::string &r2, double val) throw(Exception);
    //! Validate object content
    void validate() throw(Exception);
    //! Computes Cumulated Default Forward Rate
    void cdfr(size_t numrows, std::vector<std::vector<double> > &ret) const;

  protected:

    //! Directives to process an xml start tag element
    void epstart(ExpatUserData &, const char *, const char **);
    //! Directives to process an xml end tag element
    void epend(ExpatUserData &, const char *);

  public:

    //! Default constructor
    Transitions();
    //! Constructor
    Transitions(const Ratings &) throw(Exception);
    //! Constructor
    Transitions(const Ratings &, const std::vector<std::vector<double> > &, int) throw(Exception);
    //! Set ratings
    void setRatings(const Ratings &);
    //! Matrix dimension (=number of ratings)
    size_t size() const;
    //! Returns period (in months) that covers this matrix
    int getPeriod() const;
    //! Returns default rating index
    int getIndexDefault() const;
    //! Regularize the transition matrix
    void regularize();
    //! Returns equivalent transition matrix that covers t months
    Transitions scale(int t) const throw(Exception);
    //! Computes default probabilities functions related to this transition matrix
    DefaultProbabilities getDefaultProbabilities(const Date &date, int numrows) const throw(Exception);
    //! Regularization error (|non_regularized| - |regularized|)
    double getRegularizationError() const;
    //! Matrix element access
    const std::vector<double>& operator[] (int row) const;

};

} // namespace

#endif
