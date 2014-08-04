
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

#include <ctime>
#include <cmath>
#include <cstdio>
#include <cassert>
#include <sys/time.h>
#include "utils/Timer.hpp"

using namespace std;

/**************************************************************************//**
 * @details Create a Timer object. Timer can be stoped (default) or started.
 * @param[in] started Flag indicating if the Timer is automatically started
 *            just after creation.
 */
ccruncher::Timer::Timer(bool started)
{
  running_ = false;
  start_time_ = 0.0;
  total_ = 0.0;
  if (started) start();
}

/**************************************************************************//**
 * @details Internal method.
 * @return Current time as seconds and floating point value.
 */
double ccruncher::Timer::seconds()
{
  timeval tv;
  gettimeofday(&tv, nullptr);
  return double(tv.tv_sec) + double(tv.tv_usec)/1000000.0;
}

/**************************************************************************//**
 * @details Starts timer setting accumulated time to 0.
 */
void ccruncher::Timer::start()
{
  running_ = true;
  total_ = 0.0;
  start_time_ = seconds();
}

/**************************************************************************//**
 * @details Stops timer (if running) and set acumulated time to 0.
 */
void ccruncher::Timer::reset()
{
  running_ = false;
  start_time_ = 0.0;
  total_ = 0.0;
}

/**************************************************************************//**
 * @details Stops timer and adds elapsed time to acumulated time. Returns
 *          total acumulated time in seconds.
 * @return Accumulated time in seconds.
 */
double ccruncher::Timer::stop()
{
  if (running_)
  {
    total_ += (seconds() - start_time_);
    running_ = false;
  }
  return total_;
}

/**************************************************************************//**
 * @details If timer is running do nothing. If timer is stoped then starts
 *          timer (preserving accumulated time).
 */
void ccruncher::Timer::resume()
{
  if (!running_)
  {
    start_time_ = seconds();
    running_ = true;
  }
}

/**************************************************************************//**
 * @details Return the number of accumulated seconds.
 * @return Accumulated number of seconds.
 */
double ccruncher::Timer::read()
{
  if (running_)
  {
    stop();
    resume();
  }
  return total_;
}

/**************************************************************************//**
 * @param[in] secs Numbers of elapsed seconds.
 * @return Elapsed time formated like hh:mm:ss.mmm
 */
string ccruncher::Timer::format(double secs)
{
  //assert(secs >= -1e-14);
  secs = fabs(secs);

  char buf[20];
  double cur = secs;

  // computing milliseconds
  int ms = int((cur-floor(cur+1.0E-14)+1.0E-14)*1000.0);
  cur = floor(cur+1.0E-14) + 1.0E-8;

  // computing hours
  int hh = int(cur/3600.0);
  cur = cur - double(hh)*3600.0;

  // computing minutes
  int min = int(cur/60.0);
  cur = cur - double(min)*60.0;

  // computing seconds
  int sec = int(floor(cur));
  //cur = cur-double(sec);

  // formating elapsed time
  snprintf(buf, 20, "%02d:%02d:%02d.%03d", hh, min, sec, ms);

  // exit function
  return string(buf);
}

