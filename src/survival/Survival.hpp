
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
// Survival.hpp - Survival header
// --------------------------------------------------------------------------
//
// 2005/05/14 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
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

    void init(Ratings *) throw(Exception);
    void insertValue(const string &r1, int t, double val) throw(Exception);
    void validate() throw(Exception);
    void fillHoles();
    double interpole(double x, double x0, double y0, double x1, double y1);

    int maxmonths;
    vector<double> *data;
    int nratings;
    Ratings *ratings;
    double epsilon;

  public:

    Survival(Ratings *) throw(Exception);
    Survival(Ratings *, int, int *, double**, int) throw(Exception);
    ~Survival();

    void evalue(int steplength, int numrows, double **ret);
    double evalue(const int irating, int t);
    int inverse(const int irating, double val);

    string getXML(int) throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
