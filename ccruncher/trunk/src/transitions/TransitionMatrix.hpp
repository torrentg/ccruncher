
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
// TransitionMatrix.hpp - TransitionMatrix header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/04/22 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added tma (Forward Default Rate) and tmaa (Cumulated Forward Default Rate)
//
// 2005/05/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added survival function (1-TMAA)
//
//===========================================================================

#ifndef _TransitionMatrix_
#define _TransitionMatrix_

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

class TransitionMatrix : public ExpatHandlers
{

  private:

    void init(Ratings *) throw(Exception);
    void insertTransition(const string &r1, const string &r2, double val) throw(Exception);
    void validate() throw(Exception);

  public:

    int n;
    double period;
    double **matrix;
    double epsilon;
    Ratings *ratings;

    int indexdefault;

    TransitionMatrix(Ratings *) throw(Exception);
    TransitionMatrix(TransitionMatrix &) throw(Exception);
    ~TransitionMatrix();

    int size();
    double ** getMatrix();
    int getIndexDefault();
    int evalue(const int irating, const double val);
    string getXML(int) throw(Exception);

    /** ExpatHandlers methods declaration */
    void epstart(ExpatUserData &, const char *, const char **);
    void epend(ExpatUserData &, const char *);

};

//---------------------------------------------------------------------------

TransitionMatrix * translate(TransitionMatrix *tm, double t) throw(Exception);
void tmaa(TransitionMatrix *tm, int numyears, double **ret) throw(Exception);
void tma(TransitionMatrix *tm, int numyears, double **ret) throw(Exception);
void survival(TransitionMatrix *tm, int numyears, double **ret) throw(Exception);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
