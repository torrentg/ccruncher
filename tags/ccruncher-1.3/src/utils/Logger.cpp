
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
// Logger.cpp - Logger code - $Rev$
// --------------------------------------------------------------------------
//
// 2005/03/22 - Gerard Torrent [gerard@mail.generacio.com]
//   . initial release
//
// 2005/05/14 - Gerard Torrent [gerard@mail.generacio.com]
//   . const + referenced string arguments
//
// 2005/05/20 - Gerard Torrent [gerard@mail.generacio.com]
//   . implemented Strings class
//
// 2005/07/30 - Gerard Torrent [gerard@mail.generacio.com]
//   . moved <cassert> include at last position
//
// 2005/10/15 - Gerard Torrent [gerard@mail.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <iostream>
#include "utils/Logger.hpp"
#include "utils/Strings.hpp"
#include <cassert>

// --------------------------------------------------------------------------

#define DEFAULT_INDENTATION_LEVEL 1
#define INDENTSIZE 2
#define MAXCOLS 69

//---------------------------------------------------------------------------

int ccruncher::Logger::ilevel = DEFAULT_INDENTATION_LEVEL;
int ccruncher::Logger::curcol = -1;
bool ccruncher::Logger::verbose = false;
bool ccruncher::Logger::tracetime = false;
Timer ccruncher::Logger::timer = Timer();

//===========================================================================
// constructor
//===========================================================================
ccruncher::Logger::Logger()
{
  init(false);
}

//===========================================================================
// init
//===========================================================================
void ccruncher::Logger::init(bool verbose_)
{
  verbose = verbose_;

  ilevel = DEFAULT_INDENTATION_LEVEL;
  curcol = -1;
  tracetime = false;
}

//===========================================================================
// constructor
//===========================================================================
void ccruncher::Logger::setVerbosity(bool verbose_)
{
  verbose = verbose_;
}

//===========================================================================
// newIndentLevel
//===========================================================================
void ccruncher::Logger::newIndentLevel()
{
  flush();
  ilevel++;
}

//===========================================================================
// previousIndentLevel
//===========================================================================
void ccruncher::Logger::previousIndentLevel()
{
  assert(ilevel > 0);
  flush();
  if (ilevel > 0) ilevel--;
}

//===========================================================================
// addBlankLine
//===========================================================================
void ccruncher::Logger::addBlankLine()
{
  // none if non-verbose mode enabled
  if (verbose == false) return;

  flush();
  cout << std::endl << std::flush;
}

//===========================================================================
// trace
//===========================================================================
void ccruncher::Logger::trace(const string &msg)
{
  trace(msg, false);
}

//===========================================================================
// trace
//===========================================================================
void ccruncher::Logger::trace(const string &msg, char c)
{
  trace(msg, c, false);
}

//===========================================================================
// trace
//===========================================================================
void ccruncher::Logger::trace(const string &msg, bool tracetime_)
{
  trace(msg, ' ', tracetime_);
}

//===========================================================================
// trace
//===========================================================================
void ccruncher::Logger::trace(const string &msg, const string &value)
{
  // none if non-verbose mode enabled
  if (verbose == false) return;

  // flushing previous message
  flush();

  // defining indentator
  string indentator = Strings::blanks(ilevel*INDENTSIZE);

  // creating intermediate filler
  int fsize = ilevel*INDENTSIZE + msg.size() + 1 + value.size();
  string sfill = Strings::blanks(max(0,MAXCOLS-fsize));

  // showing output
  cout << indentator << msg << " " << sfill << value << std::endl << std::flush;

  // setting status values
  tracetime = false;
  curcol = -1;
}

//===========================================================================
// trace
//===========================================================================
void ccruncher::Logger::trace(const string &msg, char c, bool tracetime_)
{
  // none if non-verbose mode enabled
  if (verbose == false) return;

  // flushing previous message
  flush();

  // defining indentator
  string indentator = Strings::blanks(ilevel*INDENTSIZE);

  // setting status values
  curcol = indentator.size() + msg.size();
  tracetime = tracetime_;

  // tracing msg
  cout << indentator + msg << std::flush;

  // post trace actions
  if (tracetime == false)
  {
    cout << " " << Strings::filler(max(0,MAXCOLS-curcol-1), c) << std::endl << std::flush;
    curcol = -1;
  }
  else
  {
    timer.start();
  }
}

//===========================================================================
// append
//===========================================================================
void ccruncher::Logger::append(const string &msg)
{
  // none if non-verbose mode enabled
  if (verbose == false) return;

  // checking if we need indentator
  if (curcol < 0)
  {
    tracetime = false;
    cout << Strings::blanks(ilevel*INDENTSIZE) << std::flush;
    curcol = ilevel*INDENTSIZE;
  }

  // appending msg
  cout << msg << std::flush;
  curcol += msg.size();

  // checking bounds
  if (curcol >= MAXCOLS)
  {
    cout << std::endl << std::flush;
    tracetime = false;
    curcol = -1;
  }
}

//===========================================================================
// notify
//===========================================================================
void ccruncher::Logger::notify()
{
  flush();
}

//===========================================================================
// flush
//===========================================================================
void ccruncher::Logger::flush()
{
  // none if non-verbose mode enabled
  if (verbose == false) return;

  // printing time + new line
  if (tracetime == true)
  {

    string etime = Timer::format(timer.stop());
    int fsize = int(MAXCOLS - (curcol + etime.size()));
    string filler = Strings::blanks(max(fsize, 0));

    // tracing time
    cout << filler << etime << std::endl << std::flush;

    // setting class values
    curcol = -1;
    tracetime = false;
  }

  if (curcol >= 0)
  {
    cout << std::endl << std::flush;
  }
}
