
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
// Utils.cpp - Utils code - $Rev$
// --------------------------------------------------------------------------
//
// 2005/07/19 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
// 2005/07/24 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added hash() method
//   . added timestamp() method
//
// 2005/07/30 - Gerard Torrent [gerard@fobos.generacio.com]
//   . moved <cassert> include at last position
//
// 2005/08/06 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added method getCompilationOptions()
//
// 2005/10/15 - Gerard Torrent [gerard@fobos.generacio.com]
//   . added Rev (aka LastChangedRevision) svn tag
//
//===========================================================================

#include <iostream>
#include <cstdio>
#include <ctime>
#include "utils/Utils.hpp"
#include <cassert>

#ifdef USE_MPI
  #include <mpi.h>
#endif

// --------------------------------------------------------------------------

ofstream *ccruncher::Utils::nullstream = NULL;
int ccruncher::Utils::rankid = -1;

//===========================================================================
// isMaster
//===========================================================================
bool ccruncher::Utils::isMaster()
{
#ifdef USE_MPI
  // if rankid uninitialized
  if (rankid < 0)
  {
    rankid = MPI::COMM_WORLD.Get_rank();
  }

  if (rankid > 0)
  {
    return false;
  }
  else
  {
    return true;
  }
#else
  return true;
#endif
}

//===========================================================================
// setSilentMode. Send cout and cerr to /dev/null (oblivion)
//===========================================================================
void ccruncher::Utils::setSilentMode() throw(Exception)
{
  if (nullstream == NULL)
  {
    nullstream = new ofstream("/dev/null");
  }

  if (!nullstream->good())
  {
    throw Exception("problems removing output on slaves nodes");
  }
  else
  {
    // closing C++ streams
    cout.rdbuf(nullstream->rdbuf());
    cerr.rdbuf(nullstream->rdbuf());

    // closing C streams (used by getop)
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
  }
}

//===========================================================================
// create a hash using ELF hash algorithm
// extracted from "Hashing Rehashed." Andrew Binstock, Dr. Dobb's Journal, APR96
//===========================================================================
unsigned long ccruncher::Utils::hash(const string &str)
{
  unsigned long h=0, g;
  const unsigned char *name = (const unsigned char *)(str.c_str());

  while ( *name )
  {
    h = ( h << 4 ) + *name++;
    if ( (g = h & 0xF0000000) )
      h ^= g >> 24;
    h &= ~g;
  }
  return h;
}

//===========================================================================
// return the current timestamp in format dd/mm/yyyy hh:mm:ss
//===========================================================================
string ccruncher::Utils::timestamp()
{
  time_t now = time(NULL);
  tm lt = *(localtime(&now));

  char aux[] = "dd/mm/yyyy hh:mm:ss ";
  aux[19] = 0;

  sprintf(aux, "%02d/%02d/%04d %02d:%02d:%02d", lt.tm_mday, lt.tm_mon+1, lt.tm_year+1900, lt.tm_hour, lt.tm_min, lt.tm_sec);

  return string(aux);
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
  ret += "[enabled] | ";
#else
  ret += "[disabled] | ";
#endif

  // MPI option
  ret += "MPI";
#ifdef USE_MPI
  ret += "[enabled]";
#else
  ret += "[disabled]";
#endif

  return ret;
}
