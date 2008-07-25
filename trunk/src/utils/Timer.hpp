
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004 Gerard Torrent
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
// Timer.hpp - Timer header - $Rev$
// --------------------------------------------------------------------------
//
// 2004/12/04 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/08/08 - Gerard Torrent [gerard@mail.generacio.com]
//   . running_ variable changed from int to boolean
//
// 2005/08/09 - Gerard Torrent [gerard@mail.generacio.com]
//   . replaced clock() function by gettimeofday() function
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
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

     bool running_;
     double start_time_;
     double total_;
     double seconds();
     //double secs_per_tick;

  public:
     Timer();
     void start();
     double stop();
     double read();
     void resume();
     int running();
     void reset();
     static string format(double seconds_);
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
