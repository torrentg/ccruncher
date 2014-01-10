
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
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

#ifndef _Logger_
#define _Logger_

#include <string>
#include <ostream>
#include <streambuf>
#include "utils/Utils.hpp"
#include "utils/Timer.hpp"
#include "utils/Date.hpp"

namespace ccruncher {

/**************************************************************************//**
 * @brief   CCruncher's trace.
 *
 * @details This logger provides methods to manage a 80-columns output
 *          supporting centered text, multi-level indenting, and 2 columns
 *          where the former is the description, and the second the value.
 *          This class extends std::ostream.
 *
 * @see     http://www.cplusplus.com/reference/ostream/ostream/
 *
 */
class Logger : public std::ostream
{

  private:

    //! Indent size
    size_t isize;
    //! Current indentation level
    size_t ilevel;
    //! Number of columns
    size_t numcols;
    //! Current column
    size_t curcol;

  public:

    //! Default constructor
    explicit Logger(std::streambuf *sb=NULL);
    //! Set indentation size
    void setIndentSize(size_t v);
    //! Set the number of columns
    void setNumCols(size_t v);
    //! Return the number of columns
    size_t getNumCols() const { return numcols; }
    //! Return the current column
    size_t getCurCol() const { return curcol; }
    //! Increase/decrease indent levels
    size_t indent(int v=1);
    //! Repeat fill char n times
    void repeat(size_t n, char c=0);
    //! Flood line with the given char
    void flood(char c);
    //! Center text
    void center(const std::string &str);

    // formated output
    template <class T> Logger& operator<<(T val) { static_cast<std::ostream&>(*this) << val; return *this; }
    Logger& operator<<(bool val) { static_cast<std::ostream&>(*this) << (val?"true":"false"); return *this; }
    Logger& operator<<(const char *s);
    Logger& operator<<(char c);
    Logger& operator<<(const std::string &str);
    Logger& operator<<(const Date &date);
    Logger& operator<<(Timer &timer);

    // manipulators calls
    Logger& operator<<(Logger& ( *pf )(Logger&)) { return pf(*this); }
    Logger& operator<<(std::ios& ( *pf )(std::ios&)) { pf(*this); return *this; }
    Logger& operator<<(std::ios_base& ( *pf )(std::ios_base&)) { pf(*this); return *this; }

};

//! Stream manipulator
Logger& split(Logger &logger);

//! Stream manipulator
Logger& endl(Logger &logger);

//! Stream manipulator
Logger& flush(Logger &logger);

//! Stream manipulator
Logger& header(Logger &logger);

//! Stream manipulator
Logger& copyright(Logger &logger);

//! Logger stream manipulator
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

//! Logger stream manipulator
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

//! Logger stream manipulator
struct footer
{
  Timer &timer;
  explicit footer(Timer &t) : timer(t) {}
  inline friend Logger& operator<<(Logger& logger, const footer& manip)
  {
    logger << "general information" << flood('*') << endl;
    logger << indent(+1);
    logger << "end time (dd/MM/yyyy hh:mm:ss)" << split << Utils::timestamp() << endl;
    logger << "total elapsed time" << split << manip.timer << endl;
    logger << indent(-1);
    logger << endl;
    return logger;
  }
};

//! Logger stream manipulator
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

} // namespace

#endif

