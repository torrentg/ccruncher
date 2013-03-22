
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2013 Gerard Torrent
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

#ifndef _Params_
#define _Params_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/ExpatHandlers.hpp"
#include "utils/Date.hpp"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Params : public ExpatHandlers
{

  private:

    // initialize variables
    void init();
    // parse a parameter
    void parseParameter(ExpatUserData &, const char **) throw(Exception);
    // validate object content
    void validate() const throw(Exception);

  public:

    // time.0 param value
    Date time0;
    // time.T param value
    Date timeT;
    // stopcriteria.maxiterations param value
    int maxiterations;
    // stopcriteria.maxseconds param value
    int maxseconds;
    // copula.type param value
    std::string copula_type; // gaussian, t(4)
    // rng.seed param value
    unsigned long rng_seed;
    // antithetic param value
    bool antithetic;
    // lhs size param value
    unsigned short lhs_size;
    // portfolio.onlyActiveClients param value
    bool onlyactive;
    // simulation block size
    unsigned short blocksize;

  protected:

    // ExpatHandlers method
    void epstart(ExpatUserData &, const char *, const char **);
    // ExpatHandlers method
    void epend(ExpatUserData &, const char *);
  
  public:
  
    // constructor
    Params();
    // returns copula type as string (gaussian or t)
    static std::string getCopulaType(const std::string &str) throw(Exception);
    // returns copula type as string (gaussian or t)
    std::string getCopulaType() const throw(Exception);
    // returns copula param (if exists)
    static double getCopulaParam(const std::string &str) throw(Exception);
    // returns copula param (if exists)
    double getCopulaParam() const throw(Exception);

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
