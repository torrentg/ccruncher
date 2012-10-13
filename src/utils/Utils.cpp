
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

#include <cstdio>
#include <ctime>
#include "utils/Utils.hpp"
#include <cassert>

#ifdef _MSC_VER
#include "win32/gettimeofday.h"
#else
#include <sys/time.h>
#endif

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
long ccruncher::Utils::trand()
{
  timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec*1000000L + long(tv.tv_usec);
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

