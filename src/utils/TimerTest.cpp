
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

#include <cmath>
#include "utils/Timer.hpp"
#include "utils/TimerTest.hpp"

using namespace std;
using namespace ccruncher;

//---------------------------------------------------------------------------

#define EPSILON 1E-14
#define NUMOPS 50000000

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::TimerTest::test1()
{
  double t1;
  double x = 2.1;
  Timer timer;

  ASSERT_EQUALS_EPSILON(0.0, timer.read(), EPSILON);
  timer.start();

  // making some stuff
  for(int i=0;i<NUMOPS;i++)
  {
    x = cos(x);
  }

  timer.stop();
  t1 = timer.read();
  ASSERT(x != 2.1);
  ASSERT(0.0 < t1);

  // reusing the timer (without lost acumulated time)
  timer.resume();

  // making more stuff
  for(int i=0;i<NUMOPS;i++) {
    x = cos(x);
  }

  // stopping the timer
  timer.stop();
  ASSERT(x != 2.1);
  ASSERT(t1 < timer.read());

  // reseting timer (acumulated time = 0 seconds)
  timer.reset();
  ASSERT_EQUALS_EPSILON(0.0, timer.read(), EPSILON);
}

//===========================================================================
// test2. test format function
//===========================================================================
void ccruncher_test::TimerTest::test2()
{
  ASSERT(Timer::format(0.510) == string("00:00:00.510"));
  ASSERT(Timer::format(50.510) == string("00:00:50.510"));
  ASSERT(Timer::format(500.510) == string("00:08:20.510"));
  ASSERT(Timer::format(5000.510) == string("01:23:20.510"));
  ASSERT(Timer::format(1555000.510) == string("431:56:40.510"));
}
