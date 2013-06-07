
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#ifndef _TMFCopulaTest_
#define _TMFCopulaTest_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <MiniCppUnit.hxx>
#include "math/TMFCopula.hpp"

//---------------------------------------------------------------------------

using namespace ccruncher;
namespace ccruncher_test {

//---------------------------------------------------------------------------

class TMFCopulaTest : public TestFixture<TMFCopulaTest>
{

  private:

    int getSector(int x, int *n, int m);
    double pearsn(const vector<double> &x, const vector<double> &y);
    void testCopula(Copula &, const vector<vector<double> > &, int *n);
    void computeDensity(Copula &copula);
    virtual vector<vector<double> > spearman(const vector<vector<double> > &);

    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();


  public:

    TEST_FIXTURE(TMFCopulaTest)
    {
      TEST_CASE(test1);
      TEST_CASE(test2);
      TEST_CASE(test3);
      TEST_CASE(test4);
      TEST_CASE(test5);
      TEST_CASE(test6);
    }

};

REGISTER_FIXTURE(TMFCopulaTest)

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------