
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
// Params.hpp - Params header
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/04/01 - Gerard Torrent [gerard@fobos.generacio.com]
//   . migrated from xerces to expat
//
// 2005/05/13 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added param montecarlo.method
//
//===========================================================================

#ifndef _Params_
#define _Params_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Params : public ExpatHandlers
{

  private:

    void init();
    void parseProperty(ExpatUserData &, const char **) throw(Exception);
    void validate(void) throw(Exception);


  public:

    Date begindate;
    int steps;
    int steplength;
    long maxiterations;
    long maxseconds;
    string copula_type; // normal
    string smethod;     // rating-path, time-to-default
    long copula_seed;
    bool antithetic;
    bool onlyactive;   // only active clients

    Params();
    ~Params();

    Date* getDates() throw(Exception);
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
