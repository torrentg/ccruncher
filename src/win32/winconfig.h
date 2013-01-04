
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

#ifndef WINCONFIG_H
#define WINCONFIG_H

// ======================================================
// patch against M_PI undeclaration
// ======================================================
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

// ======================================================
// patch against isnan() function
// ======================================================
#ifdef _MSC_VER
#include <cfloat>
#define isnan _isnan
#endif

// ======================================================
// patch against NAN problem
// Visual C++ etc lacks NAN, and won't accept 0.0/0.0.  
// NAN definition from:
// http://msdn.microsoft.com/en-us/library/w22adx1s%28v=VS.100%29.aspx
// ======================================================
#if !defined(NAN)
static const unsigned long _nan_[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const double *) _nan_)
#endif

// ======================================================
// patch against getopt problem
// properties->configuration properties->C/C++->
// preprocessor->preprocessor definitions (add BUILD_GETOPT)
// ======================================================
#ifdef _MSC_VER
#include "getopt.h"
#endif

// ======================================================
// patch against opendir & closedir problems
// properties->configuration properties->C/C++->
// preprocessor->preprocessor definitions (add BUILD_DIRENT)
// ======================================================
#ifdef _MSC_VER
#include "dirent.h"
#endif

// ======================================================
// patch against gettimeofday problems
// called directly in Timer.hpp
// includes <Windows.h> call
// ======================================================
//#include "gettimeofday.h"

// ======================================================
// patch against mkdir & getcwd problems
// ======================================================
#ifdef _MSC_VER
#include <direct.h>
#define getcwd _getcwd
#define mkdir _mkdir
#endif

// ======================================================
// patch against snprintf problem
// ======================================================
#ifdef _MSC_VER
#define snprintf _snprintf
#endif

// ======================================================
// patch against access problem
// ======================================================
#include <io.h>
#define access _access
#ifndef R_OK
  #define R_OK 04
#endif
#ifndef W_OK
  #define W_OK 02
#endif

// ======================================================
// disabling warnings
// ======================================================
#if defined(_MSC_VER)
#pragma warning( disable : 4290 )
#endif

#endif

