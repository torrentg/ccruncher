
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
// Logger.hpp - Logger header
// --------------------------------------------------------------------------
//
// 2005/03/22 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#ifndef _Logger_
#define _Logger_

//---------------------------------------------------------------------------

#include "config.h"
#include <string>
#include "Timer.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Logger
{

  private:

    /** verbosity level (0=none, 1=verbose) */
    static bool verbose;

    /** current indentation level */
    static int ilevel;

    /** internal status */
    static bool tracetime;

    /** internal timer */
    static Timer timer;

    /** internal columns counter */
    static int curcol;

    /** initialization stuff */
    static void init(bool verbose_);

    /** flush function */
    static void flush();


  public:

    /** constructor */
    Logger();

    /** set verbosity */
    static void setVerbosity(bool verbose_);

    /** set ilevel = ilevel+1 */
    static void newIndentLevel();

    /** set ilevel = ilevel-1 */
    static void previousIndentLevel();

    /** trace a message */
    static void trace(string msg);
    static void trace(string msg, string value);
    static void trace(string msg, bool tracetime_);
    static void trace(string msg, char c);
    static void trace(string msg, char c, bool tracetime_);
    static void notify();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
