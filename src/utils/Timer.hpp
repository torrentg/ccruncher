
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

#ifndef _Timer_
#define _Timer_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>

//---------------------------------------------------------------------------

using namespace std;
namespace ccruncher {

//---------------------------------------------------------------------------

class Timer
{

  private:

     // flag indicating if the timer is running
     bool running_;
     // start time (if running)
     double start_time_;
     // acumulated time
     double total_;
     //double secs_per_tick;

  private:
  
     // number of seconds
     double seconds();

  public:
  
     // constructor
     Timer(bool started=false);
     // starts the timer
     void start();
     // stops the timer
     double stop();
     // returns the elapsed time
     double read();
     // resume timer
     void resume();
     // reset the timer
     void reset();
     // format seconds in format hh:mm:ss.mmm
     static string format(double seconds_);
     
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
