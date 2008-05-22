
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
// Exception.hpp - Exception header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/06/05 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added getStackTrace() method (link with -rdynamic flag)
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#ifndef _Exception_
#define _Exception_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <stdexcept>
#include <iostream>
#include <string>

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class Exception : public std::exception
{

  private:

    std::string retrieveStackTrace();

  protected:

    std::string stacktrace;
    std::string msg;

  public:

    Exception(const std::string&);
    Exception(const ccruncher::Exception &);
    Exception(const std::exception &);
    Exception(const ccruncher::Exception &, const std::string&);
    Exception(const std::exception &, const std::string&);
    Exception(const char *);

    ~Exception() throw ();

    const char * what() const throw();
    std::string toString() const;
    std::string getStackTrace() const;

};

//---------------------------------------------------------------------------

// output operator
std::ostream& operator << (std::ostream& os, Exception const &e);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
