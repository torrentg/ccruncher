
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

#ifndef _Timer_
#define _Timer_

#include <string>

namespace ccruncher {

/**************************************************************************//**
 * @brief   Utility class to count time lapses.
 *
 * @details Inspired in jama/tnt_stopwatch && boost/timer.
 *
 * @see http://math.nist.gov/tnt/download.html
 *
 * @code
 *   Timer timer;
 *   timer.start();
 *   // a heavy task here
 *   timer.stop();
 *   double t1 = timer.read();
 *   timer.resume();
 *   // another heavy task here
 *   double t2 = timer.read();
 * @endcode
 *
 */
class Timer
{

  private:

     //! Flag indicating if the timer is running
     bool running_;
     //! Start time (if running)
     double start_time_;
     //! Acumulated time in seconds
     double total_;

  private:
  
     //! Current time in seconds
     double seconds();

  public:
  
     //! Constructor
     Timer(bool started=false);
     //! Starts the timer
     void start();
     //! Stops the timer
     double stop();
     //! Returns the elapsed time (in seconds)
     double read();
     //! Resume timer
     void resume();
     //! Reset the timer
     void reset();
     //! Indicates if timer is running
     bool isRunning() const { return running_; }
     //! Format seconds in format hh:mm:ss.mmm
     static std::string format(double secs);
     //! Adds n seconds to accumulated time
     Timer& operator+=(double secs) { total_ += secs; return *this; }
     //! Subtracts n seconds to accumulated time
     Timer& operator-=(double secs) { total_ -= secs; return *this; }

};

} // namespace

#endif

