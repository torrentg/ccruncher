
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
// patch against round() function
// ======================================================
inline double round( double d ) 
{ 
  return floor( d + 0.5 ); 
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

#endif
