
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

#include <sys/time.h>
#include <ctime>
#include <cctype>
#include <cstdio>
#include <algorithm>
#include <cassert>
#include "utils/Utils.hpp"

#ifdef _WIN32
  #include <windows.h>
#elif MACOS
  #include <sys/param.h>
  #include <sys/sysctl.h>
#else
  #include <unistd.h>
#endif

using namespace std;

/**************************************************************************//**
 * @details Format current timestamp as <code>'dd/MM/yyyy hh:mm:ss'</code>.
 * @return Current timestamp formated as string.
 */
string ccruncher::Utils::timestamp()
{
  time_t now = time(NULL);
  tm lt = *(localtime(&now));

  char aux[] = "dd/mm/yyyy hh:mm:ss ";
  aux[19] = 0;

  snprintf(aux, 20, "%02d/%02d/%04d %02d:%02d:%02d", lt.tm_mday, lt.tm_mon+1,
           lt.tm_year+1900, lt.tm_hour, lt.tm_min, lt.tm_sec);

  return string(aux);
}

/**************************************************************************//**
 * @details This value can be used to seed a RNG (random number generator).
 *          It has microseconds resolution.
 * @return Value based on current time.
 */
unsigned long ccruncher::Utils::trand()
{
  timeval tv;
  gettimeofday(&tv,NULL);
  return (unsigned long)(tv.tv_sec)*1000000UL + (unsigned long)(tv.tv_usec);
}

/**************************************************************************//**
 * @details Compilation options are:
 *          <ul>
 *            <li> debug. <code>\#define NDEBUG</code></li>
 *            <li> profiler. <code>\#define PROFILER</code></li>
 *          </ul>.
 *          String is formated using this pattern:
 *          <code>'debug[enabled] | profiler[disabled]'</code>.
 * @return A string with compilation options.
 */
string ccruncher::Utils::getCompilationOptions()
{
  string ret = "";

  // debug option
  ret += "debug";
#ifdef NDEBUG
  ret += "[disabled] | ";
#else
  ret += "[enabled] | ";
#endif

  // profiler option
  ret += "profiler";
#ifdef PROFILER
  ret += "[enabled] ";
#else
  ret += "[disabled] ";
#endif

  return ret;
}

/**************************************************************************//**
 * @details This method is platform dependent.
 *          Supported platforms are: Unix/Windows/Mac.
 * @return The number of cores.
 */
int ccruncher::Utils::getNumCores()
{
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#elif MACOS
    int nm[2];
    size_t len = 4;
    uint32_t count;

    nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);

    if(count < 1) {
        nm[1] = HW_NCPU;
        sysctl(nm, 2, &count, &len, NULL, 0);
        if(count < 1) { count = 1; }
    }
    return count;
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

/**************************************************************************//**
 * @details Tokens are delimited by a character, and user can provide more
 *          than one character.
 * @param[in] str String to tokenize.
 * @param[out] tokens List of tokens found.
 * @param[in] delimiters Set of characters used as delimiters (eg. ',;').
 */
void ccruncher::Utils::tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }
}

/**************************************************************************//**
 * @details Removes starting and ending space characters.
 * @param[in] str String to be trimmed.
 * @return String trimmed.
 */
string ccruncher::Utils::trim(const string &str)
{
  if (str.length() == 0) return "";

  int pos1 = str.length()-1;
  for(int i=0; i<(int)str.length(); i++) {
    if (!isspace(str[i])) {
      pos1=i;
      break;
    }
  }

  int pos2 = 0;
  for(int i=(int)str.length()-1; i>=0; i--) {
    if (!isspace(str[i])) {
      pos2=i;
      break;
    }
  }

  if (pos2 < pos1) {
    return "";
  }
  else if (pos1 == pos2) {
    if (isspace(str[pos1])) return "";
    else return str.substr(pos1,1);
  }
  else {
    return str.substr(pos1,pos2-pos1+1);
  }
}

/**************************************************************************//**
 * @param[in] str String to be uppercased.
 * @return String uppercased.
 */
string ccruncher::Utils::uppercase(const string &str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) toupper);

  return res;
}

/**************************************************************************//**
 * @param[in] str String to be lowercased.
 * @return String lowercased.
 */
string ccruncher::Utils::lowercase(const string &str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) tolower);

  return res;
}

