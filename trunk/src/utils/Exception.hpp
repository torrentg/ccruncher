
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2010 Gerard Torrent
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

    // retrieves stack trace
    std::string retrieveStackTrace();

  protected:

    // stack trace info
    std::string stacktrace;
    // exception message
    std::string msg;

  public:

    // constructor
    Exception(const std::string&);
    // constructor
    Exception(const ccruncher::Exception &);
    // constructor
    Exception(const std::exception &);
    // constructor
    Exception(const ccruncher::Exception &, const std::string&);
    // constructor
    Exception(const std::exception &, const std::string&);
    // constructor
    Exception(const char *);
    // destructor
    ~Exception() throw ();
    // returns exception message
    const char * what() const throw();
    // returns exception message
    std::string toString() const;
    // returns stack trace
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
