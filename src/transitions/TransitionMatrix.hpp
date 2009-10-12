
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

#ifndef _TransitionMatrix_
#define _TransitionMatrix_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "ratings/Ratings.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class TransitionMatrix : public ExpatHandlers
{

  private:

    // nxn = matrix size (n=number of ratings)
    int n;
    // period (in months) that this transition matrix covers
    int period;
    // matrix values
    double **matrix;
    // list of ratings
    Ratings *ratings;
    // index of default rating
    int indexdefault;

    // insert a transition value into the matrix
    void insertTransition(const string &r1, const string &r2, double val) throw(Exception);
    // validate object content
    void validate() throw(Exception);


  public:

    // default constructor
    TransitionMatrix();
    // constructor
    TransitionMatrix(const Ratings &) throw(Exception);
    // copy constructor
    TransitionMatrix(const TransitionMatrix &) throw(Exception);
    // destructor
    ~TransitionMatrix();

    // set ratings
    void setRatings(const Ratings &);
    // returns n (number of ratings)
    int size() const;
    // returns period that covers this matrix
    int getPeriod() const;
    // returns pointer to matrix values
    double ** getMatrix() const;
    // returns default rating index
    int getIndexDefault() const;
    // simulate transition with random value val
    int evalue(const int irating, const double val) const;
    // serialize object content as xml
    string getXML(int) const throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

    // returns equivalent transition matrix that covers t months
    friend TransitionMatrix * translate(const TransitionMatrix &tm, int t) throw(Exception);
    // computes Cumulated Default Forward Rate
    friend void cdfr(const TransitionMatrix &tm, int steplength, int numrows, double **ret) throw(Exception);
    // computes survival function related to this transition matrix
    friend void survival(const TransitionMatrix &tm, int steplength, int numrows, double **ret) throw(Exception);

};

//---------------------------------------------------------------------------

  // returns equivalent transition matrix that covers t months
  TransitionMatrix * translate(const TransitionMatrix &tm, int t) throw(Exception);
  // computes Cumulated Default Forward Rate
  void cdfr(const TransitionMatrix &tm, int steplength, int numrows, double **ret) throw(Exception);
  // computes survival function related to this transition matrix
  void survival(const TransitionMatrix &tm, int steplength, int numrows, double **ret) throw(Exception);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
