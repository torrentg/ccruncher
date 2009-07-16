
//===========================================================================
//
// CreditCruncher - A portfolio credit risk valorator
// Copyright (C) 2004-2009 Gerard Torrent
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

#ifndef WINCONFIG_H
#define WINCONFIG_H

// ======================================================
// patch against NAN problem
// ======================================================
#include <cmath>
#include <cfloat>
#define NAN sqrt(-1.0)
#define isnan _isnan

// ======================================================
// patch against infinity problem
// ======================================================
#define INFINITY HUGE_VAL
#define isinf(x) !_finite(x)

// ======================================================
// define some special functions (caution, low accuracy?)
// ======================================================
#define log1p(x) log(1.0 + x)
#define expm1(x) (exp(x)-1.0)

// ======================================================
// patch against round() function
// ======================================================
inline double round( double d ) 
{ 
  return floor( d + 0.5 ); 
}

// ======================================================
// patch against trunc() function
// ======================================================
inline double trunc( double d ) 
{ 
  if (d >= 0) return floor(d);
  else return -floor(-d);
}

// ======================================================
// patch against getopt problem
// properties->configuration properties->C/C++->
// preprocessor->preprocessor definitions (add BUILD_GETOPT)
// ======================================================
#include "getopt.h"

// ======================================================
// patch against opendir & closedir problems
// properties->configuration properties->C/C++->
// preprocessor->preprocessor definitions (add BUILD_DIRENT)
// ======================================================
#include "dirent.h"

// ======================================================
// patch against gettimeofday problems
// called directly in Timer.hpp
// includes <Windows.h> call
// ======================================================
//#include "gettimeofday.h"

// ======================================================
// patch against mkdir & getcwd problems
// ======================================================
#include <direct.h>
#define getcwd _getcwd
#define mkdir _mkdir

// ======================================================
// patch against access problem
// ======================================================
#include <io.h>
#define access _access
#define R_OK 04
#define W_OK 02

// ======================================================
// disabling warnings
// ======================================================
#pragma warning( disable : 4290 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4146 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4700 )
#pragma warning( disable : 4800 )
#pragma warning( disable : 4996 )

#endif

