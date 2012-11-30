
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <gsl/gsl_matrix.h>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "params/Factors.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Correlations : public ExpatHandlers
{

  private:

    // list of factors
    Factors factors;
    // matrix values
    vector<vector<double> > matrix;

  private:

    // insert a new matrix value
    void insertCorrelation(const string &r1, const string &r2, double val) throw(Exception);
    // validate object content
    void validate() throw(Exception);

  protected:
  
    // ExpatHandler method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandler method
    void epend(ExpatUserData &, const char *);
  
  public:

    // constructor
    Correlations();
    // constructor
    Correlations(const Factors &) throw(Exception);
    // initialize object
    void setFactors(const Factors &) throw(Exception);
    // return factors
    const Factors &getFactors() const;
    // matrix size (= number of factors)
    int size() const;
    // matrix element access
    const vector<double>& operator[] (int row) const;
    // return cholesky matrix of factors
    gsl_matrix * getCholesky() const throw(Exception);
    // return factor loadings
    vector<double> getFactorLoadings() const;
    // serializes object content as xml
    string getXML(int) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
