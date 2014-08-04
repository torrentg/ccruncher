
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
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

#include <cmath>
#include "utils/Timer.hpp"
#include "utils/TimerTest.hpp"

using namespace std;
using namespace ccruncher;

#define EPSILON 1E-14
#define NUMOPS 50000000

//===========================================================================
// test1
//===========================================================================
void ccruncher_test::TimerTest::test1()
{
  double t1;
  Timer timer;

  ASSERT_EQUALS_EPSILON(0.0, timer.read(), EPSILON);
  timer.start();

  /*
    waiting 1 second.
    this is not exactly true because time_t has a
    resolution of 1 second and trunc/round current
    value. eg: 1043832.4 sec -> 1043832
    if we add 1 second and wait, then we observe
    an ellapsed time of 0.6 seconds.
  */
  time_t time1 = time(0) + 1;
  while (time(0) < time1);

  timer.stop();
  t1 = timer.read();
  ASSERT(0.0 < t1);

  // reusing the timer (without lossing acumulated time)
  timer.resume();

  // waiting 1 second
  time_t time2 = time(0) + 1;
  while (time(0) < time2);

  // stopping the timer
  timer.stop();
  ASSERT(t1 < timer.read());

  // reseting timer (acumulated time = 0 seconds)
  timer.reset();
  ASSERT_EQUALS_EPSILON(0.0, timer.read(), EPSILON);

  // checking operators
  timer += 10.0;
  ASSERT(0.0 <= 10.0-timer.read());

  timer -= 10.0;
  ASSERT(timer.read() < 1.0);
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
