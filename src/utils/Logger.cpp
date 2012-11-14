
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2012 Gerard Torrent
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

#include <cstring>
#include "utils/Logger.hpp"
#include "utils/Strings.hpp"
#include "utils/Utils.hpp"
#include <cassert>

//===========================================================================
// constructor
//===========================================================================
ccruncher::Logger::Logger(streambuf *s) : ostream(s)
{
  isize = 2;
  ilevel = 0;
  numcols = 80;
  curcol = 0;
}

//===========================================================================
// setIndentSize
//===========================================================================
void ccruncher::Logger::setIndentSize(size_t v)
{
  isize = v;
}

//===========================================================================
// setNumCols
//===========================================================================
void ccruncher::Logger::setNumCols(size_t v)
{
  numcols = v;
}

//===========================================================================
// indent
//===========================================================================
size_t ccruncher::Logger::indent(int v)
{
  ilevel = std::max(0, (int)(ilevel)+v);
  return ilevel;
}

//===========================================================================
// repeat fill char n times
//===========================================================================
void ccruncher::Logger::repeat(size_t n, char c)
{
  if (c==0) c = fill();
  for(size_t i=0; i<n; i++) {
    ostream::put(c);
    curcol++;
  }
}

//===========================================================================
// flood
//===========================================================================
void ccruncher::Logger::flood(char c)
{
  if (curcol == 0) repeat(ilevel*isize, ' ');
  else *this << ' ';
  repeat(numcols-curcol,c);
}

//===========================================================================
// global function
//===========================================================================
Logger& ccruncher::operator<<(Logger& os, char c)
{
  if (os.curcol == 0) os.repeat(os.ilevel*os.isize, ' ');
  static_cast<ostream&>(os) << c;
  if (c == '\n') os.curcol = 0;
  else os.curcol++;
  return os;
}

//===========================================================================
// global function
//===========================================================================
Logger& ccruncher::operator<<(Logger& os, const char* s)
{
  const char *ptr = strchr(s, '\n');

  while(ptr != NULL)
  {
    if (os.curcol == 0) os.repeat(os.ilevel*os.isize, ' ');
    size_t l = ptr - s;
    static_cast<ostream&>(os) << string(s, l);
    os.curcol += l;
    os << '\n';
    os.curcol = 0;
    s = ptr+1;
    ptr = strchr(s, '\n');
  }

  if (os.curcol == 0) os.repeat(os.ilevel*os.isize, ' ');
  static_cast<ostream&>(os) << string(s);
  os.curcol += strlen(s);
  return os;
}

//===========================================================================
// global function
//===========================================================================
Logger& ccruncher::operator<<(Logger& os, const string& str)
{
  os << str.c_str();
  return os;
}

//===========================================================================
// global function
//===========================================================================
Logger& ccruncher::operator<<(Logger& os, const Date &date)
{
  os << date.toString().c_str();
  return os;
}

//===========================================================================
// global function
//===========================================================================
Logger& ccruncher::operator<<(Logger& os, Timer &timer)
{
  os << Timer::format(timer.read()).c_str();
  return os;
}

//===========================================================================
// endl manipulator
//===========================================================================
Logger& ccruncher::endl(Logger &os)
{
  os << '\n';
  os.flush();
  return os;
}

//===========================================================================
// split manipulator
//===========================================================================
Logger& ccruncher::split(Logger &logger)
{
  if (logger.getCurCol() > 0) logger << ' ';
  size_t len = std::max(size_t(0), logger.getNumCols() - logger.getCurCol());
  logger.width(len);
  logger.fill(' ');
  logger.setf(ios_base::right, ios_base::adjustfield);
  return logger;
}

//===========================================================================
// header manipulator
//===========================================================================
Logger& ccruncher::header(Logger &log)
{
  log << "general information" << flood('*') << endl;
  log << indent(+1);
  log << "ccruncher version" << split << string(PACKAGE_VERSION)+" ("+string(SVN_VERSION)+")" << endl;
  log << "start time (dd/MM/yyyy hh:mm:ss)" << split << Utils::timestamp() << endl;
  log << indent(-1);
  return log;
}

