
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2014 Gerard Torrent
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

#ifndef _ExceptionTest_
#define _ExceptionTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include "utils/MiniCppUnit.hxx"
#include "utils/Exception.hpp"

//---------------------------------------------------------------------------

namespace ccruncher_test {

//---------------------------------------------------------------------------

class ExceptionTest : public TestFixture<ExceptionTest>
{

  private:

    void function1();
    void function2() throw(ccruncher::Exception, std::exception);
    void function3() throw(ccruncher::Exception);
    void function4() throw();

    void test1();


  public:

    TEST_FIXTURE(ExceptionTest)
    {
      TEST_CASE(test1);
    }

};

REGISTER_FIXTURE(ExceptionTest)

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
