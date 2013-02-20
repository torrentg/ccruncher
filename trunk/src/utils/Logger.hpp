
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

#ifndef _Logger_
#define _Logger_

//---------------------------------------------------------------------------

#include "utils/config.h"
#include <string>
#include <ostream>
#include <streambuf>
#include "utils/Utils.hpp"
#include "utils/Timer.hpp"
#include "utils/Date.hpp"

//---------------------------------------------------------------------------

namespace ccruncher {

//---------------------------------------------------------------------------

class Logger : public std::ostream
{

  private:

    // indent size;
    size_t isize;
    // current indentation level
    size_t ilevel;
    // number of columns
    size_t numcols;
    // current column
    size_t curcol;

  public:

    // default constructor
    explicit Logger(std::streambuf *s=NULL);
    // indentation size
    void setIndentSize(size_t v);
    // number of columns
    void setNumCols(size_t);
    // return numcols
    size_t getNumCols() const { return numcols; }
    // return curcol
    size_t getCurCol() const { return curcol; }
    // increase/decrease indent levels
    size_t indent(int v=1);
    // repeat fill char n times
    void repeat(size_t n, char c=0);
    // flood
    void flood(char c);
    // center text
    void center(const std::string &);

    // formated output
    Logger& operator<<(bool val) { static_cast<std::ostream&>(*this) << (val?"true":"false"); return *this; }
    Logger& operator<<(short val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(unsigned short val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(int val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(unsigned int val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(long val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(unsigned long val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(float val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(double val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(long double val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(const void* val) { static_cast<std::ostream&>(*this) << val; return *this; }

    // manipulators calls
    Logger& operator<<(Logger& ( *pf )(Logger&)) { return pf(*this); }
    Logger& operator<<(std::ios& ( *pf )(std::ios&)) { pf(*this); return *this; }
    Logger& operator<<(std::ios_base& ( *pf )(std::ios_base&)) { pf(*this); return *this; }

    // friend functions
    friend Logger& operator<<(Logger& os, const char* s);
    friend Logger& operator<<(Logger& os, char c);
    friend Logger& operator<<(Logger& os, const std::string &);
    friend Logger& operator<<(Logger& os, const Date &);
    friend Logger& operator<<(Logger& os, Timer &);

};

//---------------------------------------------------------------------------

// manipulator
Logger& split(Logger &logger);

// manipulator
Logger& endl(Logger &logger);

// manipulator
Logger& flush(Logger &logger);

// manipulator
Logger& header(Logger &log);

// manipulator
Logger& copyright(Logger &log);

// manipulator
struct flood
{
  const char fill ;
  explicit flood(char c) : fill(c) {}
  inline friend Logger& operator<<(Logger& logger, const flood& manip)
  {
    logger.flood(manip.fill);
    return logger;
  }
};

// manipulator
struct indent
{
  int delta;
  explicit indent(int n=1) : delta(n) {}
  inline friend Logger& operator<<(Logger& logger, const indent& manip)
  {
    logger.indent(manip.delta);
    return logger;
  }
};

// manipulator
struct footer
{
  Timer &timer;
  explicit footer(Timer &t) : timer(t) {}
  inline friend Logger& operator<<(Logger& log, const footer& manip)
  {
    log << "general information" << flood('*') << endl;
    log << indent(+1);
    log << "end time (dd/MM/yyyy hh:mm:ss)" << split << Utils::timestamp() << endl;
    log << "total elapsed time" << split << manip.timer << endl;
    log << indent(-1);
    log << endl;
    return log;
  }
};

// manipulator
struct center
{
  std::string str;
  explicit center(const std::string &s) : str(s) {}
  inline friend Logger& operator<<(Logger& logger, const center& manip)
  {
    logger.center(manip.str);
    return logger;
  }
};

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
