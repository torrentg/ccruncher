
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
// UtilsTest.hpp - UtilsTest header - $Rev$
// --------------------------------------------------------------------------
//
// 2005/07/27 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#ifndef _UtilsTest_
#define _UtilsTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class UtilsTest : public TestFixture<UtilsTest>
{

  private:

    void test1(void);


  public:

    TEST_FIXTURE(UtilsTest)
    {
      TEST_CASE(test1);
    }

    void setUp();
    void tearDown();

};

REGISTER_FIXTURE(UtilsTest);

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
