
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

#ifndef _Survivals_
#define _Survivals_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "params/Ratings.hpp"

#ifdef __GNUC__
#define NOINLINE  __attribute__((noinline))
#else
#define NOINLINE 
#endif

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Survivals : public ExpatHandlers
{

  private:

    // survival function for each rating
    vector<vector<double> > ddata;
    // inverse survival function values
    vector<vector<double> > idata;
    // ratings table
    Ratings ratings;
    // index of default rating
    int indexdefault;

  private:
  
    // insert a survival value
    void insertValue(const string &r1, int t, double val) throw(Exception);
    // validate object content
    void validate() throw(Exception);
    // fill holes in survival functions
    void fillHoles();
    // compute inverse for each survival function
    void computeInvTable();
    // linear interpolation algorithm
    double interpole(double x, double x0, double y0, double x1, double y1) const;
    // inverse function
    double inverse1(const int irating, double val) const NOINLINE;

  protected:

    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);

  public:

    // defaults constructor
    Survivals();
    // constructor
    Survivals(const Ratings &) throw(Exception);
    // constructor
    Survivals(const Ratings &, const vector<int> &imonths, const vector<vector<double> > &values) throw(Exception);
    // returns ratings size
    int size() const;
    // set ratings
    void setRatings(const Ratings &) throw(Exception);
    // return ratings
    const Ratings & getRatings() const;
    // return index of default rating
    int getIndexDefault() const;
    // evalue survival for irating at t
    double evalue(const int irating, int t) const;
    // evalue inverse survival for irating at t
    double inverse(const int irating, double val) const;
    // return minimal defined time (in months)
    int getMinCommonTime() const;
    // serialize object content as xml
    string getXML(int) throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------