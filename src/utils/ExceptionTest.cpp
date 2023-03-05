
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2023 Gerard Torrent
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

#include <iostream>
#include "utils/Exception.hpp"
#include "utils/ExceptionTest.hpp"

using namespace std;

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::ExceptionTest::test1()
{
  ASSERT_NO_THROW(function4());
  ASSERT_THROW(function3());
  ASSERT_THROW(function2());
  ASSERT_THROW(function1());
}

//===========================================================================
// function1. Can throw anything (no throw clause)
//===========================================================================
void ccruncher_test::ExceptionTest::function1()
{
  try
  {
    function2();
  }
  catch(ccruncher::Exception &e)
  {
    throw ccruncher::Exception(e, "exception throwed by function1()");
  }
  catch(...)
  {
    cout << "catched all exceptions distinct type of Exception" << endl;
  }
}

//===========================================================================
// function2. Can throw 2 exceptions types: Exception and exception
//===========================================================================
void ccruncher_test::ExceptionTest::function2()
{
  try
  {
    function3();
  }
  catch(ccruncher::Exception &e)
  {
    throw ccruncher::Exception(e, "exception throwed by function2()");
  }
}

//===========================================================================
// function3. Only can throw exceptions type Exception
//===========================================================================
void ccruncher_test::ExceptionTest::function3()
{
  function4();

  throw ccruncher::Exception("exception throwed by function3()");
}

//===========================================================================
// function4. Can't throw exceptions
//===========================================================================
void ccruncher_test::ExceptionTest::function4() noexcept
{
  // nothing to do
}
