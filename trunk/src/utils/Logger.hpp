
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

#ifndef _Logger_
#define _Logger_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include "utils/Timer.hpp"

//---------------------------------------------------------------------------

using namespace std;
using namespace ccruncher;
namespace ccruncher {

//---------------------------------------------------------------------------

class Logger
{

  private:

    // verbosity level (0=none, 1=verbose)
    static bool verbose;
    // current indentation level
    static int ilevel;
    // internal columns counter
    static int curcol;

  private:

    // private constructor (non-instantiable class)
    Logger();
    // initialization stuff
    static void init(bool verbose_);

  public:

    // set verbosity
    static void setVerbosity(bool verbose_);
    // set ilevel = ilevel+1
    static void newIndentLevel();
    // set ilevel = ilevel-1
    static void previousIndentLevel();
    // add a blank line
    static void addBlankLine();
    // trace a message
    static void append(const string &msg);
    // trace a message
    static void trace(const string &msg);
    // trace a message
    static void trace(const string &msg, const string &value);
    // trace a message
    static void trace(const string &msg, Timer &timer);
    // trace a message
    static void trace(const string &msg, char c);
    // flush content
    static void flush();

};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
