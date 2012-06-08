
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

#include "utils/Timer.hpp"
#include <ctime>
#include <cmath>
#include <cstdio>
#include <cassert>

#ifdef _MSC_VER
#include "win32/gettimeofday.h"
#else
#include <sys/time.h>
#endif

//===========================================================================
// Create an instance of a Stopwatch, with its own internal counter
//===========================================================================
ccruncher::Timer::Timer(bool started)
{
  running_ = false;
  start_time_ = 0.0;
  total_ = 0.0;
  if (started) start();
  //secs_per_tick = 1.0 / CLOCKS_PER_SEC;
}

//===========================================================================
// seconds
// gettimeofday() return system time
// clock() return cpu process time
//===========================================================================
double ccruncher::Timer::seconds()
{
  timeval tv;
  gettimeofday(&tv, NULL);
  return double(tv.tv_sec) + double(tv.tv_usec)/1000000.0;
  //return ( (double) clock() ) * secs_per_tick;
}

//===========================================================================
// Start timing from 0.00.
//===========================================================================
void ccruncher::Timer::start()
{
  running_ = true;
  total_ = 0.0;
  start_time_ = seconds();
}

//===========================================================================
// Estableix el contador a 0
//===========================================================================
void ccruncher::Timer::reset()
{
  running_ = false;
  start_time_ = 0.0;
  total_ = 0.0;
}

//===========================================================================
// Stop timing and return elapsed time (in seconds).
//===========================================================================
double ccruncher::Timer::stop()
{
  if (running_)
  {
    total_ += (seconds() - start_time_);
    running_ = false;
  }
  return total_;
}

//===========================================================================
// Resume timing, if currently stopped.  Operation
// has no effect if Stopwatch is already running_.
//===========================================================================
void ccruncher::Timer::resume()
{
  if (!running_)
  {
    start_time_ = seconds();
    running_ = true;
  }
}

//===========================================================================
// Read current time (in seconds).
//===========================================================================
double ccruncher::Timer::read()
{
  if (running_)
  {
    stop();
    resume();
  }
  return total_;
}

//===========================================================================
// returns elapsed time in seconds formated like hh:mm:ss.mmm
//===========================================================================
string ccruncher::Timer::format(double val)
{
  assert(val >= -1e-14);
  val = fabs(val);

  char buf[20];
  double cur = val;

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
  sprintf(buf,"%02d:%02d:%02d.%03d", hh, min, sec, ms);

  // exit function
  return string(buf);
}