
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2005 Gerard Torrent
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
//
// Survival.hpp - Survival header - $Rev$
// --------------------------------------------------------------------------
//
// 2005/05/14 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/07/29 - Gerard Torrent [gerard@mail.generacio.com]
//   . improved performance for inverse method
//
// 2005/07/31 - Gerard Torrent [gerard@mail.generacio.com]
//   . added getMinCommonTime() method
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
// 2009/02/14 - Gerard Torrent [gerard@mail.generacio.com]
//   . removed maxmonths attribute
//   . inverse() returns double
//
//===========================================================================

#ifndef _Survival_
#define _Survival_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <vector>
#include "utils/ExpatHandlers.hpp"
#include "utils/Exception.hpp"
#include "ratings/Ratings.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Survival : public ExpatHandlers
{

  private:

    // survival function for each rating
    vector<double> *ddata;
    // inverse survival function values
    double **idata;
    // number of ratings
    int nratings;
    // pointer to ratings table
    Ratings *ratings;
    // epsilon used to compare doubles
    double epsilon;

    // initialize object
    void init(const Ratings &) throw(Exception);
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
    double inverse1(const int irating, double val) const;


  public:

    // constructor
    Survival(const Ratings &) throw(Exception);
    // constructor
    Survival(const Ratings &, int, int *, double**) throw(Exception);
    // destructor
    ~Survival();

    // evalue survival for irating at t
    double evalue(const int irating, int t) const;
    // evalue inverse survival for irating at t
    double inverse(const int irating, double val) const;
    // return minimal defined time (in months)
    int getMinCommonTime() const;
    // serialize object content as xml
    string getXML(int) const throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
