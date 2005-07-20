
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
// Montecarlo.cpp - MonteCarlo code
// --------------------------------------------------------------------------
//
// 2005/07/19 - Gerard Torrent [gerard@fobos.generacio.com]
//   . initial release
//
//===========================================================================

#include <cassert>
#include <iostream>
#include <cstdio>
#include "utils/Utils.hpp"

#ifdef USE_MPI
  #include <mpi.h>
#endif

// --------------------------------------------------------------------------

ofstream *ccruncher::Utils::nullstream = NULL;

//===========================================================================
// isMaster
//===========================================================================
bool ccruncher::Utils::isMaster()
{
#ifdef USE_MPI
  if (MPI::COMM_WORLD.Get_rank() == 0)
  {
    return true;
  }
  else
  {
    return false;
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
