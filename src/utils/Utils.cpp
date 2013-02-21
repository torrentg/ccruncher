
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

#include <ctime>
#include <cctype>
#include <cstdio>
#include <algorithm>
#include "utils/Utils.hpp"
#include <cassert>

#ifdef _MSC_VER
#include "win32/gettimeofday.h"
#else
#include <sys/time.h>
#endif

#ifdef _WIN32
#include <windows.h>
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

using namespace std;

//===========================================================================
// return the current timestamp in format dd/mm/yyyy hh:mm:ss
//===========================================================================
string ccruncher::Utils::timestamp()
{
  time_t now = time(NULL);
  tm lt = *(localtime(&now));

  char aux[] = "dd/mm/yyyy hh:mm:ss ";
  aux[19] = 0;

  snprintf(aux, 20, "%02d/%02d/%04d %02d:%02d:%02d", lt.tm_mday, lt.tm_mon+1, lt.tm_year+1900, lt.tm_hour, lt.tm_min, lt.tm_sec);

  return string(aux);
}

//===========================================================================
// returns a random value based on current time
//===========================================================================
unsigned long ccruncher::Utils::trand()
{
  timeval tv;
  gettimeofday(&tv,NULL);
  return (unsigned long)(tv.tv_sec)*1000000UL + (unsigned long)(tv.tv_usec);
}

//===========================================================================
// return a string with compilation options
//===========================================================================
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

//===========================================================================
// return the number of cores
//===========================================================================
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

//===========================================================================
// tokenize
//===========================================================================
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

//===========================================================================
// trim
//===========================================================================
string ccruncher::Utils::trim(const string &s)
{
  if (s.length() == 0) return "";

  int pos1 = s.length()-1;
  for(int i=0; i<(int)s.length(); i++) {
    if (!isspace(s[i])) {
      pos1=i;
      break;
    }
  }

  int pos2 = 0;
  for(int i=(int)s.length()-1; i>=0; i--) {
    if (!isspace(s[i])) {
      pos2=i;
      break;
    }
  }

  if (pos2 < pos1) {
    return "";
  }
  else if (pos1 == pos2) {
    if (isspace(s[pos1])) return "";
    else return s.substr(pos1,1);
  }
  else {
    return s.substr(pos1,pos2-pos1+1);
  }
}

//===========================================================================
// uppercase
//===========================================================================
string ccruncher::Utils::uppercase(const string &str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) toupper);

  return res;
}

//===========================================================================
// lowercase
//===========================================================================
string ccruncher::Utils::lowercase(const string &str)
{
  string res = str;

  transform(res.begin(), res.end(), res.begin(), (int(*)(int)) tolower);

  return res;
}

//===========================================================================
// filler
//===========================================================================
string ccruncher::Utils::filler(int n, char c)
{
  return string(n, c);
}

//===========================================================================
// blanks
//===========================================================================
string ccruncher::Utils::blanks(int n)
{
  return filler(n, ' ');
}

