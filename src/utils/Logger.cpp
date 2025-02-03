
//===========================================================================
//
// CCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2025 Gerard Torrent
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
#include <cassert>
#include "utils/Logger.hpp"
#include "utils/Utils.hpp"
#include "utils/config.h"

using namespace std;
using namespace ccruncher;

/**************************************************************************//**
 * @details Creates Logger using this given streambuf. If the streambuf is
 *          not provided (s=NULL), then Logger creates its own.
 * @see http://www.cplusplus.com/reference/streambuf/streambuf/
 * @param[in] sb Stream buffer used by this logger. Can be <code>NULL</code>.
 */
ccruncher::Logger::Logger(streambuf *sb) : ostream(sb)
{
  isize = 2;
  ilevel = 0;
  numcols = 80;
  curcol = 0;
}

/**************************************************************************//**
 * @details Number of spaces used as indent.
 *          Default indent size is 2 spaces.
 * @param[in] v Indent size.
 */
void ccruncher::Logger::setIndentSize(size_t v)
{
  isize = v;
}

/**************************************************************************//**
 * @details Number of columns. If a line exceeds this size then it is
 *          printed as-is (exceding the maximum column size). The column
 *          size is used to center text and apply a flood.
 *          Default number of columns is 80.
 * @param[in] v Number of columns.
 */
void ccruncher::Logger::setNumCols(size_t v)
{
  numcols = v;
}

/**************************************************************************//**
 * @details Indent Number of columns. If a line exceeds this size then it is
 *          printed as-is (exceding the maximum column size). The column
 *          size is used to center text and apply a flood.
 *          Default number of columns is 80.
 * @param[in] v Number of indentation levels to add (positive) or
 *          substract (negative).
 */
size_t ccruncher::Logger::indent(int v)
{
  ilevel = std::max(0, (int)(ilevel)+v);
  return ilevel;
}

/**************************************************************************//**
 * @details Repeat the given char n times. If the fill char is unspecified
 *          (c=0) then the default fill char is used. Caution, this method
 *          don't take into account the maximum number of columns.
 * @see http://www.cplusplus.com/reference/ios/basic_ios/fill/
 * @param[in] n Number of times that the fill char is printed.
 * @param[in] c Fill char to use. If c=0 then uses the default fill char.
 */
void ccruncher::Logger::repeat(size_t n, char c)
{
  if (c==0) c = fill();
  for(size_t i=0; i<n; i++) {
    ostream::put(c);
    curcol++;
  }
}

/**************************************************************************//**
 * @details Repeat the given char to the maximum number of columns.
 * @param[in] c Fill char to use.
 */
void ccruncher::Logger::flood(char c)
{
  if (curcol == 0) repeat(ilevel*isize, ' ');
  else *this << ' ';
  repeat(numcols-curcol,c);
}

/**************************************************************************//**
 * @details Center the given text taking into account the maximum column
 *          size. It supports multi-line messages.
 * @param[in] str Message to print.
 */
void ccruncher::Logger::center(const std::string &str)
{
  vector<string> lines;
  Utils::tokenize(str, lines, "\n");
  for(string &line : lines)
  {
    line = Utils::trim(line);
    if (line.empty()) {
      *this << endl;
      continue;
    }
    int len2 = line.length();
    int len1 = std::max((numcols-len2)/2, (size_t)0);
    int len3 = std::max(numcols-len1-len2, (size_t)0);
    repeat(len1, ' ');
    *this << line;
    repeat(len3, ' ');
    *this << endl;
  }
}

/**************************************************************************//**
 * @details Prints a character taking into account the maximum column,
 *          line breaks and indentation.
 * @param[in] c Character to print.
 */
Logger& ccruncher::Logger::operator<<(char c)
{
  if (c != '\n' && curcol+1 > numcols) {
    static_cast<ostream&>(*this) << '\n';
    curcol = 0;
  }
  if (curcol == 0) repeat(ilevel*isize, ' ');
  static_cast<ostream&>(*this) << c;
  if (c == '\n') curcol = 0;
  else curcol++;
  return *this;
}

/**************************************************************************//**
 * @details Prints a character array taking into account the maximum column,
 *          line breaks and indentation.
 * @param[in] s String to print (0 terminated).
 */
Logger& ccruncher::Logger::operator<<(const char *s)
{
  const char *ptr = strchr(s, '\n');

  while(ptr != nullptr)
  {
    if (curcol == 0) repeat(ilevel*isize, ' ');
    size_t l = ptr - s;
    static_cast<ostream&>(*this) << string(s, l);
    curcol += l;
    *this << '\n';
    s = ptr+1;
    ptr = strchr(s, '\n');
  }

  if (curcol == 0) repeat(ilevel*isize, ' ');
  static_cast<ostream&>(*this) << string(s);
  curcol += strlen(s);
  return *this;
}

/**************************************************************************//**
 * @details Prints a string taking into account the maximum column,
 *          line breaks and indentation.
 * @param[in] str String to print.
 */
Logger& ccruncher::Logger::operator<<(const std::string &str)
{
  *this << str.c_str();
  return *this;
}

/**************************************************************************//**
 * @details Prints the given Date using the Date::toString() format.
 * @see Date#toString
 * @param[in] date Date to print.
 */
Logger& ccruncher::Logger::operator<<(const Date &date)
{
  *this << date.toString().c_str();
  return *this;
}

/**************************************************************************//**
 * @details Prints an end-of-line in the given Logger.
 * @param[in,out] logger Logger.
 */
Logger& ccruncher::endl(Logger &logger)
{
  logger << '\n';
  logger.flush();
  return logger;
}

/**************************************************************************//**
 * @details Flush the content of the given Logger.
 * @param[in,out] logger Logger.
 */
Logger& ccruncher::flush(Logger &logger)
{
  logger.flush();
  return logger;
}

/**************************************************************************//**
 * @details Split the content adjusting the next output to right and filling
 *          with spaces the intermediate columns. It is only allowed 1 split
 *          per line.
 * @param[in,out] logger Logger.
 */
Logger& ccruncher::split(Logger &logger)
{
  if (logger.getCurCol() > 0) logger << ' ';
  size_t len = std::max(size_t(0), logger.getNumCols() - logger.getCurCol());
  logger.width(len);
  logger.fill(' ');
  logger.setf(ios_base::right, ios_base::adjustfield);
  return logger;
}

/**************************************************************************//**
 * @details Prints the CCruncher header with info about version and
 *          starting time.
 * @param[in,out] logger Logger.
 */
Logger& ccruncher::header(Logger &logger)
{
  logger << "general information" << flood('*') << endl;
  logger << indent(+1);
  logger << "ccruncher version" << split << string(PACKAGE_VERSION)+" ("+string(GIT_VERSION)+")" << endl;
  logger << "start time (dd/MM/yyyy hh:mm:ss)" << split << Utils::timestamp() << endl;
  logger << indent(-1);
  return logger;
}

/**************************************************************************//**
 * @details Prints the CCruncher multi-line copyright notice.
 * @param[in,out] logger Logger.
 */
Logger& ccruncher::copyright(Logger &logger)
{
  string str =
    "\nccruncher is Copyright (C) 2004-2025 Gerard Torrent and licensed\n"
    "under the GNU General Public License, version 2. More info at\n"
    "http://www.ccruncher.net";
  logger.center(str);
  return logger;
}

