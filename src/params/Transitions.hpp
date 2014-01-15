
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

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "params/Ratings.hpp"
#include "params/DefaultProbabilities.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Transitions : public ExpatHandlers
{

  private:

    // period (in months) that this transition matrix covers
    int period;
    // matrix values
    std::vector<std::vector<double> > matrix;
    // list of ratings
    Ratings ratings;
    // index of default rating
    int indexdefault;
    // regularization error
    double rerror;

  private:

    // matrix product (M12 = M1·M2)
    static void prod(const std::vector<std::vector<double> > &M1, const std::vector<std::vector<double> > &M2, std::vector<std::vector<double> > &M12);
    // insert a transition value into the matrix
    void insertTransition(const std::string &r1, const std::string &r2, double val) throw(Exception);
    // validate object content
    void validate() throw(Exception);
    // computes Cumulated Default Forward Rate
    void cdfr(size_t numrows, std::vector<std::vector<double> > &ret) const throw(Exception);

  protected:

    //! Directives to process an xml start tag element
    void epstart(ExpatUserData &, const char *, const char **);
    //! Directives to process an xml end tag element
    void epend(ExpatUserData &, const char *);

  public:

    // default constructor
    Transitions();
    // constructor
    Transitions(const Ratings &) throw(Exception);
    // constructor
    Transitions(const Ratings &, const std::vector<std::vector<double> > &, int) throw(Exception);
    // set ratings
    void setRatings(const Ratings &);
    // returns n (number of ratings)
    size_t size() const;
    // returns period that covers this matrix
    int getPeriod() const;
    // returns default rating index
    int getIndexDefault() const;
    // regularize the transition matrix
    void regularize() throw(Exception);
    // returns equivalent transition matrix that covers t months
    Transitions scale(int t) const throw(Exception);
    // computes default probabilities functions related to this transition matrix
    DefaultProbabilities getDefaultProbabilities(const Date &date, int numrows) const throw(Exception);
    // regularization error (|non_regularized| - |regularized|)
    double getRegularizationError() const;
    // matrix element access
    const std::vector<double>& operator[] (int row) const;

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
